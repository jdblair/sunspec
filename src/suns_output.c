/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output.c
 *
 * output functions for the various output formats
 *
 * Copyright (c) 2011-2012, John D. Blair <jdb@moship.net>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of John D. Blair nor his lackeys may be used
 *       to endorse or promote products derived from this software
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JOHN D. BLAIR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "trx/macros.h"
#include "trx/string.h"
#include "trx/date.h"
#include "trx/debug.h"
#include "suns_model.h"
#include "suns_output.h"
#include "suns_parser.h"

/* list of model output formats */
static suns_model_list_export_format_t suns_model_list_export_formats[] = {
    /*     { "slang",   suns_model_export_all }, */
    { "xml",     suns_model_xml_export_all },
    { NULL, NULL }
};

static suns_model_export_format_t suns_model_export_formats[] = {
    { "slang", suns_model_fprintf },
    /*    { "sql",   suns_model_sql_fprintf }, */
    { "csv",   suns_model_csv_fprintf },
    { "xml",   suns_model_xml_fprintf },
    { NULL, NULL }
};

/* list of dataset output functions */
static suns_dataset_output_format_t suns_dataset_output_formats[] = {
    { "text", suns_dataset_text_fprintf },
    /*    { "sql",  suns_dataset_sql_fprintf }, */
    /*    { "csv",  suns_dataset_csv_fprintf }, */
    { "xml",  suns_dataset_xml_fprintf },
    { NULL, NULL }
};

static suns_device_output_format_t suns_device_output_formats[] = {
    { "text",  suns_device_text_fprintf },
    { "xml",  suns_device_xml_fprintf },
    { NULL, NULL }
};

/* output a model in the given type */
int suns_model_export(FILE *stream, char *type, suns_model_t *model)
{
    assert(type);
    assert(model);
    assert(stream);
    
    int i;
    
    for (i = 0; suns_model_export_formats[i].name != NULL; i++) {
        debug("i = %d", i);
        if (strcmp(suns_model_export_formats[i].name, type) == 0) {
            suns_model_export_formats[i].fprintf(stream, model);
            return 0;
        }
    }
    
    debug("export format %s is not defined", type);
    return -1;
}


/* output all the models in a list_t in the given type */
int suns_model_export_all(FILE *stream, char *type,
                          list_t *model_list, list_t *define_list)
{
    int i;
    list_node_t *c;

    for (i = 0; suns_model_list_export_formats[i].name != NULL; i++) {
        if (strcmp(suns_model_list_export_formats[i].name, type) == 0)
            return suns_model_list_export_formats[i].fprintf(stream,
                                                             type,
                                                             model_list,
                                                             define_list);
    }

    debug("export format %s is not defined", type);

    list_for_each(model_list, c) {
        if (suns_model_export(stream, type, c->data) < 0)
            return -1;
    }

    list_for_each(define_list, c) {
        suns_define_block_fprint(stream, c->data);
    }

    return 0;
}


int suns_model_export_slang_all(FILE *stream, char *type,
                                list_t *model_list, list_t *define_list)
{
    int rc = 0;

    return rc;
}


/* output a dataset */
int suns_dataset_output(char *fmt, suns_dataset_t *data, FILE *stream)
{
    assert(fmt);
    assert(data);
    assert(stream);
    
    int i;
    suns_dataset_output_format_t *output = NULL;

    for (i = 0; suns_dataset_output_formats[i].name != NULL; i++) {
        debug("i = %d", i);
        if (strcmp(suns_dataset_output_formats[i].name, fmt) == 0) {
            output = &(suns_dataset_output_formats[i]);
            break;
        }
    }

    if (output == NULL) {
        error("output format %s is not defined", fmt);
        return -1;
    }

    return 0;
}


int suns_device_output(char *fmt, suns_device_t *device, FILE *stream)
{
    assert(device);
    
    int rc = 0;
    list_node_t *c;
    int i;
    suns_device_output_format_t *output = NULL;

    for (i = 0; suns_device_output_formats[i].name != NULL; i++) {
        debug("i = %d", i);
        if (strcmp(suns_device_output_formats[i].name, fmt) == 0) {
            output = &(suns_device_output_formats[i]);
            break;
        }
    }

    if (output != NULL) {
        return output->fprintf(stream, device);
    }

    /* no device output format is defined; fall back to just
       calling suns_dataset_output() for each dataset */
    list_for_each(device->datasets, c) {
        rc = suns_dataset_output(fmt, c->data, stream);
        if (rc < 0)
            break;
    }
    
    return rc;
}

    
void suns_dp_fprint(FILE *stream, suns_dp_t *dp)
{
    fprintf(stream, "    %-32s { ", dp->name);
    if (dp->offset > -1)
        fprintf(stream, "%3d ", dp->offset);
    suns_type_pair_fprint(stream, dp->type_pair);
    
    list_node_t *c;
    if (dp->attributes) {
        fprintf(stream, " ");
        list_for_each(dp->attributes, c) {
            suns_attribute_fprintf(stream, c->data, 0);
        }
    }
    fprintf(stream, " }\n");
}


void suns_attribute_fprintf(FILE *stream, suns_attribute_t *a, int offset)
{

    fprintf(stream, "%s", a->name);
    if (a->list) {
        list_node_t *c;
        fprintf(stream, "={ ");
        /* typographical offset for pretty printing */
        int i;
        for(i = 0; i < offset; i++) {
            fprintf(stream, " ");
        }
        list_for_each(a->list, c) {
            suns_attribute_fprintf(stream, c->data, offset + 2);
        }
        fprintf(stream, "} ");
    } else {
        if (a->value)
            fprintf(stream, "=\"%s\" ", a->value);
        else
            fprintf(stream, " ");
    }
}

/**
 * Dump the provided data as a string of 16 bit unsigned hex values.
 * If there is an odd number of bytes the data is padded.  (it would
 * be better if *buf was an array o uint16_t, but using unsigned char
 * the function compatible with the code that is calling.
 */
void suns_registers_fprintf(FILE * stream,
                            unsigned char *buf, size_t len, char *line_prefix)
{
    int i;
    int odd;

    int start_byte = 0;

    /* is len an even number? */
    odd = len % 2;

    fprintf(stream, "%s", line_prefix);
    for (i = 0; i < len - odd; i++) {
        if ((i + 1) % 2)
            fprintf(stream, "0x%02x", buf[i]);
        else
            fprintf(stream, "%02x ", buf[i]);
        if (((i + 1) % 16) == 0) {
            fprintf(stream, "  # %03d - %03d\n%s",
                    (start_byte / 2) + 1, (i / 2) + 1, line_prefix);
            start_byte = i + 1;
        }
    }
    
    if (odd)
        fprintf(stream, "0x%02x00", buf[len]);
    
    fprintf(stream, "  # %03d - %03d",
            (start_byte / 2) + 1, ((i - 1) / 2) + 1);
    if (odd)
        fprintf(stream, "  (padded one byte to align)\n");

    fprintf(stream, "%s\n", line_prefix);
}


/**
 * Output a complete sunspec model as a list of 16 bit hex values,
 * formatted in slang model format which can be loaded into the
 * test slave mode for testing.
 *
 */
void suns_binary_model_fprintf(FILE *stream, list_t *did_list,
                              unsigned char *buf, size_t len)
{
    suns_model_did_t *did;
    static char line_prefix[] = "  ";
    
    /* the first 2 bytes contain the did */
    uint16_t did_value = be16toh(*((uint16_t *)buf));

    did = suns_find_did(did_list, did_value);

    fprintf(stream, "data captured_%d {\n", did_value);
    if (did) {
        fprintf(stream, "%s# did %d: %s model\n",
                line_prefix, did->did, did->name);
    } else {
        fprintf(stream, "%s# unknown model block %d\n",
                line_prefix, did_value);
    }
    suns_registers_fprintf(stream, buf, len, line_prefix);
    fprintf(stream, "}\n");
}


/**********************************************************************
 *
 * common string output functions
 *
 **********************************************************************/

int suns_snprintf_type_pair(char *str, size_t size, suns_type_pair_t *tp)
{
    int len = 0;

    len += snprintf(str + len, size - len, "%s", suns_type_string(tp->type));
    if (len > size)
        return len;
    
    if (tp->type == SUNS_STRING)
        len += snprintf(str + len, size - len, ".%d", tp->len);
    else if (tp->sf != 0)
        len += snprintf(str + len, size - len, ".%d", tp->sf);
    else if (tp->name)
        len += snprintf(str + len, size - len, ".%s", tp->name);

    return len;
}

void suns_type_pair_fprint(FILE *stream, suns_type_pair_t *tp)
{
    char buf[BUFFER_SIZE];
    
    suns_snprintf_type_pair(buf, BUFFER_SIZE, tp);
    fprintf(stream, "%s", buf);
}


/*
 *  WARNING! there may be some loss of precision in scale factor
 *  conversions since these cause values to be converted to doubles
 *  before being displayed.
 * 
 *  FIXME: I should write a decimal string output function, which
 *  would keep the int values as integers and move the decimal
 *  point around symbolically.
 */


static int value_output_null(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "null type");
}

static int value_output_undef(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "undef type");
}

static int value_output_int16(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%d", v->value.i16);    
}

static int value_output_int16_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_int_sf(buf, len, v->value.i16, v->tp.sf, 16);
}
    
static int value_output_uint16(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%u", v->value.u16);
}

static int value_output_uint16_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_uint_sf(buf, len, v->value.u16, v->tp.sf, 16);
}

static int value_output_int32(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%d", v->value.i32);
}

static int value_output_int32_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_int_sf(buf, len, v->value.i32, v->tp.sf, 16);
}

static int value_output_uint32(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%u", v->value.u32);
}

static int value_output_uint32_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_uint_sf(buf, len, v->value.u32, v->tp.sf, 16);
}

static int value_output_float32(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%f", v->value.f32);
}

static int value_output_float32_sf(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%g", v->value.f32 * pow(10, v->tp.sf));
}

static int value_output_float64(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%f", v->value.f64);
}

static int value_output_float64_sf(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%g", v->value.f64 * pow(10, v->tp.sf));
}

static int value_output_hex_uint16(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "0x%04x", v->value.u16);
}

static int value_output_hex_uint32(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "0x%08x", v->value.u32);
}

static int value_output_string(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%s", v->value.s);
}

static int value_output_meta(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%s", suns_value_meta_string(v->meta));
}

static int value_output_int64(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%lld", v->value.i64);    
}

static int value_output_int64_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_int_sf(buf, len, v->value.i64, v->tp.sf, 16);
}
    
static int value_output_uint64(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%llu", v->value.u64);
}

static int value_output_uint64_sf(char *buf, size_t len, suns_value_t *v)
{
    return suns_snprintf_uint_sf(buf, len, v->value.u64, v->tp.sf, 16);
}


/* convert a value to a string using the supplied
   suns_value_output_vector_t vector table */
int suns_snprintf_value(char *str, size_t size,
                        suns_value_t *v, suns_value_output_vector_t *fmt)
{
    int len = 0;

    /* check for meta condition */
    if (v->meta != SUNS_VALUE_OK)
        return fmt->meta(str, size, v);

    /* convert value to string */
    switch (v->tp.type) {
    case SUNS_NULL:
        return fmt->null(str, size, v);

    case SUNS_UNDEF:
        return fmt->undef(str, size, v);

    case SUNS_INT16:
        return fmt->int16(str, size, v);

    case SUNS_SF: 
        return fmt->sunssf(str, size, v);

    case SUNS_INT32:
        return fmt->int32(str, size, v);

    case SUNS_ENUM16:
        return fmt->enum16(str, size, v);

    case SUNS_ENUM32:
        return fmt->enum32(str, size, v);

    case SUNS_UINT16:
        return fmt->uint16(str, size, v);

    case SUNS_ACC16:
        return fmt->acc16(str, size, v);

    case SUNS_UINT32:
        return fmt->uint32(str, size, v);

    case SUNS_ACC32:
        return fmt->acc32(str, size, v);

    case SUNS_FLOAT32:
        return fmt->float32(str, size, v);

    case SUNS_FLOAT64:
        return fmt->float64(str, size, v);

    case SUNS_BITFIELD16:
        return fmt->bitfield16(str, size, v);

    case SUNS_BITFIELD32:
        return fmt->bitfield32(str, size, v);

    case SUNS_STRING:
        return fmt->string(str, size, v);

    case SUNS_INT64:
        return fmt->int64(str, size, v);

    case SUNS_UINT64:
        return fmt->int64(str, size, v);

    case SUNS_ACC64:
        return fmt->acc64(str, size, v);

    default:
        len += snprintf(str + len, size - len,
                        " unknown type %2d", v->tp.type);
    }

    return len;
}


/* This is the default set of string conversion functions for a
   suns_value.  All of the actual output functions "inherit" from
   this one by copying it and modifying the function pointers it
   wants to over-ride.
*/
suns_value_output_vector_t suns_output_value_base_fmt = {
    .null       =  value_output_null,
    .undef      =  value_output_undef,
    .int16      =  value_output_int16,
    .uint16     =  value_output_uint16,
    .acc16      =  value_output_uint16,
    .int32      =  value_output_int32,
    .uint32     =  value_output_uint32,
    .float32    =  value_output_float32,
    .acc32      =  value_output_int32,
    .enum16     =  value_output_uint16,
    .enum32     =  value_output_uint32,
    .bitfield16 =  value_output_hex_uint16,
    .bitfield32 =  value_output_hex_uint32,
    .int64      =  value_output_int64,
    .uint64     =  value_output_uint64,
    .acc64      =  value_output_uint64,
    .float64    =  value_output_float64,
    .sunssf     =  value_output_int16,
    .string     =  value_output_string,
    .meta       =  value_output_meta,
};



/**********************************************************************
 *
 * slang model notation format
 *
 **********************************************************************/

void suns_model_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c;

    fprintf(stream, "model suns {\n");
    if (model->did_list) {
        list_for_each(model->did_list, c) {
            suns_model_did_t *did = c->data;
            fprintf(stream, "  did %03d \"%s\"\n", did->did, did->name);
        }
    }
    fprintf(stream, "  len %d\n", model->len);
    if (verbose_level > 0) {
        fprintf(stream, "  #  base_len = %d\n", model->base_len);
    }

    /* list_for_each(model->did_list, c) { */
    list_for_each(model->dp_blocks, c) {
        suns_dp_block_fprintf(stream, c->data);
    }

    /* defines - enums & bitfields */
    list_for_each(model->defines, c) {
        suns_define_block_fprint(stream, c->data);
        fprintf(stream, "\n");
    }

    fprintf(stream, "}\n\n");
}


void suns_dp_block_fprintf(FILE *stream, suns_dp_block_t *dp_block)
{
    list_node_t *c;

    fprintf(stream, "  datapoints ");

    if (dp_block->repeating) {
        fprintf(stream, "repeating ");
    }

    fprintf(stream, "{\n");
    if (verbose_level > 0) {
        fprintf(stream, "  # len %d\n", dp_block->len);
    }
    if (dp_block->dp_list) {
        list_for_each(dp_block->dp_list, c) {
            suns_dp_fprint(stream, c->data);
        }
    }
    fprintf(stream, "  }\n\n");

}


void suns_define_block_fprint(FILE *stream, suns_define_block_t *block)
{
    list_node_t *c;

    fprintf(stream, "  define %s {\n", block->name);
    list_for_each(block->list, c) {
        suns_define_fprint(stream, c->data);
    }
    fprintf(stream, " }\n");
}


void suns_define_fprint(FILE *stream, suns_define_t *define)
{
    fprintf(stream, "    %-32s { ", define->name);
    if (define->attributes) {
        list_node_t *c;
        list_for_each(define->attributes, c) {
            suns_attribute_fprintf(stream, c->data, 0);
        }
    } else {
        fprintf(stream, "%2d \"%s\"", define->value, define->string);
    }
    fprintf(stream, " }\n");
}



/**********************************************************************
 *
 * text format
 *
 **********************************************************************/

int suns_snprintf_value_text(char *str, size_t size,
                             suns_value_t *v)
{
    suns_value_output_vector_t fmt = suns_output_value_base_fmt;
    return suns_snprintf_value(str, size, v, &fmt); 
}


/* apply scale factors */
int suns_snprintf_value_sf_text(char *str, size_t size,
                                suns_value_t *v)
{
    suns_value_output_vector_t fmt = suns_output_value_base_fmt;

    /* override numeric values with functions which apply scale factors */
    fmt.int16  = value_output_int16_sf;
    fmt.uint16 = value_output_uint16_sf;
    fmt.acc16 = value_output_uint16_sf;
    fmt.int32  = value_output_int32_sf;
    fmt.uint32 = value_output_uint32_sf;
    fmt.acc32 = value_output_uint32_sf;
    fmt.float32  = value_output_float32_sf;
    fmt.int64  = value_output_int64_sf;
    fmt.uint64 = value_output_uint64_sf;
    fmt.acc64 = value_output_uint64_sf;
    fmt.float64  = value_output_float64_sf;
    
    return suns_snprintf_value(str, size, v, &fmt);
}


void suns_data_fprintf(FILE *stream, suns_data_t *data)
{
    fprintf(stream, "%d (%d): ", data->offset, buffer_len(data->data));
    buffer_dump(data->data);
}


void suns_data_block_fprintf(FILE *stream, suns_data_block_t *block)
{
    fprintf(stream, "data_block: %s\n", block->name);
    buffer_dump(block->data);
}
   

int suns_dataset_text_fprintf(FILE *stream, suns_dataset_t *data)
{
    assert(stream);
    assert(data);

    list_node_t *c;
    char scaled_value_buf[BUFFER_SIZE];
    
    fprintf(stream, "data model: %s(%d)\n",
            data->did->name,
            data->did->did);
    
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;

        /* don't display scale factors unless verbose_level > 1 */
        if ((verbose_level < 1) && (v->tp.type == SUNS_SF))
            continue;

        /* don't display not-implemented values unless verbose_level > 1 */
        if ((verbose_level < 1) && (v->meta == SUNS_VALUE_NOT_IMPLEMENTED))
            continue;

        suns_snprintf_value_sf_text(scaled_value_buf, BUFFER_SIZE, v);
        if (v->repeating) {
            fprintf(stream, "  %02d:", v->index);
        } else {
            fprintf(stream, "     ");
        }
        fprintf(stream, "%-30s%20s",
                (v->label ? v->label : v->name),
                scaled_value_buf);
        
        if (v->units)
            fprintf(stream, " %s", v->units);

        /* display enum values */
        if (((v->tp.type == SUNS_ENUM16) ||
             (v->tp.type == SUNS_ENUM32)) &&
            (v->tp.name != NULL)) {
            suns_define_block_t *b;
            b = suns_search_define_blocks(data->did->model->defines,
                                          v->tp.name);
                
            if (b) {
                int value;
                if (v->tp.type == SUNS_ENUM16)
                    value = suns_value_get_enum16(v);
                else
                    value = suns_value_get_enum32(v);

                suns_define_t *d =
                    suns_search_enum_defines(b->list, value);
                if (d) {
                    fprintf(stream, " %s", d->name);
                }
            }
        }

        fprintf(stream, "\n");
    }    
    fprintf(stream, "\n");

    return 0;
}


int suns_device_text_fprintf(FILE *stream, suns_device_t *device)
{
    int rc = 0;
    list_node_t *c;
    char timestamp[BUFFER_SIZE];
    
    /* set date to now */
    date_snprintf_rfc3339(timestamp, BUFFER_SIZE,
                          device->unixtime, device->usec);
    fprintf(stream, "Timestamp: %s\n\n", timestamp);
    
    list_for_each(device->datasets, c) {
        suns_dataset_t *d = c->data;
        
        rc = suns_dataset_text_fprintf(stream, d);
        if (rc < 0)
            break;
    }

    return rc;
}   

/**********************************************************************
 *
 * sql format
 *
 **********************************************************************/

static int value_output_sql_string(char *buf, size_t len, suns_value_t *v)
{
    char tmp[BUFFER_SIZE];
    string_escape_sql(v->value.s, tmp, len);
    return snprintf(buf, len, "'%s'", tmp);
}

static int value_output_sql_null(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "NULL");
}


int suns_snprintf_value_sql(char *str, size_t size,
                             suns_value_t *v)
{
    suns_value_output_vector_t fmt = suns_output_value_base_fmt;

    /* escape single quotes in strings */
    fmt.string = value_output_sql_string;

    /* NULL for all meta values other than SUNS_VALUE_OK */
    fmt.meta   = value_output_sql_null;

    return suns_snprintf_value(str, size, v, &fmt);
}


void suns_model_sql_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c, *d;

    fprintf(stream, "CREATE TABLE IF NOT EXISTS %s\n", model->name);
    fprintf(stream, "(t,man,mod,sn");
    
    list_for_each(model->dp_blocks, d) {
        suns_dp_block_t *dp_block = d->data;

        /* end base table definition and start the repeating table */
        if (dp_block->repeating) {
            fprintf(stream, ");\n\n");
            fprintf(stream, "CREATE TABLE IF NOT EXISTS %s_repeat\n",
                    model->name);
            fprintf(stream, "(header_id,index");
        }

        list_for_each(dp_block->dp_list, c) {
            suns_dp_t *dp = c->data;
            fprintf(stream, ",%s", dp->name);
        }
    }
    fprintf(stream, ");\n\n");
}


int suns_dataset_sql_fprintf(FILE *stream, suns_dataset_t *data)
{
    list_node_t *c;

    suns_model_t *m = data->did->model;
    
    fprintf(stream, "INSERT INTO %s (unixtime,timestamp,addr",
            m->name);
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;
        fprintf(stream, ",%s", v->name);
    }

    /* FIXME: need to store actual time values, not spacers */
    fprintf(stream, ") VALUES (%d,'%s',%d", 0, "NULL", 0);
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;
        char buf[BUFFER_SIZE];
        /* FIXME: should store NULL instead of "not implemented" */
        suns_snprintf_value_sql(buf, BUFFER_SIZE, v);
        /* FIXME: should store numeric values, not strings */
        fprintf(stream, ",\"%s\"", buf);
    }
    fprintf(stream, ");\n");
    
    return 0;
}



/**********************************************************************
 *
 * csv format
 *
 **********************************************************************/

static int value_output_csv_string(char *buf, size_t len, suns_value_t *v)
{
    char tmp[BUFFER_SIZE];
    string_escape_cstyle(v->value.s, tmp, len);
    return snprintf(buf, len, "\"%s\"", tmp);
}

static int value_output_csv_null(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "NULL");
}


int suns_snprintf_value_csv(char *str, size_t size,
                             suns_value_t *v)
{
    suns_value_output_vector_t fmt = suns_output_value_base_fmt;

    /* escape quotes */
    fmt.string     =  value_output_csv_string;
    /* NULL for all other than SUNS_VALUE_OK */
    fmt.meta       =  value_output_csv_null;

    return suns_snprintf_value(str, size, v, &fmt);
}


void suns_model_csv_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c, *d;

    fprintf(stream, "%s\n", model->name);
    fprintf(stream, "offset,size,name,type,units,scale_factor\n");
    /*    fprintf(stream, "notation,block_offset,size,word_offset,byte_offset,rw,name,type,units,contents,description\n"); */
    list_for_each(model->dp_blocks, d) {
        suns_dp_block_t *dp_block = d->data;
        list_for_each(dp_block->dp_list, c) {
            suns_dp_t *dp = c->data;
            int size = suns_type_pair_size(dp->type_pair);
            char sf[BUFFER_SIZE];
            char *units;

            /* fill in scale factor if necessary */
            if (dp->type_pair->name) {
                snprintf(sf, BUFFER_SIZE, "%s", dp->type_pair->name);
            } else if (dp->type_pair->sf != 0) {
                snprintf(sf, BUFFER_SIZE, "%d", dp->type_pair->sf);
            } else {
                sf[0] = '\0';
            }

            /* units */
            units = suns_find_attribute(dp, "u");
            if (! units)
                units = "";

            fprintf(stream, "%d,%d,%s,%s,%s\n",
                    /* start */          dp->offset,
                    /* size */           size / 2,
                    /* name */           dp->name,
                    /* type */           suns_type_string(dp->type_pair->type),
                    /* scale factor */   sf);
        }
    }
    fprintf(stream, "\n");
}

int suns_dataset_csv_fprintf(FILE *stream, suns_dataset_t *data)
{
    list_node_t *c;
    /* suns_model_t *m = data->did->model; */
    
    fprintf(stream, "unixtime,timestamp,addr");
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;
        fprintf(stream, ",%s", v->name);
    }
    /* FIXME: need to store actual time values, not spacers */
    fprintf(stream, "%d,\"%s\",%d", 0, "NULL", 0);
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;
        char buf[BUFFER_SIZE];
        suns_snprintf_value_csv(buf, BUFFER_SIZE, v);
        /* FIXME: should store numeric values, not strings */
        fprintf(stream, ",%s", buf);
    }
    fprintf(stream, "\n\n");

    return 0;
}
 

/**********************************************************************
 *
 * sunspec logger xml format
 *
 **********************************************************************/

static int value_output_xml_string(char *buf, size_t len, suns_value_t *v)
{
    return string_escape_xml(v->value.s, buf, len);
}

static int value_output_xml_meta(char *buf, size_t len, suns_value_t *v)
{
    return snprintf(buf, len, "%s", suns_value_meta_string(v->meta));
}

int suns_snprintf_value_xml(char *str, size_t size,
                             suns_value_t *v)
{
    suns_value_output_vector_t fmt = suns_output_value_base_fmt;
    fmt.string     =  value_output_xml_string;
    fmt.meta       =  value_output_xml_meta;

    /* xml data format does not allow values in hex */
    fmt.bitfield16 =  value_output_uint16;
    fmt.bitfield32 =  value_output_uint32;

    return suns_snprintf_value(str, size, v, &fmt);
}



int suns_dataset_xml_fprintf(FILE *stream, suns_dataset_t *data)
{
    list_node_t *c;
    char value[BUFFER_SIZE];
    
    fprintf(stream, "   <m id=\"%d\"", data->did->did);
    if (data->index != 0)
        fprintf(stream, " x=\"%d\"", data->index);
    fprintf(stream, ">\n");

    list_for_each(data->values, c) {
        suns_value_t *v = c->data;

        /* skip scale factors and "not implemented" values */
        if ((v->tp.type == SUNS_SF) ||
            (v->meta == SUNS_VALUE_NOT_IMPLEMENTED))
            continue;

        /* do not apply scale factor here */
        suns_snprintf_value_xml(value, BUFFER_SIZE, v);
        fprintf(stream, "    <p id=\"%s\"", v->name);
        if (v->tp.sf != 0)
            fprintf(stream, " sf=\"%d\"", v->tp.sf);
        else
            debug("no scale factor for %s", v->name);
        
        if (v->repeating)
            fprintf(stream, " x=\"%d\"", v->index + 1);
        fprintf(stream, ">%s</p>\n", value);
    }
    fprintf(stream, "   </m>\n");
    
    return 0;
}


int suns_device_xml_fprintf(FILE *stream, suns_device_t *device)
{
    int rc = 0;
    list_node_t *c;
    char safe_string[BUFFER_SIZE];
    
    /* root element */
    /*    fprintf(stream, "<sunSpecData v=\"1\" xmlns=\"http://www.sunspec.org/data/v1\">\n"); */
    fprintf(stream, "<sunSpecData v=\"1\">\n");

    fprintf(stream, " <d");

    if (device->lid) {
        string_escape_xml(device->lid, safe_string, BUFFER_SIZE);
        fprintf(stream, " lid=\"%s\"", safe_string);
    }

    if (device->ns) {
        string_escape_xml(device->ns, safe_string, BUFFER_SIZE);
        fprintf(stream, " ns=\"%s\"", safe_string);
    }        
    
    if (device->manufacturer) {
        string_escape_xml(device->manufacturer, safe_string, BUFFER_SIZE);
        fprintf(stream, " man=\"%s\"", safe_string);
    }
    
    if (device->model) {
        string_escape_xml(device->model, safe_string, BUFFER_SIZE);
        fprintf(stream, " mod=\"%s\"", safe_string);
    }
    
    if (device->serial_number) {
        string_escape_xml(device->serial_number, safe_string, BUFFER_SIZE);
        fprintf(stream, " sn=\"%s\"", safe_string);
    }
    
    /* set date to now */
    date_snprintf_rfc3339_z(safe_string, BUFFER_SIZE,
                            device->unixtime, device->usec);
    fprintf(stream, " t=\"%s\">\n", safe_string);
    
    list_for_each(device->datasets, c) {
        suns_dataset_t *d = c->data;

        /* don't include common model block */
        /* if (d->did->did == 1)
           continue; */
        
        rc = suns_dataset_xml_fprintf(stream, d);
        if (rc < 0)
            break;
    }

    fprintf(stream, " </d>\n");
    fprintf(stream, "</sunSpecData>\n");

    return rc;
}



int suns_model_xml_export_all(FILE *stream, char *type,
                              list_t *list, list_t *define_list)
{
    list_node_t *c;

    const char root[] = "sunSpecModels";

    fprintf(stream, "<%s v=\"1\">\n", root);
    list_for_each(list, c)
        suns_model_xml_fprintf(stream, c->data);
    fprintf(stream, "</%s>\n", root);

    return 0;
}


void suns_model_xml_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c;

    if (! model->did_list) {
        error("no did list for provided model %s", model->name);
        return;
    }

    list_for_each(model->did_list, c) {

        suns_model_did_t *did = c->data;
        fprintf(stream, "  <model id=\"%d\" len=\"%d\">\n",
                did->did, model->len);
        
        list_node_t *d;
        list_for_each(model->dp_blocks, d) {
            suns_model_xml_dp_block_fprintf(stream, d->data, did->did);
        }

        list_for_each(model->defines, d) {
            suns_model_xml_define_block_fprintf(stream, d->data);
        }

        fprintf(stream, "  </model>\n");

        /* strings */
        suns_model_xml_strings(stream, did, model->dp_blocks);
    }

}


void suns_model_xml_strings(FILE *stream,
                            suns_model_did_t *did,
                            list_t *dp_block_list)
{
    list_node_t *c;

    fprintf(stream, "  <strings id=\"%d\" locale=\"en\">\n", did->did);
    list_for_each(dp_block_list, c) {
        suns_dp_block_t *dp_block = c->data;
        list_node_t *d;
        list_for_each(dp_block->dp_list, d) {
            suns_dp_t *dp = d->data;
            char *label = suns_find_attribute(dp, "label");
            if (label) {
                fprintf(stream, "    <point name=\"%s\">\n", dp->name);
                fprintf(stream, "      <label>%s</label>\n", label);
                fprintf(stream, "      <description></description>\n");    
                fprintf(stream, "      <notes></notes>\n");
                fprintf(stream, "    </point>\n");
            }
        }
    }
    fprintf(stream, "  </strings>\n");    
}

void suns_model_xml_dp_block_fprintf(FILE *stream,
                                     suns_dp_block_t *dp_block,
                                     int did)
{
    list_node_t *c;

    fprintf(stream, "    <block");
    if (dp_block->repeating)
        fprintf(stream, " type=\"repeating\"");    
    fprintf(stream, " len=\"%d\"", dp_block->len);
    fprintf(stream, ">\n");
    list_for_each(dp_block->dp_list, c) {
        suns_model_xml_dp_fprintf(stream, c->data, did);
    }
    fprintf(stream, "    </block>\n");
}


void suns_model_xml_define_block_fprintf(FILE *stream,
                                         suns_define_block_t *block)
{
    list_node_t *c;

    fprintf(stream, "    <define name=\"%s\">\n", block->name);
    list_for_each(block->list, c) {
        suns_model_xml_define_fprintf(stream, c->data);
    }
    fprintf(stream, "    </define>\n");

}

void suns_model_xml_define_fprintf(FILE *stream,
                                   suns_define_t *define)
{
    fprintf(stream, "      <symbol name=\"%s\" value=\"%d\" />\n",
            define->name, define->value);
}

void suns_model_xml_dp_fprintf(FILE *stream,
                               suns_dp_t *dp,
                               int did)
{
    fprintf(stream, "      <point id=\"%s\" offset=\"%d\" "
            "type=\"%s\"",
            dp->name,
            dp->offset,
            suns_type_string(dp->type_pair->type));
    
    if (dp->type_pair->type == SUNS_STRING)
        fprintf(stream, " len=\"%d\"", dp->type_pair->len / 2);
    
    /* the name string is overloaded-
       it can be a scale factor reference, or a reference to
       the definitions for an enum or bitfield
       in the xml we are more explicit */
    if (dp->type_pair->name) {
        if ((dp->type_pair->type == SUNS_ENUM16) ||
            (dp->type_pair->type == SUNS_ENUM32) ||
            (dp->type_pair->type == SUNS_BITFIELD16) ||
            (dp->type_pair->type == SUNS_BITFIELD32)) {
            fprintf(stream, " define=\"%s\"", dp->type_pair->name);
        }
    }
    
    if (dp->type_pair->sf != 0) {
        fprintf(stream, " sf=\"%d\"", dp->type_pair->sf);
    } else if (dp->type_pair->name != NULL) {
        fprintf(stream, " sf=\"%s\"", dp->type_pair->name);
    }

    /* output additional point attributes by searching the attribute list */
    if (dp->attributes) {
        list_node_t *c;
        list_for_each(dp->attributes, c) {
            suns_attribute_t *a = c->data;

            /* units */
            if (strcmp(a->name, "u") == 0) {
                fprintf(stream, " units=\"%s\"", a->value);
            }

            /* req */
            if (strcmp(a->name, "req") == 0) {
                /* is it a nested attribute? */
                if (a->list) {
                    char buf[BUFFER_SIZE];
                    list_node_t *d;
                    snprintf(buf, BUFFER_SIZE, "%d", did);
                    list_for_each(a->list, d) {
                        suns_attribute_t *a2 = d->data;
                        if (strcmp(buf, a2->name) == 0) { 
                            fprintf(stream, " mandatory=\"true\"");
                       }
                    }
                } else {
                    fprintf(stream, " mandatory=\"true\"");
                }
            }
        }
    }

    fprintf(stream, " />\n");
}


/* convert the provided integer and exponent into
   normalized scientific notation.

   the point of this function is to preserve base 10 numbers, rather than
   convert them to base 2 floating points and back.

   it looks like i could use gcc's _Decimal32 type to accomplish this, but
   i'm going to do it by hand to maintain portability and control.

   returns number of chars writte on success, -1 on failure (meaning
   the buffer was too small); not the usual snprintf() behavior.
*/
int suns_snprintf_int_sf_e(char *buf,
                           size_t len,
                           int64_t x,
                           int e)
{
    int numlen;
    char tmpbuf[BUFFER_SIZE];

    numlen = snprintf(tmpbuf, BUFFER_SIZE - 1, "%lld", (int64_t) x);
    if (numlen > BUFFER_SIZE - 1) {
        /* tmpbuf is too small */
        debug("tmpbuf[BUFFER_SIZE] is too small to hold x = %lld", x);
        return -1;
    }

    return _suns_snprintf_int_sf_e(buf, len, tmpbuf, e);
}

int suns_snprintf_uint_sf_e(char *buf,
                            size_t len,
                            uint64_t x,
                            int e)
{
    int numlen;
    char tmpbuf[BUFFER_SIZE];
    
    numlen = snprintf(tmpbuf, BUFFER_SIZE - 1, "%llu", (uint64_t) x);
    if (numlen > BUFFER_SIZE - 1) {
        /* tmpbuf is too small */
        debug("tmpbuf[BUFFER_SIZE] is too small to hold x = %llu", x);
        return -1;
    }

    return _suns_snprintf_int_sf_e(buf, len, tmpbuf, e);
}

/* inner function used after the type-specific conversion of the
   base number to a string */
int _suns_snprintf_int_sf_e(char *buf,
                            size_t len,
                            char *base,
                            int e)
{
    int buf_char = 0;
    int numlen = strlen(base);
    int i;

    if (numlen + 2 > len) {
        debug("len = %d is too small for the base integer", len);
        return -1;
    }

    /* normalize e */
    e += numlen - 1;

    /* copy first digit and decimal point */
    buf[buf_char++] = base[0];
    buf[buf_char++] = '.';

    /* start on the 2nd digit and copy the rest */
    for (i = 1; i < numlen; i++)
        buf[buf_char++] = base[i];

    /* now add exponent if needed */
    if (e != 0) {
        buf[buf_char++] = 'e';
       
        buf_char += snprintf(buf + numlen + 2, len - numlen - 3, "%d", e);
        if (buf_char += snprintf(buf + numlen + 2, len - numlen - 3, "%d", e)
            > len - numlen - 3) {
            debug("len %d is to small", len);
            return -1;
        }
    }

    return buf_char;
}



/* outputs a sunspec value with its scale factor applied, with
   no loss of precision due to conversion to base 2 floating point

   this uses long int as its argument so it works for all 32 bit
   datatype (signed and unsigned).  it will not work for unsigned 64 bit
   integers, so this will have to be revisited at some point.
 */
int suns_snprintf_int_sf(char *buf,
                         size_t len,
                         int64_t x,
                         int e,
                         int maxdigits)
{
    int numlen;
    char tmpbuf[BUFFER_SIZE];

    numlen = snprintf(tmpbuf, BUFFER_SIZE, "%lld", x);
    if (numlen > BUFFER_SIZE) {
        /* tmpbuf is too small */
        debug("tmpbuf[BUFFER_SIZE] is too small to hold x = %lld", x);
        return -1;
    }

    return _suns_snprintf_int_sf(buf, len, tmpbuf, e, maxdigits);
}

int suns_snprintf_uint_sf(char *buf,
                          size_t len,
                          uint64_t x,
                          int e,
                          int maxdigits)
{
    int numlen;
    char tmpbuf[BUFFER_SIZE];

    numlen = snprintf(tmpbuf, BUFFER_SIZE, "%llu", x);
    if (numlen > BUFFER_SIZE) {
        /* tmpbuf is too small */
        debug("tmpbuf[BUFFER_SIZE] is too small to hold x = %lld", x);
        return -1;
    }

    return _suns_snprintf_int_sf(buf, len, tmpbuf, e, maxdigits);
}

int _suns_snprintf_int_sf(char *buf,
                          size_t len,
                          char *base,
                          int e,
                          int maxdigits)
{
    int numlen = strlen(base);
    int i;
    int buf_char = 0;

    if (numlen > len) {
        debug("len = %d < required minimum length for number = %d",
              len, numlen);
        return -1;
    }

    /* short circuit if e = 0 */
    if (e == 0) {
        strncpy(buf, base, len);
        return 0;
    }

    /* if the resulting number of digits is greater than maxdigits use
       scientific notation */
    if ((numlen > maxdigits) ||
        (numlen + abs(e) > maxdigits + 2)) {
        return _suns_snprintf_int_sf_e(buf, len, base, e);
    }

    if (e < 0) {
        /* negative e means a fractional value */
        int base_char = 0;

        /* this section writes the digits to the left of the decimal point */
        if (e + numlen > 0) {
            /* there are digits to the left of the decimal point */
            for (i = 0; i < e + numlen; i++) {
                buf[buf_char++] = base[base_char++];
            }
        } else {
            /* all the digits are to the right of the decimal point */
            buf[buf_char++] = '0';
        }
        
        buf[buf_char++] = '.';

        /* are there are zeros to the left of the digits? */
        for (i = 0; i > e + numlen; i--) {
            buf[buf_char++] = '0';
        }
        
        /* now append the actual digits */
        while (base_char < numlen) {
            buf[buf_char++] = base[base_char++];
        }

    } else {
        /* e must be positive (0 is taken care of above) */
        for (i = 0; i < numlen; i++) {
            buf[buf_char++] = base[i];
        }
        for (i = 0; i < e; i++) {
            buf[buf_char++] = '0';
        }
    }
    buf[buf_char] = '\0';  /* tie off the string */

    return buf_char;
}


