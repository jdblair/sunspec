/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output.c
 * $Id: $
 *
 * output functions for the various output formats
 *
 * Copyright (c) 2011, John D. Blair <jdb@moship.net>
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


#include "trx/macros.h"
#include "trx/debug.h"
#include "suns_model.h"
#include "suns_output.h"

/* list of model output formats */
static suns_model_export_format_t suns_model_export_formats[] = {
    { "slang", suns_model_fprintf },
    { "sql",   suns_model_sql_fprintf },
    { "csv",   suns_model_csv_fprintf },
    { NULL, NULL }
};

/* list of dataset output functions */
static suns_dataset_output_format_t suns_dataset_output_formats[] = {
    { "text", suns_dataset_text_fprintf },
    { "sql",  suns_dataset_sql_fprintf },
    { "csv",  suns_dataset_csv_fprintf },
    { NULL, NULL }
};


/* output a model in the given type */
int suns_model_export(char *type, suns_model_t *model, FILE *stream)
{
    assert(type);
    assert(model);
    assert(stream);
    
    int i;
    suns_model_export_format_t *export = NULL;

    for (i = 0; suns_model_export_formats[i].name != NULL; i++) {
	debug("i = %d", i);
	if (strcmp(suns_model_export_formats[i].name, type) == 0) {
	    export = &(suns_model_export_formats[i]);
	    break;
	}
    }

    if (export == NULL) {
	error("export format %s is not defined", type);
	return -1;
    }

    export->fprintf(stream, model);

    return 0;
}

/* output all the models in a list_t in the given type */
int suns_model_export_all(char *type, list_t *model_list, FILE *stream)
{
    list_node_t *c;

    list_for_each(model_list, c) {
	/* bail out on first failure since this is probably
	   caused by an invalid export format string */
	debug("c = %p", c);
	if (suns_model_export(type, c->data, stream) < 0) {
	    return -1;
	}
    }

    return 0;
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

    output->fprintf(stream, data);

    return 0;
}




void suns_dp_fprint(FILE *stream, suns_dp_t *dp)
{
    fprintf(stream, "    %-32s { ", dp->name);
    if (dp->offset > 0)
	fprintf(stream, "%3d ", dp->offset);
    suns_type_pair_fprint(stream, dp->type_pair);
    fprintf(stream, " }\n");
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
	len += snprintf(str + len, size - len, ".%d", tp->sub.len);
    else if (tp->sub.name)
	len += snprintf(str + len, size - len, ".%s", tp->sub.name);

    return len;
}

void suns_type_pair_fprint(FILE *stream, suns_type_pair_t *tp)
{
    char buf[BUFFER_SIZE];
    
    suns_snprintf_type_pair(buf, BUFFER_SIZE, tp);
    fprintf(stream, "%s", buf);
}
    



/**********************************************************************
 *
 * slang model notation format
 *
 **********************************************************************/

void suns_model_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c;

    fprintf(stream, "model suns {\n");
    /* fprintf(stream, "  name \"%s\"\n", model->name);
       fprintf(stream, "  did %d\n", model->did); */
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
    fprintf(stream, "  }\n");
}


void suns_define_fprint(FILE *stream, suns_define_t *define)
{
    fprintf(stream, "    %-32s { %2d \"%s\" }\n",
	   define->name, define->value, define->string);
}



/**********************************************************************
 *
 * text format
 *
 **********************************************************************/

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
    char value_buf[BUFFER_SIZE];
    char tp_buf[BUFFER_SIZE];

    fprintf(stream, "datablock: %s(%d)\n",
            data->did->name,
            data->did->did);
    
    list_for_each(data->values, c) {
        suns_value_t *v = c->data;
        suns_snprintf_type_pair(tp_buf, BUFFER_SIZE, &(v->tp));
        suns_snprintf_value(value_buf, BUFFER_SIZE, v);
        fprintf(stream, "  %-30s %s\n",  v->name, value_buf);
    }

    fprintf(stream, "\n");

    return 0;
}
    

/**********************************************************************
 *
 * sql format
 *
 **********************************************************************/


void suns_model_sql_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c, *d;

    fprintf(stream, "CREATE TABLE IF NOT EXISTS %s\n", model->name);
    fprintf(stream, "( unixtime,timestamp,addr");
    
    list_for_each(model->dp_blocks, d) {
	suns_dp_block_t *dp_block = d->data;
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
        suns_snprintf_value(buf, BUFFER_SIZE, v);
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

void suns_model_csv_fprintf(FILE *stream, suns_model_t *model)
{
    list_node_t *c, *d;

    fprintf(stream, "notation,block_offset,size,word_offset,byte_offset,rw,name,type,units,contents,description\n");
    list_for_each(model->dp_blocks, d) {
	suns_dp_block_t *dp_block = d->data;
	list_for_each(dp_block->dp_list, c) {
	    suns_dp_t *dp = c->data;
	    int size = suns_type_pair_size(dp->type_pair);
	    fprintf(stream, "%d,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s\n",
		    /* notation */       dp->offset + 40000,
		    /* block_offset */   dp->offset,
		    /* size */           size,
		    /* word_offset */    size - 1,
		    /* byte_offset */    (size - 1) * 2,
		    /* rw */             "",
		    /* name */           dp->name,
		    /* type */           suns_type_string(dp->type_pair->type),
		    /* units */          "",
		    /* contents */       "",
		    /* description */    "");
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
    fprintf(stream, "%d,'%s',%d", 0, "NULL", 0);
    list_for_each(data->values, c) {
	suns_value_t *v = c->data;
	char buf[BUFFER_SIZE];
	/* FIXME: should store NULL instead of "not implemented" */
	suns_snprintf_value(buf, BUFFER_SIZE, v);
	/* FIXME: should store numeric values, not strings */
	fprintf(stream, ",\"%s\"", buf);
    }
    fprintf(stream, "\n\n");

    return 0;
}
 
