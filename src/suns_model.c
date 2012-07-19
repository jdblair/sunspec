/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_model.c
 *
 * Functions for constructing and interacting with sunspec models.
 * This includes all the data structures used for every component of the
 * abstract internal representation.
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

#define _BSD_SOURCE  /* for big/little endian macros */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <endian.h>
#include <time.h>

#include "suns_model.h"
#include "suns_parser.h"
#include "trx/debug.h"
#include "trx/macros.h"
#include "trx/string.h"


suns_model_t *suns_model_new(void)
{
    suns_model_t *m = malloc(sizeof(suns_model_t));
    if (m == NULL) {
        debug("malloc() failed");
        return NULL;
    }
    
    memset(m, 0, sizeof(suns_model_t));

    m->defines = list_new();
    m->did_list = list_new();
    m->dp_blocks = list_new();

    return m;
}


suns_model_did_t * suns_model_did_new(uint16_t id)
{
    suns_model_did_t *did = malloc(sizeof(suns_model_did_t));
    if (did == NULL) {
        error("memory error: can't malloc(sizeof(suns_model_did_t))");
        return NULL;
    }

    memset(did, 0, sizeof(suns_model_did_t));    
    did->did = id;
    
    return did;
}


suns_dp_block_t *suns_dp_block_new(void)
{
    suns_dp_block_t *dp_block = malloc(sizeof(suns_dp_block_t));
    if (dp_block == NULL) {
        error("memory error: can't malloc(sizeof(suns_dp_block_t))");
        return NULL;
    }

    memset(dp_block, 0, sizeof(suns_dp_block_t));

    return dp_block;
}


suns_dp_t *suns_dp_new(void)
{
    suns_dp_t *dp = malloc(sizeof(suns_dp_t));
    if (dp == NULL) {
        error("memory error: can't malloc(sizeof(suns_dp_t))");
        return NULL;
    }
    memset(dp, 0, sizeof(suns_dp_t));
    dp->offset = -1;  /* 0 is a valid offset */
    
    return dp;
}


char * suns_type_string(suns_type_t type)
{
    /* these must be in the same order as the enum definition
       in suns_model.h!! */
    char *type_table[] = {
        "null",
        "int16",
        "uint16",
        "acc16",
        "int32",
        "uint32",
        "float32",
        "acc32",
        "int64",
        "uint64",
        "float64",
        "acc64",
        "enum16",
        "enum32",
        "bitfield16",
        "bitfield32",
        "sunssf",
        "string",
        "pad",
        "ipaddr",
        "ipv6addr",
        "undef",
        NULL
    };

    if ((type > SUNS_UNDEF) ||
        (type < SUNS_NULL)) {
        return NULL;
    }

    return type_table[type];
}


char * suns_value_meta_string(suns_value_meta_t meta)
{
    char *meta_table[] = {
        "null",
        "ok",
        "not implemented",
        "error",
        "undef",
        NULL,
    };

    if ((meta > SUNS_VALUE_UNDEF) ||
        (meta < SUNS_VALUE_NULL)) {
        return NULL;
    }

    return meta_table[meta];
}


suns_type_t suns_type_from_name(char *name)
{
    int i;

    typedef struct suns_type_name_map {
        char *name;
        suns_type_t type;
    } suns_type_name_map_t;

    suns_type_name_map_t name_map[] = {
        { "null",       SUNS_NULL },
        { "int16",      SUNS_INT16 },
        { "uint16",     SUNS_UINT16 },
        { "acc16",      SUNS_ACC16 },
        { "int32",      SUNS_INT32 },
        { "uint32",     SUNS_UINT32 },
        { "float32",    SUNS_FLOAT32 },
        { "acc32",      SUNS_ACC32 },
        { "int64",      SUNS_INT64 },
        { "uint64",     SUNS_UINT64 },
        { "float64",    SUNS_FLOAT64 },
        { "acc64",      SUNS_ACC64 },
        { "enum16",     SUNS_ENUM16 },
        { "enum32",     SUNS_ENUM32 },
        { "bitfield16", SUNS_BITFIELD16 },
        { "bitfield32", SUNS_BITFIELD32 },
        { "sunssf",     SUNS_SF },
        { "string",     SUNS_STRING },
        { "pad",        SUNS_PAD },
        { "ipaddr",     SUNS_IPV4 },
        { "ipv6addr",   SUNS_IPV6 },
        { "undef",      SUNS_UNDEF },
        { NULL,         -1 },
    };

    for (i = 0; name_map[i].name != NULL; i++) {
        if (strcmp(name_map[i].name, name) == 0) {
            return name_map[i].type;
        }
    }

    /* if we're here the type was not found */
    return SUNS_UNDEF;
}


int suns_value_name_with_index(suns_value_t *v, char *buf, size_t len)
{
    /* index only makes sense for repeating blocks */
    if (v->repeating)
        return snprintf(buf, len, "%s,%02d", v->name, v->index);

    return snprintf(buf, len, "%s", v->name);
}


suns_type_pair_t *suns_type_pair_new(void)
{
    suns_type_pair_t *type_pair;

    type_pair = malloc(sizeof(suns_type_pair_t));

    if (type_pair == NULL)  /* just in case */
        return NULL;

    memset(type_pair, 0, sizeof(suns_type_pair_t));

    return type_pair;
}


suns_attribute_t *suns_attribute_new(void)
{
    suns_attribute_t *a = malloc(sizeof(suns_attribute_t));
    memset(a, 0, sizeof(suns_attribute_t));
    return a;
}


void suns_attribute_list_free(list_t *l)
{
    list_free(l, suns_attribute_free);
}


void suns_attribute_free(void *p)
{
    suns_attribute_t *a = p;
    if (a->list)
        suns_attribute_list_free(a->list);

    free(a);
}

void suns_model_free(suns_model_t *model)
{
    list_free(model->dp_blocks, (list_free_data_f) suns_model_dp_block_free);
    free(model);
}


void suns_model_dp_block_free(suns_dp_block_t *dp_block)
{
    list_free(dp_block->dp_list, free);
    free(dp_block);
}


suns_data_t *suns_data_new(void)
{
    suns_data_t *block = malloc(sizeof(suns_data_t));
    if (block == NULL) {
        error("malloc() returned NULL");
        return NULL;
    }

    memset(block, 0, sizeof(suns_data_t));

    block->data = buffer_new(BUFFER_SIZE);
    
    return block;
}


void suns_data_free(suns_data_t *block)
{
    if (block->data)
        buffer_free(block->data);

    free(block);
}


suns_data_block_t * suns_data_block_new(void)
{
    suns_data_block_t *new;

    new = malloc(sizeof(suns_data_block_t));
    if (new == NULL)
        return NULL;

    memset(new, 0, sizeof(suns_data_block_t));

    return new;
}


void suns_data_block_free(suns_data_block_t *block)
{
    buffer_free(block->data);
    free(block);
}


suns_define_block_t * suns_define_block_new(void)
{
    suns_define_block_t *new;

    new = malloc(sizeof(suns_define_block_t));
    if (new == NULL)
        return NULL;

    memset(new, 0, sizeof(suns_define_block_t));

    new->list = NULL;

    return new;
}


void suns_define_block_free(suns_define_block_t *block)
{
    list_free(block->list, free);
    free(block);
}

    

/* return the size in bytes of a specified suns_type_t */
int suns_type_size(suns_type_t type)
{
    /* must be in the same order as the enum in suns_model.h!! */
    static int sizes[] = {
        0, /* SUNS_NULL */
        2, /* SUNS_INT16 */
        2, /* SUNS_UINT16 */
        2, /* SUNS_ACC16 */
        4, /* SUNS_INT32 */
        4, /* SUNS_UINT32 */
        4, /* SUNS_FLOAT32 */
        4, /* SUNS_ACC32 */
        8, /* SUNS_INT64 */
        8, /* SUNS_UINT64 */
        8, /* SUNS_FLOAT64 */
        8, /* SUNS_ACC64 */
        2, /* SUNS_ENUM16 */
        4, /* SUNS_ENUM32 */
        2, /* SUNS_BITFIELD16 */
        4, /* SUNS_BITFIELD32 */
        2, /* SUNS_SF */
        0, /* SUNS_STRING */
        2, /* SUNS_PAD */
        4, /* SUNS_IPV4 */
        8, /* SUNS_IPV6 */
        0, /* SUNS_UNDEF */
    };

    if ((type > SUNS_UNDEF) ||
        (type < SUNS_NULL)) {
        debug("invalid type %d passed to suns_type_size()", type);
        return -1;
    }

    return sizes[type];
}

/* like suns_type_size(), but acts on a suns_type_pair_t *
   this means can return the length of strings */
int suns_type_pair_size(suns_type_pair_t *tp)
{
    if (tp->type == SUNS_STRING) {
        return tp->len;
    }

    return suns_type_size(tp->type);
}


/* now to write functions which convert between suns_value_t and
   binary buffers */

int suns_value_to_buf(suns_value_t *v, unsigned char *buf, size_t len)
{
    switch (v->tp.type) {

        /* 16 bit datatypes */
    case SUNS_INT16:
    case SUNS_UINT16:
    case SUNS_ACC16:
    case SUNS_ENUM16:
    case SUNS_BITFIELD16:
    case SUNS_SF:
        if (len < 2) {
            debug("not enough space for 16 bit conversion "
                  "(type = %s,  len = %d)", suns_type_string(v->tp.type), len);
        }
        /* we can safely treat all these values as uint for this purpose */
        *((uint16_t *) buf) = htobe16(v->value.u16);
        break;
		/* 32 bit datatypes */
    case SUNS_INT32:
    case SUNS_UINT32:
    case SUNS_FLOAT32:
    case SUNS_ACC32:
    case SUNS_ENUM32:
    case SUNS_BITFIELD32:
    case SUNS_IPV4:
        if (len < 4) {
            debug("not enough space for 32 bit conversion "
                  "(type = %s,  len = %d)", suns_type_string(v->tp.type), len);
        }
        /* we can safely treat all these values as uint for this purpose */
        *((uint32_t *) buf) = htobe32(v->value.u32);
        break;
    case SUNS_INT64:
    case SUNS_UINT64:
    case SUNS_FLOAT64:
    case SUNS_ACC64:
        if (len < 4) {
            debug("not enough space for 64 bit conversion "
                  "(type = %s,  len = %d)", suns_type_string(v->tp.type), len);
        }
        /* we can safely treat all these values as uint for this purpose */
        *((uint64_t *) buf) = htobe64(v->value.u64);
        break;

        /* 128 bit datatypes */
    case SUNS_IPV6:
        /* FIXME */
        debug("ipv6 support not finished");
        break;

        /* strings */
    case SUNS_STRING:
        if (len < v->tp.len) {
            debug("not enough space for string(%d) (type = %s,  len = %d)",
                  v->tp.len, suns_type_string(v->tp.type), len);
        }
        strncpy((char *) buf, v->value.s, v->tp.len);  /* don't allow string to
                                                              overrun its defined size */
        /* buf[v->tp.len - 1] = '\0';  */          /* make sure the string is
                                                          always NULL terminated */
        break;

    case SUNS_PAD:
        /* pad registers are always 0xFFFF */
        *((uint16_t *) buf) = 0xFFFF;
        break;

    default:
        /* this means we hit an unsupported datatype or SUNS_UNDEF */
        debug("unsupported datatype %s", suns_type_string(v->tp.type));
        return -2;
    }

    return 0;
}


/* writes to a buffer_t, not an ordinary binary buffer
   returns the number of bytes added to the buffer */
int suns_value_to_buffer(buffer_t *buf, suns_value_t *v)
{
    int rc;
    int size;

    if (v->tp.type == SUNS_STRING) {
        size = v->tp.len;
    } else {
        size = suns_type_size(v->tp.type);
        if (size == 0) {
            debug("unsupported type %s(%d)",
                  suns_type_string(v->tp.type), v->tp.type);
        }
    }

    /* enough space? */
    if (buffer_space(buf) < size) {
        /* should we resize instead of just bailing out? */
        return 0;
    }

    /* write the data to the buffer */
    rc =  suns_value_to_buf(v, (unsigned char *) buf->in,
                            buffer_space(buf));

    /* debug("buf->in = %p, size = %d", buf->in, size); */
    buf->in += size;

    return rc;
}


/* sets v->meta to appropriately to SUNS_VALUE_OK or SUNS_VALUE_NOT_IMPLEMENTED
   also returns the meta value.
   this logic is used in at least two places so I'm breaking it out here */
suns_value_meta_t suns_check_not_implemented(suns_type_pair_t *tp,
                                             suns_value_t *v)
{
    if ((tp->type == SUNS_INT16   && v->value.i16 == (int16_t)  0x8000)     ||
        (tp->type == SUNS_UINT16  && v->value.u16 == (uint16_t) 0xFFFF)     ||
        (tp->type == SUNS_ENUM16  && v->value.u16 == (uint16_t) 0xFFFF)     ||
        (tp->type == SUNS_SF      && v->value.i16 == (int16_t)  0x8000)     ||
        (tp->type == SUNS_BITFIELD16 && v->value.u16 == (uint16_t) 0xFFFF)  ||
        (tp->type == SUNS_INT32   && v->value.i32 == (int32_t)  0x80000000) ||
        (tp->type == SUNS_UINT32  && v->value.i32 == (uint32_t) 0xFFFFFFFF) ||
        (tp->type == SUNS_ENUM32  && v->value.i32 == (uint32_t) 0xFFFFFFFF) ||
        (tp->type == SUNS_BITFIELD32 && v->value.u32 == (uint32_t) 0xFFFFFFFF) ||
        (tp->type == SUNS_INT64   && v->value.i64 == (int64_t) 0x8000000000000000) ||
        (tp->type == SUNS_UINT64  && v->value.u64 == (uint64_t) 0xFFFFFFFFFFFFFFFF) ||
        (tp->type == SUNS_FLOAT32 && isnan(v->value.f32)) ||
        (tp->type == SUNS_FLOAT64 && isnan(v->value.f64)) ||
        (tp->type == SUNS_IPV4  && v->value.u32 == 0)
        ) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        /* mark the value as valid */
        v->meta = SUNS_VALUE_OK;
    }

    return v->meta;
}


int suns_buf_to_value(unsigned char *buf,
                      suns_type_pair_t *tp,
                      suns_value_t *v)
{
    switch (tp->type) {
	
        /* 16 bit datatypes */
    case SUNS_INT16:
    case SUNS_UINT16:
    case SUNS_ACC16:
    case SUNS_ENUM16:
    case SUNS_BITFIELD16:
    case SUNS_SF:
        v->value.u16 = be16toh(*((uint16_t *)buf));
        break;
	
        /* 32 bit datatypes */
    case SUNS_INT32:
    case SUNS_UINT32:
    case SUNS_FLOAT32:
    case SUNS_ACC32:
    case SUNS_ENUM32:
    case SUNS_BITFIELD32:
    case SUNS_IPV4:
        v->value.u32 = be32toh(*((uint32_t *)buf));
        break;
	
        /* 64 bit datatypes */
    case SUNS_INT64:
    case SUNS_UINT64:
    case SUNS_FLOAT64:
    case SUNS_ACC64:
        v->value.u64 = be64toh(*((uint64_t *)buf));
        break;

        /* 128 bit datatypes */
    case SUNS_IPV6:
        /* FIXME */
        debug("ipv6 support not finished");
        break;

        /* strings */
    case SUNS_STRING:
        /* same as malloc() if tp->s == NULL */
        v->value.s = realloc(v->value.s, tp->len + 1);
        if (v->value.s == NULL) {
            /* uh oh */
            debug("malloc() returned NULL!");
            error("memory allocation failure");
            return -1;
        }

        /* don't assume the string will be NULL terminated */
        v->value.s[tp->len] = '\0';

        /* copy out of the buffer */
        strncpy(v->value.s, (char *) buf, tp->len);

        break;

    case SUNS_PAD:
        /* skip pad registers */
        debug("skipping pad register");
        break;
    
    default:
        /* this means we hit an unsupported datatype or SUNS_UNDEF */
        debug("unsupported datatype %s(%d)",
              suns_type_string(tp->type), tp->type);
        return -2;
    }    

    /* this sets v->meta appropriately */
    suns_check_not_implemented(tp, v);

    /* set the value to the specified type */
    v->tp = *tp;

    /* stash the "raw" binary value */
    v->raw_len = min(16, suns_type_size(tp->type));
    memcpy(v->raw, buf, v->raw_len);
    
    return 0;
}


/* parse a string into a value based on the type information in *tp */
int suns_string_to_value(const char *string,
                         suns_value_t *v,
                         suns_type_pair_t *tp)
{
    /* We could just use a large enough numeric size to store all numbers
       rather than splitting them up into 16 bit and 32 bit numbers.
       If this was written from scratch this would make the most sense.
       However, since we already have the machinery to do it, let's use
       the precise built-in binary types.  */

    /* set the value to the specified type */
    v->tp = *tp;
    
    /* special case floating point values */
    if (tp->type == SUNS_FLOAT32) {
        if (sscanf(string, "%f", &(v->value.f32)) != 1) {
            debug("can't parse '%s' to SUNS_FLOAT32", string);
            return -1;
        }
        if (isnan(v->value.f32))
            v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else if (tp->type == SUNS_FLOAT64) {
        if (sscanf(string, "%lf", &(v->value.f64)) != 1) {
            debug("can't parse '%s' to SUNS_FLOAT32", string);
            return -1;
        }
        if (isnan(v->value.f64))
            v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        char base[BUFFER_SIZE];
        int exp;
        unsigned int octet[4];  /* used in SUNS_IPV4 */

        if (tp->type != SUNS_STRING &&
            string_decompose_decimal(string, base, BUFFER_SIZE, &exp) < 0)
            return -1;

        switch (tp->type) {
        
            /* 16 bit signed integers */
        case SUNS_INT16:
        case SUNS_SF:
            if (sscanf(string, "%hd", &(v->value.i16)) < 0) {
                debug("can't parse '%s' to int16", string);
                return -1;
            }
            break;
            
            /* 16 bit unsigned integers */
        case SUNS_UINT16:
        case SUNS_ACC16:
        case SUNS_ENUM16:
        case SUNS_BITFIELD16:
            if (sscanf(string, "%hu", &(v->value.u16)) < 0) {
                debug("can't parse '%s' to uint16", string);
                return -1;
            }
            break;
            
            /* 32 bit signed integers */
        case SUNS_INT32:
            if (sscanf(string, "%d", &(v->value.i32)) < 0) {
                debug("can't parse '%s' to int32", string);
                return -1;
            }
            break;

            /* 64 bit unsigned integers */
        case SUNS_UINT64:
            if (sscanf(string, "%lu",
                       (long unsigned int *) &(v->value.u64)) < 0) {
                debug("can't parse '%s' to uint64", string);
                return -1;
            }
            break;

            /* 64 bit signed integers */
        case SUNS_INT64:
            if (sscanf(string, "%ld", (long int *) &(v->value.i64)) < 0) {
                debug("can't parse '%s' to int64", string);
                return -1;
            }
            break;
                
            /* 32 bit unsigned integers */
        case SUNS_UINT32:
        case SUNS_ACC32:
        case SUNS_ENUM32:
        case SUNS_BITFIELD32:
            if (sscanf(string, "%u", &(v->value.u32)) < 0) {
                debug("can't parse '%s' to uint32", string);
                return -1;
            }
            break;

            /* strings */
        case SUNS_STRING:
            /* same as malloc() if tp->s == NULL */
            v->value.s = realloc(v->value.s, tp->len + 1);
            if (v->value.s == NULL) {
                /* uh oh */
                debug("malloc() returned NULL!");
                return -3;
            }

            /* don't assume the string will be NULL terminated */
            v->value.s[tp->len] = '\0';
            
            /* copy out of the buffer */
            strncpy(v->value.s, string, tp->len);
            
            break;

        case SUNS_IPV4:
            if (sscanf(string, "%u.%u.%u.%u",
                       &(octet[0]), &(octet[1]), &(octet[2]), &(octet[3]))
                == 4) {
                v->value.u32 = ((octet[0] << 24) &
                                (octet[1] << 16) &
                                (octet[2] <<  8) &
                                (octet[3] <<  0));
            }
            break;

        case SUNS_IPV6:
            /* FIXME */
            debug("ipv6 support not finished");
            break;
            
        default:
            /* this means we hit an unsupported datatype or SUNS_UNDEF */
            debug("unsupported datatype %s(%d)",
                  suns_type_string(tp->type), tp->type);
            return -2;
        }
        
        /* this sets v->meta appropriately */
        suns_check_not_implemented(tp, v);
        
        /* store the exponent in the tp */
        v->tp.sf = exp;
        
    }

    /* this sets v->meta appropriately */
    suns_check_not_implemented(tp, v);

    /* set the value to the specified type */
    v->tp = *tp;

    /* stash the "raw" binary value */
    return 0;
}


suns_value_t *suns_value_new(void)
{
    suns_value_t *v = malloc(sizeof(suns_value_t));

    if (v == NULL) {
        /* uh oh */
        debug("malloc() returned NULL!");
        error("memory allocation failure");
        return NULL;
    }

    suns_value_init(v);  /* zeros out memory for us */

    return v;
}


void suns_value_free(suns_value_t *v)
{
    assert(v);

    if (((suns_value_t *)v)->tp.type == SUNS_STRING) {
        if (((suns_value_t *)v)->value.s) {
            free(((suns_value_t *)v)->value.s);
            ((suns_value_t *)v)->value.s = NULL;
        }
    }

    /* if v->name_with_index is not a pointer to v->name
       it contains a chunk of memory containing the
       composite name_with_index */
    if (v->name_with_index != v->name)
        free(v->name_with_index);
    
    free(v);
}


/**
 * special accessor to set the index value
 * this has the side-effect of keeping v->name_with_index up-to-date
 */
int suns_value_set_index(suns_value_t *v, int index)
{
    v->index = index;

    if (v->name)
        return suns_value_set_name(v, v->name);

    /* if we're here the v->name element hasn't been set
       so therey's no name to update */
    return 0;
}


/**
 * special accessor to set the value name
 * this is normally a pointer to a string that has been parsed out of
 * a model definition file.
 * it has the side-effect of keeping the the name_with_index up-to-date
 */
int suns_value_set_name(suns_value_t *v, char *name)
{
    /* remember this is called from suns_value_set_index() !! */

    v->name = name;

    if (v->repeating) {

        /* do we need some some space for name_with_index? */
        if ((v->name_with_index == NULL) ||
            (v->name_with_index == v->name)) {
            
            v->name_with_index = malloc(BUFFER_SIZE);
            if (v->name_with_index == NULL) {
                error("can't malloc() space for v->name_with_index");
                return -1;
            }
            memset(v->name_with_index, 0, BUFFER_SIZE);
        }
        
        return snprintf(v->name_with_index, BUFFER_SIZE, "%s,%02d",
                        v->name, v->index);    
    }

    /* nonrepeating block just points name_with_index at name */
    v->name_with_index = v->name;
    return 0;
}


/* returns 1 if the type is a numeric type */
int suns_type_is_numeric(suns_type_t t)
{
    /* note that a scale factor is not considered a numeric type! */
    return ((t == SUNS_INT16) ||
            (t == SUNS_UINT16) ||
            (t == SUNS_ACC16) ||
            (t == SUNS_INT32) ||
            (t == SUNS_FLOAT32) ||
            (t == SUNS_ACC32) ||
            (t == SUNS_INT64) ||
            (t == SUNS_UINT64) ||
            (t == SUNS_FLOAT64) ||
            (t == SUNS_ACC64));    
}


/* returns 1 if the value is a numeric type */
int suns_value_is_numeric(suns_value_t *v)
{
    return (suns_type_is_numeric(v->tp.type));
}


/* returns 1 if the value is an accumulator type */
int suns_value_is_acc(suns_value_t *v)
{
    return ((v->tp.type == SUNS_ACC16) ||
            (v->tp.type == SUNS_ACC32) ||
            (v->tp.type == SUNS_ACC64));
}


/* returns 1 if *v is an accumulator type and equal to zero */
int suns_value_acc_is_zero(suns_value_t *v)
{
    switch (v->tp.type) {
    case SUNS_ACC16:
        return (suns_value_get_acc16(v) == 0);

    case SUNS_ACC32:
        return (suns_value_get_acc32(v) == 0);

    case SUNS_ACC64:
        return (suns_value_get_acc64(v) == 0);
        
    default:
        /* fall through to below */
        break;

    }

    debug("*v is not an accumulator type");
    return 0;
}


/**
 * returns 1 if the type is an enum or bitfield, meaning
 * that it can have symbols defined for it
 */
int suns_type_is_symbolic(suns_type_t t)
{
    return ((t == SUNS_ENUM16) ||
            (t == SUNS_ENUM32) ||
            (t == SUNS_BITFIELD16) ||
            (t == SUNS_BITFIELD32));
}


suns_dataset_t *suns_dataset_new(void)
{
    suns_dataset_t *d;

    d = malloc(sizeof(suns_dataset_t));
    if (d == NULL) {
        debug("malloc() failed");
        return NULL;
    }

    memset(d, 0, sizeof(suns_dataset_t));
	
    d->values = list_new();
    if (d->values == NULL) {
        debug("list_new() failed");
        return NULL;
    }

    return d;
}


void suns_dataset_free(suns_dataset_t *d)
{
    assert(d);

    list_free(d->values, (list_free_data_f) suns_value_free);
    free(d);
}


suns_device_t *suns_device_new(void)
{
    suns_device_t *d = malloc(sizeof(suns_device_t));

    if (d == NULL) {
        debug("malloc() failed");
        return NULL;
    }
    memset(d, 0, sizeof(suns_device_t));
    d->datasets = list_new();

    /* initialize timestamp to now */
    d->unixtime = time(NULL);

    return d;
}

void suns_device_free(suns_device_t *d)
{
    assert(d);

    list_free(d->datasets, (list_free_data_f) suns_dataset_free);
    free(d);
}


int suns_device_add_dataset(suns_device_t *device, suns_dataset_t *data)
{
    int rc = 0;
    int i;
    suns_value_t *v;

    typedef struct dp_elmt_map {
        char *dp;
        char **ptr;
    } dp_elmt_map_t;

    dp_elmt_map_t elmt_map[] = {
        { "Mn", &(device->manufacturer) },
        { "Md", &(device->model) },
        { "SN", &(device->serial_number) },
        { NULL,             NULL  }
    };

    rc = list_node_add(device->datasets, list_node_new(data));
    if (rc < 0) {
        debug("list_node_add() failed");
        error("memory error: failed to add device to dataset");
        return rc;
    }

    debug("adding dataset for %d: %s", data->did->did, data->did->name);

    /* check if we are adding the common model dataset */
    /* if we are, then cache the pointers to the pieces we care about */
    if (data->did->did == 1) {
        device->common = data;
        
        for (i = 0; elmt_map[i].dp != NULL; i++) {
            v = suns_search_value_list(device->common->values, elmt_map[i].dp);
            if (v)
                *(elmt_map[i].ptr) = suns_value_get_string(v);
        }
    }

    /* search the dataset already to see if we've already added
       a dataset of the current did so we can set the index value
       correctly.  this is an n^2 search, but we should be ok since
       devices should have a relatively small number of models.
    */


    return rc;
}

/* "accessors" for suns_value_t
   
these are implemented as functions even though macros would be more
efficient to force the compiler to do type checking on the values
that are passed back and forth

other than assert() no checks are made on the actual stored type
*/

void suns_value_init(suns_value_t *v)
{
    assert(v != NULL);
    memset(v, 0, sizeof(suns_value_t));
}


void suns_value_set_null(suns_value_t *v)
{
    assert(v != NULL);
    memset(v, 0, sizeof(suns_value_t));
}

    

void suns_value_set_uint16(suns_value_t *v, uint16_t u16)
{
    assert(v != NULL);

    v->value.u16 = u16;
    v->tp.type = SUNS_UINT16;
    if (u16 == (uint16_t) 0xFFFF) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

uint16_t suns_value_get_uint16(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_UINT16);

    return v->value.u16;
}


void suns_value_set_int16(suns_value_t *v, int16_t i16)
{
    assert(v != NULL);

    v->value.i16 = i16;
    v->tp.type = SUNS_INT16;
    /* must cast 0x8000 to int16_t, otherwise the compiler
       interprets the value as a plain int (int32_t on i386) */
    if (i16 == (int16_t) 0x8000) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

int16_t suns_value_get_int16(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_INT16);

    return v->value.i16;
}


void suns_value_set_acc16(suns_value_t *v, uint16_t u16)
{
    assert(v != NULL);

    v->value.u16 = u16;
    v->tp.type = SUNS_ACC16;
    v->meta = SUNS_VALUE_OK;
}

uint16_t suns_value_get_acc16(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_ACC16);

    return v->value.u16;
}


void suns_value_set_uint32(suns_value_t *v, uint32_t u32)
{
    assert(v != NULL);

    v->value.u32 = u32;
    v->tp.type = SUNS_UINT32;
    if (u32 == (uint32_t) 0xFFFFFFFF) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

uint32_t suns_value_get_uint32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_UINT32);

    return v->value.u32;
}


void suns_value_set_enum32(suns_value_t *v, uint32_t u32)
{
    assert(v != NULL);

    v->value.u32 = u32;
    v->tp.type = SUNS_ENUM32;
    if (u32 == (uint32_t) 0xFFFFFFFF) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

uint32_t suns_value_get_enum32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_ENUM32);

    return v->value.u32;
}


void suns_value_set_int32(suns_value_t *v, int32_t i32)
{
    assert(v != NULL);

    v->value.i32 = i32;
    v->tp.type = SUNS_INT32;
    if (i32 == (int32_t) 0x80000000) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

int32_t suns_value_get_int32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_UINT32);

    return v->value.i32;
}


void suns_value_set_acc32(suns_value_t *v, uint32_t u32)
{
    assert(v != NULL);

    v->value.u32 = u32;
    v->tp.type = SUNS_ACC32;
    v->meta = SUNS_VALUE_OK;
}

uint32_t suns_value_get_acc32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_ACC32);

    return v->value.u32;
}


void suns_value_set_uint64(suns_value_t *v, uint64_t u64)
{
    assert(v != NULL);

    v->value.u64 = u64;
    v->tp.type = SUNS_UINT64;
    if (u64 == (uint64_t) 0xFFFFFFFFFFFFFFFF) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

uint64_t suns_value_get_uint64(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_UINT64);

    return v->value.u64;
}


void suns_value_set_int64(suns_value_t *v, int64_t i64)
{
    assert(v != NULL);

    v->value.i64 = i64;
    v->tp.type = SUNS_INT64;
    if (i64 == (int64_t) 0x8000000000000000) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

int64_t suns_value_get_int64(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_UINT64);

    return v->value.i64;
}


void suns_value_set_acc64(suns_value_t *v, uint64_t u64)
{
    assert(v != NULL);

    v->value.u64 = u64;
    v->tp.type = SUNS_ACC64;
    v->meta = SUNS_VALUE_OK;
}


uint64_t suns_value_get_acc64(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_ACC64);

    return v->value.u64;
}


void suns_value_set_float32(suns_value_t *v, float f32)
{
    assert(v != NULL);

    v->value.f32 = f32;
    v->tp.type = SUNS_FLOAT32;
    if (isnan(f32)) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}

float32_t suns_value_get_float32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_FLOAT32);

    return v->value.f32;
}


void suns_value_set_float64(suns_value_t *v, float64_t f64)
{
    assert(v != NULL);

    v->value.f64 = f64;
    v->tp.type = SUNS_FLOAT64;
    if (isnan(f64)) {
        v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    } else {
        v->meta = SUNS_VALUE_OK;
    }
}


float64_t suns_value_get_float64(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_FLOAT64);

    return v->value.f64;
}


void suns_value_set_enum16(suns_value_t *v, uint16_t u16)
{
    assert(v != NULL);
    
    v->value.u16 = u16;
    v->tp.type = SUNS_ENUM16;
    v->meta = SUNS_VALUE_OK;
}

uint16_t suns_value_get_enum16(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_ENUM16);

    return v->value.u16;
}

void suns_value_set_bitfield16(suns_value_t *v, uint16_t u16)
{
    assert(v != NULL);

    v->value.u16 = u16;
    v->tp.type = SUNS_BITFIELD16;
    v->meta = SUNS_VALUE_OK;
}

uint16_t suns_value_get_bitfield16(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_BITFIELD16);

    return v->value.u16;
}

void suns_value_set_bitfield32(suns_value_t *v, uint32_t u32)
{
    assert(v != NULL);

    v->value.u32 = u32;
    v->tp.type = SUNS_BITFIELD32;
    v->meta = SUNS_VALUE_OK;
}

uint32_t suns_value_get_bitfield32(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_BITFIELD32);

    return v->value.u32;
}

void suns_value_set_sunssf(suns_value_t *v, uint16_t u16)
{
    assert(v != NULL);

    v->value.u16 = u16;
    v->tp.type = SUNS_SF;
    v->meta = SUNS_VALUE_OK;
}

int16_t suns_value_get_sunssf(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_SUNSSF);

    return v->value.i16;
}


void suns_value_set_string(suns_value_t *v, char *string, size_t len)
{
    assert(v != NULL);
    assert(string);

    /* we may be re-casting this suns_value_t from some
       other value.  if so, we need to ignore v->value since
       it doesn't represent a string pointer */
    if (v->tp.type != SUNS_STRING) {
        v->value.s = NULL;
    }

    /* allocate some space if we need to */
    if (v->tp.len != len ||
        v->value.s == NULL) {
        v->value.s = realloc(v->value.s, len);
        v->tp.len = len;
    }

    memset(v->value.s, 0, len);
    /* suns strings aren't required to be null terminated, so don't leave
       space for a null byte */
    strncpy(v->value.s, string, len);
    v->tp.type = SUNS_STRING;
}

char * suns_value_get_string(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_STRING);

    return v->value.s;
}

void suns_value_set_ipv4(suns_value_t *v, uint32_t ipv4)
{
    assert(v != NULL);
    
    v->value.u32 = ipv4;
    v->tp.type = SUNS_IPV4;
    v->meta = SUNS_VALUE_OK;
}

uint32_t suns_value_get_ipv4(suns_value_t *v)
{
    assert(v != NULL);
    assert(v->tp.type == SUNS_IPV4);

    return v->value.u32;
}


/* search for a specified did
   returns the first suns_model_did_t matching the specified did value
   returns NULL if no matching did value is found */
suns_model_did_t *suns_find_did(list_t *did_list, uint16_t did)
{
    assert(did_list);

    suns_model_did_t *d;

    debug("looking up model for did %d", did);

    list_node_t *c;
    list_for_each(did_list, c) {
        d = c->data;
        if (d->did == did) {
            return d;
        }
    }

    return NULL;
}


/* take a binary data blob and decode it
   this relies on the global did_list to find the model */
suns_dataset_t *suns_decode_data(list_t *did_list,
                                 unsigned char *buf,
                                 size_t len)
{
    list_node_t *c;
    suns_model_did_t *did = NULL;
    suns_dataset_t *data;

    /* the first 2 bytes contain the did */
    uint16_t did_value = be16toh(*((uint16_t *)buf));

    /* the next 2 bytes contains the length */
    uint16_t did_len = be16toh(*((uint16_t *)buf + 1));

    did = suns_find_did(did_list, did_value);

    if (did == NULL) {
        warning("unknown did %d", did_value);
        return NULL;
    }
    
    suns_model_t *m = did->model;

    data = suns_dataset_new();
    if (data == NULL) {
        debug("suns_dataset_new() failed");
        return NULL;
    }
    data->did = did;

    debug("did %d found", did_value);

    debug("did_len = %d, m->len = %d", did_len, m->len);
    
    /* first check the length value

    if model-> len != model->base_len it means there is a variable
    length section at the end of the model
    */
       
    /* sanity check the provided length values */
    if (m->base_len != m->len) {
        /* check for repeating portion of the model */
        if ((did_len - m->base_len) % (m->len - m->base_len) != 0) {
            warning("repeating portion of the model (%d) is not a multiple "
                    "of the expected repeating portion (%d).  %d extra "
                    "registers have been ignored.",
                    did_len - m->base_len,
                    m->len - m->base_len,
                    (did_len - m->base_len) % (m->len - m->base_len));
            /* FIXME: need to mark a flag if we're going
               to generate a report */
        }
    } else {
        /* check that the length is what we expect */
        if (m->len != did_len) {
            /*
              The introduction of the "pad" value to some odd-numbered
              models has made producing this warning considerably more
              complex.  Here's the rule:

              If the model's last value is of type pad then we should
              not produce a warning if the device's model length is
              1 less than what is defined.  Otherwise, we still
              display a warning since the length shouldn't be different
              in another case.
              
              We always still try to decode the data.
            */
            suns_dp_t *dp = suns_model_last_dp(m);
            if ((did_len == (m->len - 1)) &&
                (dp->type_pair->type == SUNS_PAD) ) {
                warning("device does not use the trailing pad register "
                        "for model %d.",
                        did->did);
            } else if (did_len < m->len) {
                warning("length value (%d) in data block (%d) does "
                        "not match data model spec length (%d). "
                        "%d extra registers in the model have been ignored.",
                        did_len, did->did, m->len, m->len - did_len);
            } else {
                warning("length value (%d) in data block (%d) does "
                        "not match data model spec length (%d). "
                        "%d extra registers from the device "
                        "have been ignored.",
                        did_len, did->did, m->len, did_len - m->len);
            }

            /* FIXME: need to mark a flag if we're going
               to generate a report */
        }
    }
    if (did == NULL) {
        error("unknown model did %03d", did_value);
        return NULL;
    }
    
    /* now step through all the dp_blocks decoding data */
    int byte_offset = 0;
    list_for_each(m->dp_blocks, c) {
        byte_offset += suns_decode_dp_block(c->data,
                                            /* offset +4 for header */
                                            buf + byte_offset + 4,
                                            (did_len * 2) - byte_offset,
                                            data->values);
        if (byte_offset >= (did_len * 2) + 4) {
            error("buffer overrun in suns_decode_data(): byte offset %d of %d",
                  byte_offset, did_len * 2);
            return data;
        }
        
    }
    
    /* resolve scale factors in the resulting dataset */
    suns_resolve_scale_factors(data);

    return data;
}



/* fill in any implied offset fields */
void suns_model_fill_offsets(suns_model_t *m)
{
    list_node_t *c, *d;
    int offset = 0;

    list_for_each(m->dp_blocks, d) {
        suns_dp_block_t *dp_block = d->data;
        int dp_block_offset = 0;
        
        list_for_each(dp_block->dp_list, c) {
            suns_dp_t *dp = c->data;

            /* use a provided offset to set our check offset */
            if (dp->offset > 0) {
                dp_block_offset = dp->offset - offset;
            } else {
                /* fill in implied offset */
                dp->offset = offset + dp_block_offset;
            }

            /* advance offset by the size of the current data type */
            if (dp->type_pair->type == SUNS_STRING) {
                /* check if len is an odd number */
                if ( ((dp->type_pair->len / 2.0) -
                      (dp->type_pair->len / 2)) > 0) {
                    warning("datapoint %s is a string of odd length %d;"
                            "rounding up to whole register",
                            dp->name, dp->type_pair->len);
                    dp_block_offset += (dp->type_pair->len / 2) + 1;
                } else {
                    dp_block_offset += dp->type_pair->len / 2;
                }
            } else {
                dp_block_offset += suns_type_size(dp->type_pair->type) / 2;
            }
	    
        }
	
        dp_block->len = dp_block_offset;

        /* the repeating block can only be the last block, so throw
           a warning if we find one someplace else */
        if (dp_block->repeating) {
            if (dp_block != m->dp_blocks->tail->data) {
                error("repeating marker found on block "
                      "that is not last");
            }
        } else {
            /* m->base_len is everything up until the optional
               repeating block */
            m->base_len += dp_block_offset;
        }

        m->len += dp_block_offset;
    }

    debug_i(m->len);
}


/* decode the provided buffer buf of length len using the provided dp_block
   the results are appended to value_list and the total buffer length decoded
   is returned (which should be the same as the len argument)

   GOTCHA: all lengths and offset are in bytes, not modbus registers
   FIXME:  at some point i need to go through everything and pick one system */
int suns_decode_dp_block(suns_dp_block_t *dp_block,
                         unsigned char *buf,
                         size_t len,
                         list_t *value_list)
{
    int len_multiple;
    int byte_offset = 0;
    int i;
    list_node_t *c;
    suns_value_t *v;
    int repeat_index = 1;  /* repeat index is 1 based */

    if (dp_block->repeating) {
        debug("repeating block");
        len_multiple = len / dp_block->len / 2;
    } else {
        len_multiple = 1;
    }

    debug("len_multiple = %d", len_multiple);
    
    for (i = 0; i < len_multiple; i++) {
        list_for_each(dp_block->dp_list, c) {
            suns_dp_t *dp = c->data;
            int size = suns_type_pair_size(dp->type_pair);
            v = suns_value_new();

            if ((byte_offset + size) <= len) {
                suns_value_set_null(v);

                /* note if this value is part of a repeating block */
                v->repeating = dp_block->repeating;

                /* use accessors to keep v->name_with_index in sync */
                suns_value_set_name(v, dp->name);
                suns_value_set_index(v, repeat_index);

                suns_buf_to_value(buf + byte_offset, dp->type_pair, v);

                /* check for attributes we care about */
                v->units = suns_find_attribute(dp, "u");
                v->label = suns_find_attribute(dp, "label");

                /* debug("v->tp.type = %s", suns_type_string(v->tp.type)); */
                list_node_add(value_list, list_node_new(v));
                byte_offset += size;
		
            } else {
                if (dp->type_pair->type != SUNS_PAD) {
                    warning("%s offset %d defined in the model exceeds "
                            "length of device data",
                            dp->name, dp->offset + (i * dp_block->len));
                } else {
                    debug("ignoring missing pad register at offset %d",
                          dp->offset + (i * dp_block->len));
                }
                debug("requested = %d, len = %d",
                      (dp->offset * 2) + suns_type_size(dp->type_pair->type),
                      len);
                return byte_offset;
            }
        }
        repeat_index++;
    }

    return byte_offset;
}


suns_define_block_t *suns_search_define_blocks(list_t *list, char *name)
{
    list_node_t *c;

    list_for_each(list, c) {
        suns_define_block_t *block = c->data;
        debug_s(block->name);
        if (strcmp(block->name, name) == 0)
            return block;
    }

    /* if we didn't hit the define we're looking for then
       search the global define list */
    list_t *global = suns_get_define_list();
    list_for_each(global, c) {
        suns_define_block_t *block = c->data;
        debug_s(block->name);
        if (strcmp(block->name, name) == 0)
            return block;
    }    

    return NULL;
}


suns_define_t *suns_define_new(void)
{
    suns_define_t *d = malloc(sizeof(suns_define_t));
    if (d == NULL)
        return NULL;

    memset(d, 0, sizeof(suns_define_t));
    /* created in attributes symbol in yacc: */
    /* d->attributes = list_new(); */

    return d;
}


void suns_define_free(suns_define_t *d)
{
    assert(d != NULL);

    list_free(d->attributes, free);
    free(d);
}


suns_define_t *suns_search_enum_defines(list_t *list, unsigned int value)
{
    list_node_t *c;

    list_for_each(list, c) {
        suns_define_t *define = c->data;
        if (define->value == value) {
            return define;
        }
    }

    /* not found */
    debug("value %d not found", value);

    return NULL;
 }

 
suns_define_t *suns_search_bitfield_defines_i(list_t *list,
                                              unsigned int value,
                                              list_node_t **c)
{
    list_for_each(list, (*c)) {
        suns_define_t *define = (*c)->data;
        if (define->value & value) {
            return define;
        }
    }

    /* not found */
    debug("value 0x%x not found", value);

    return NULL;
}
        

suns_dp_t *suns_search_dp_block_for_dp_by_name(suns_dp_block_t *dp_block,
                                               char *name)
{
    list_node_t *c;

    list_for_each(dp_block->dp_list, c) {
        suns_dp_t *dp = c->data;
        if (strcmp(name, dp->name) == 0) {
            return dp;
        }
    }

    /* datapoint not found */
    return NULL;
}


/**
 * search a model for a suns_dp_t by name.
 *
 * \param *m model to search
 * \param *name name to search for
 * \param **dp_block_ref set to the suns_dp_block_t the suns_dp_t was found in
 */
suns_dp_t *suns_search_model_for_dp_by_name(suns_model_t *m,
                                            char *name,
                                            suns_dp_block_t **dp_block_ref)
{
    list_node_t *c;
    suns_dp_t *dp;

    list_for_each(m->dp_blocks, c) {
        suns_dp_block_t *dp_block = c->data;
        if ((dp = suns_search_dp_block_for_dp_by_name(dp_block, name))
            != NULL) {
            *dp_block_ref = dp_block;
            return dp;
        }
    }
    
    /* datapoint not found */
    *dp_block_ref = NULL;
    return NULL;
}


suns_value_t *suns_search_value_list(list_t *list, char *name)
{
    list_node_t *c;
    suns_value_t *v;

    list_for_each(list, c) {
        v = c->data;
        if (strcmp(name, v->name) == 0) {
            return v;
        }
    }

    return NULL;
}


/* search the values in a suns_dataset_t to resolve all the
   scale factor pointers */
int suns_resolve_scale_factors(suns_dataset_t *dataset)
{
    list_node_t *c;
    suns_value_t *v;
    suns_value_t *found;

    list_for_each(dataset->values, c) {
        v = c->data;
        if (v->tp.name && suns_type_is_numeric(v->tp.type)) {
            found = suns_search_value_list(dataset->values, v->tp.name);
            if (found) {
                /* referenced value must be a sunssf type! */
                if (found->tp.type != SUNS_SF) {
                    error("scale factor subscript for %s is not a sunsf type",
                          found->name);
                    /* bail - model must be corrected. */
                    return -1;
                }

                /* check if the scale factor is implemented */
                if (found->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
                    if ((v->meta != SUNS_VALUE_NOT_IMPLEMENTED) &&
                        (! suns_value_acc_is_zero(v)) ) {
                        warning("implemented datapoint %s references the "
                                "not-implemented scale factor %s",
                                v->name, found->name);
                    }
                    /* put zero in the scale factor so we
                       produce a meaningful result */
                    v->tp.sf = 0;
                } else {
                    /* stash scale factor in the suns_value_t */
                    v->tp.sf = suns_value_get_sunssf(found);
                }
            }
        }
    }

    return 0;
}


char * suns_find_attribute(suns_dp_t *dp, char *name)
{
    list_node_t *c;
    
    if (dp->attributes) {
        list_for_each(dp->attributes, c) {
            suns_attribute_t *a = c->data;
            if (strcmp(a->name, name) == 0) {
            return a->value;
            }
        }
    }

    /* not found */
    return NULL;
}


/**
 * create a concatenated string of all did numbers in a model
 *
 */
int suns_did_number_string(suns_model_t *m, char *buf, size_t len)
{
    list_node_t *c;
    int offset = 0;
    int first = 1;

    list_for_each(m->did_list, c) {
        suns_model_did_t *did = c->data;
        if (first) {
            offset += snprintf(buf + offset, len - offset, "%d", did->did);
            first = 0;
        } else {
            offset += snprintf(buf + offset, len - offset, " %d", did->did);
        }

        if (offset >= len)
            break;
    }

    debug_s(buf);

    return offset;
}


/**
 * checks scale factor references in a suns_model_t for consistency
 * returns 0 on success, 1 if there are any invalid scale factor references
 */
int suns_check_scale_factors(suns_model_t *m)
{
    int rc = 0;
    list_node_t *c, *d;

    char did_string[BUFFER_SIZE];
    suns_did_number_string(m, did_string, BUFFER_SIZE);

    verbose(1, "checking %s (%s)", m->name, did_string);

    list_for_each(m->dp_blocks, c) {
        suns_dp_block_t *block = c->data;
        list_for_each(block->dp_list, d) {
            suns_dp_block_t *dp_block_ref;
            suns_dp_t *dp = d->data;
            suns_dp_t *sf;
            /* check if we've found a scale factor that has the
               name field set.  we want to skip datapoints that
               have fixed scale factors. */
            if (suns_type_is_numeric(dp->type_pair->type) &&
                dp->type_pair->name) {
                debug("checking %s", dp->name);
                sf = suns_search_model_for_dp_by_name(m,
                                                      dp->type_pair->name,
                                                      &dp_block_ref);
                if (sf) {
                    /* check that a datapoint in the non-repeating
                       section does not reference a scale factor in
                       the repeating section. */
                    if ((! block->repeating) &&
                        (dp_block_ref->repeating)) {
                        error("datapoint %s, which is in the non-repeating "
                              "block, references a scale factor in the "
                              "repeating block", dp->name);
                        rc = -1;
                    }
                } else {
                    error("datapoint %s in model \"%s\" references "
                          "non-existent scale factor %s",
                          dp->name, m->name, dp->type_pair->name);
                    rc = -1;
                }
            }
        }
    }

    return rc;
}


/**
 * check a model for internal consistency and missing attributes
 *
 */
int suns_model_check_consistency(suns_model_t *m)
{
    int rc = 0;

    /* check that all scale references refer to actual datapoints */
    rc = suns_check_scale_factors(m);

    if (m->name == NULL) {
        error("model containing did %d is missing the name string",
              ((suns_model_did_t*)(m->did_list->head->data))->did);
        rc = -1;
    }

    list_node_t *c;
    list_for_each(m->did_list, c) {
        suns_model_did_t *did = c->data;
        if (did->name == NULL) {
            if (m->name)
                error("did %d in model \"%s\" is missing the name string",
                      did->did, m->name);
            else
                error("did %d is missing the name string", did->did);
        }
    }

    return rc;
}


/**
 * resolve all define pointers in the model
 */
void suns_model_resolve_defines(suns_model_t *m)
{
    list_node_t *c, *d;

    debug("HERE");

    list_for_each(m->dp_blocks, c) {
        suns_dp_block_t *block = c->data;
        list_for_each(block->dp_list, d) {
            suns_dp_t *dp = d->data;
            debug("point: %s, type: %s", dp->name,
                  suns_type_string(dp->type_pair->type));
            if (suns_type_is_symbolic(dp->type_pair->type)) {
                if (dp->type_pair->name) {
                    debug("searching for define: %s", dp->type_pair->name);
                    /* fill in define
                       dp->type_pair->define is set to NULL
                       if here is no define */
                    dp->type_pair->define =
                        suns_search_define_blocks(m->defines,
                                                  dp->type_pair->name);
                } else {
                    /* only complain about missing defines with -vvv

                       since there are a lots of spare bitfields and enums
                       with no definitions this keeps the output tidier
                       for doing comformance reports */
                    if (verbose_level > 2) {
                        warning("No define for %s point %s in model \"%s\"",
                                suns_type_string(dp->type_pair->type),
                                dp->name, m->name);
                    }
                }
            }
        }
    }
}


/**
 * Return the last datapoint in a model
 *
 */
suns_dp_t *suns_model_last_dp(suns_model_t *m)
{
    if (m == NULL) {
        debug("m is a NULL pointer");
        return NULL;
    }

    if (m->dp_blocks == NULL) {
        debug("m->dp_blocks is a NULL pointer");
        return NULL;
    }

    /* list_t provides us with list->tail, which is a pointer to the
       last node in the list */
    suns_dp_block_t *dp_block = m->dp_blocks->tail->data;
    suns_dp_t *dp = dp_block->dp_list->tail->data;

    return dp;
}


/**
 * Given a new dataset's did, search the datasets attached to a
 * suns_device_t to determine the index of a new dataset.
 * This is used to compute the index value used to uniquely identify
 * a model's dataset when models are repeated on a given device.
 *
 * This algorithm is a bit tricky b/c no index is set if a model
 * is the only instance of a did on the device.  However, if more
 * than one of a given model are on a device then we need to number
 * them starting with 1.  Hence the side effect.
 *
 * This function must be called before the new did is added to
 * the dataset, or it will be "discovered."
 *
 * side effects: sets the index of the first occurance of a dataset
 *               to 1
 */
int suns_model_get_did_index(suns_device_t *device, uint16_t did)
{
    /* first check if there are no datasets in the device */
    if (list_count(device->datasets) == 0) {
        return 0;
    }
    /* check to see if the previously added value is the same as
       the current model as a shortcut for setting the index value
       when the same model repeats on a given device.  this will mean
       we avoid n^2 runtime in the most common case */
    if (device->datasets->tail) {
        suns_dataset_t *last = device->datasets->tail->data;
        if (last->did->did == did) {
            /* this is tricky: a dataset only gets an index if
               it is needed to uniquely identify it.  since indices start
               at 1 we use 0 to mean no index.  this means if we've found
               the 2nd instance of a model we need to set the index
               on the first instance. */
            if (last->index == 0) {
                last->index = 1;
                return 2;
            } else {
                return last->index + 1;
            }
        }
    }

    /* if we're here then the shortcut didn't work and we need to
       search all datasets so far in the device */
    list_node_t *c;
    suns_dataset_t *found = NULL;
    list_for_each (device->datasets, c) {
        suns_dataset_t *dataset = c->data;
        if (dataset->did->did == did)
            found = dataset;
    }
    
    /* again with the tricky: set the index of the last dataset
       with the same did to 1 if it is the first instance of a did
       we have just found again */
    if (found) {
        if (found->index == 0) {
            found->index = 1;
            return 2;
        } else {
            return found->index + 1;
        }
    }

    /* we have found no previous instance of the did */
    return 0;
}
