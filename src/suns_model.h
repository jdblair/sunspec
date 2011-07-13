/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_model.h
 * $Id: $
 *
 * This is where all the data structures used to hold the sunspec model
 * as well as retrieved data are defined.
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


#ifndef _SUNS_MODEL_H_
#define _SUNS_MODEL_H_

#include <stdlib.h>
#include <stdint.h>

#include "trx/list.h"
#include "trx/buffer.h"


#define SUNS_ID_HIGH 0x5375
#define SUNS_ID_LOW  0x6e53

typedef enum suns_type {
    SUNS_NULL = 0,  /* just initialized value */
    SUNS_INT16,
    SUNS_UINT16,
    SUNS_ACC16,
    SUNS_INT32,
    SUNS_UINT32,
    SUNS_FLOAT32,
    SUNS_ACC32,
    SUNS_INT64,
    SUNS_UINT64,
    SUNS_FLOAT64,
    SUNS_ENUM16,
    SUNS_BITFIELD16,
    SUNS_BITFIELD32,
    SUNS_SF,
    SUNS_STRING,
    SUNS_UNDEF,  /* must always be last item */
} suns_type_t;


typedef struct suns_type_pair {
    suns_type_t type;

    union {
	char *name;        /* scale factor name */
	size_t len;        /* string length */
	int sf;            /* scale factor value */
    } sub;                 /* sub = "subscript" */

    /* shortcut is an optimization
       once a subscript has been resolved we tuck its actual
       value here so we don't have to look it up every time we use it */
    union {
	struct suns_value  *sf;
	struct suns_define *define;
    } shortcut;

} suns_type_pair_t;


typedef struct suns_dp_block {
    int repeating;
    char *feature;
    list_t *dp_list;
    int len;
} suns_dp_block_t;

typedef struct suns_dp {
    char *name;
    int offset;            /* in modbus registers, starting with 1 */
    suns_type_pair_t *type_pair;
} suns_dp_t;

typedef struct suns_model {
    char *comment;
    char *name;
    char *type;
    list_t *did_list;
    uint16_t len;
    uint16_t base_len;     /* some models have a fixed "header"
			      followed by a variable length section */
    
    /* suns_dp_block_t *dp_block; */
    list_t *dp_blocks;

    list_t *defines;
    
    list_t *enums;
    list_t *bitfields;
    list_t *test_data;
    /* suns_dp_t *dp_index; */  /* null terminated array */
} suns_model_t;

/*  suns_model_did_t is used to build an index of did values
    more than one did can reference the same model */
typedef struct suns_model_did {
    uint16_t did;
    char *name;
    suns_model_t *model;
} suns_model_did_t;

/* used to hold test data */
typedef struct suns_data {
    uint16_t offset;
    buffer_t *data;
} suns_data_t;


typedef struct suns_data_block {
    char *name;
    buffer_t *data;
} suns_data_block_t;

typedef struct suns_value_unsigned_decimal {
    unsigned int u;
    int p;            /* power of 10 */
} suns_value_unsigned_decimal_t;

typedef struct suns_value_decimal {
    signed int u;
    int p;            /* power of 10 */
} suns_value_decimal_t;


/* define blocks are used to store auxiliary information about data models
   - enum & bitfield human readable names
   - test data blocks
*/
typedef struct suns_define_block {
    char *name;
    char *type;
    list_t *list;
} suns_define_block_t;


typedef struct suns_define {
    char *name;
    unsigned int value;
    char *string;
} suns_define_t;
    
/* named_list_t is used to store general lists of suns_value_t
   but it could really be used to store named lists of anything
*/
typedef struct named_list {
    char *name;
    list_t *list;
} named_list_t;



typedef enum suns_value_meta {
    SUNS_VALUE_NULL = 0,          /* default value is NULL */
    SUNS_VALUE_OK,                /* value is normal */
    SUNS_VALUE_NOT_IMPLEMENTED,
    SUNS_VALUE_ERROR,
    SUNS_VALUE_UNDEF,             /* must stay last */
} suns_value_meta_t;


#define SUNS_VALUE_RAW_SIZE 8
typedef struct suns_value {
    char *name;                /* datapoint name associated with this */
    suns_type_pair_t tp;       /* type_pair of value */
    suns_value_meta_t meta;    /* meta-value (null, error, etc.) */
    union {                    /* actual value */
	uint16_t u16;
	uint32_t u32;
	int16_t i16;
	int32_t i32;
	float f32;             /* FIXME: host float size may be wrong! */
	char *s;               /* string - must be free()d if not NULL */
    } value;
    unsigned char raw[SUNS_VALUE_RAW_SIZE]; /* "raw" from-the-wire data */
    size_t raw_len;            /* length of the raw data */
} suns_value_t;


typedef struct suns_dataset {
    int addr;
    struct timeval *timestamp;
    suns_model_did_t *did;
    list_t *values;
} suns_dataset_t;


typedef void (*suns_model_fprintf_f)(FILE *stream, suns_model_t *model);

typedef struct suns_model_export_format {
    char *name;
    suns_model_fprintf_f fprintf;
} suns_model_export_format_t;


typedef int (*suns_dataset_fprintf_f)(FILE *stream,
				      suns_dataset_t *data);

typedef struct suns_dataset_output_format {
    char *name;
    suns_dataset_fprintf_f fprintf;
} suns_dataset_output_format_t;





char * suns_type_string(suns_type_t type);
suns_type_pair_t *suns_type_pair_new(suns_type_t type);
char * suns_value_meta_string(suns_value_meta_t meta);
suns_type_t suns_type_from_name(char *name);
void suns_model_free(suns_model_t *model);
void suns_model_dp_block_free(suns_dp_block_t *dp_block);
suns_data_t *suns_data_new(void);
int suns_data_add_reg(suns_data_t *block, uint16_t reg);
int suns_data_check_size(suns_data_t *block,
			       size_t needed_size);
void suns_data_free(suns_data_t *block);
int suns_type_size(suns_type_t type);
int suns_type_pair_size(suns_type_pair_t *tp);
int suns_value_to_buf(suns_value_t *v, unsigned char *buf, size_t len);
int suns_value_to_buffer(buffer_t *buf, suns_value_t *v);
int suns_buf_to_value(unsigned char *buf,
		      suns_type_pair_t *tp,
		      suns_value_t *v);
suns_dataset_t *suns_dataset_new(suns_model_did_t *did);
void suns_dataset_free(suns_dataset_t *d);

/* suns_value_t stuff */
suns_model_t *suns_model_new();
suns_value_t *suns_value_new(void);
void suns_value_free(void *v);
void suns_value_init(suns_value_t *v);
int suns_snprintf_value(char *str, size_t size, suns_value_t *v);
void suns_value_set_null(suns_value_t *v);
void suns_value_set_uint16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_uint16(suns_value_t *v);
void suns_value_set_int16(suns_value_t *v, int16_t i16);
int16_t suns_value_get_int16(suns_value_t *v);
void suns_value_set_uint32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_uint32(suns_value_t *v);
void suns_value_set_int32(suns_value_t *v, int32_t i32);
int32_t suns_value_get_int32(suns_value_t *v);
void suns_value_set_float32(suns_value_t *v, float f32);
float suns_value_get_float32(suns_value_t *v);
void suns_value_set_enum16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_enum16(suns_value_t *v);
void suns_value_set_bitfield16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_bitfield16(suns_value_t *v);
void suns_value_set_bitfield32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_bitfield32(suns_value_t *v);
void suns_value_set_sunssf(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_sunssf(suns_value_t *v);
void suns_value_set_string(suns_value_t *v, char *string, size_t len);
char * suns_value_get_string(suns_value_t *v);
void suns_value_set_acc16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_acc16(suns_value_t *v);
void suns_value_set_acc32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_acc32(suns_value_t *v);

suns_model_did_t *suns_find_did(list_t *did_list, uint16_t did);
suns_dataset_t *suns_decode_data(list_t *did_list,
				 unsigned char *buf,
				 size_t len);

int suns_decode_dp_block(suns_dp_block_t *dp_block,
			 unsigned char *buf,
			 size_t len,
			 list_t *value_list);

void suns_model_fill_offsets(suns_model_t *m);

suns_define_t *suns_search_enum_defines(list_t *list, unsigned int value);
suns_define_t *suns_search_bitfield_defines(list_t *list, unsigned int value);

#endif /* _SUNS_MODEL_H_ */



