/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_model.h
 *
 * This is where all the data structures used to hold the sunspec model
 * as well as retrieved data are defined.
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


#ifndef _SUNS_MODEL_H_
#define _SUNS_MODEL_H_

#include <stdlib.h>
#include <stdint.h>

#include "trx/list.h"
#include "trx/buffer.h"


#define SUNS_ID_HIGH 0x5375
#define SUNS_ID_LOW  0x6e53


/* define floating point types */
typedef float float32_t;
typedef double float64_t;

/* if you add or remove a numeric type you must update
   suns_value_is_numeric() appropriately */
/* the order of values must match the order in
   suns_type_string() ! */
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
    SUNS_ACC64,
    SUNS_ENUM16,
    SUNS_ENUM32,
    SUNS_BITFIELD16,
    SUNS_BITFIELD32,
    SUNS_SF,
    SUNS_STRING,
    SUNS_PAD,
    SUNS_IPADDR,
    SUNS_UNDEF,  /* must always be last item */
} suns_type_t;


typedef struct suns_type_pair {
    suns_type_t type;

    /* the type's subscript is one of the following: */
    char *name;        /* scale factor name (other datapoint) */
    size_t len;        /* string length */
    int sf;            /* fixed scale factor */
                       /* also used to store scale factors from logger xml */
} suns_type_pair_t;


typedef struct suns_dp_block {
    int repeating;
    char *feature;    /* not sure i'm using this for anything */
    list_t *dp_list;
    int len;
} suns_dp_block_t;

typedef struct suns_model {
    char *comment;
    char *name;
    char *type;
    list_t *did_list;
    uint16_t len;
    uint16_t base_len;     /* some models have a fixed "header"
                              followed by a variable length section */
    list_t *dp_blocks;
    list_t *defines;
    list_t *test_data;
} suns_model_t;

typedef struct suns_dp {
    char *name;
    int offset;            /* in modbus registers, starting with 1 */
    suns_type_pair_t *type_pair;
    list_t *attributes;
} suns_dp_t;

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
    list_t *attributes;
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
    char *name_with_index;     /* composite name including index */
    char *lname;               /* long/legacy name */
    suns_type_pair_t tp;       /* type_pair of value (note: not a pointer) */
    suns_value_meta_t meta;    /* meta-value (null, error, etc.) */
    union {                    /* actual value */
        uint16_t u16;          
        uint32_t u32;
        uint64_t u64;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        float32_t f32;
        float64_t f64;
        char *s;             /* string - must be free()d if not NULL */
    } value;
    unsigned char raw[SUNS_VALUE_RAW_SIZE]; /* "raw" from-the-wire data */
    size_t raw_len;          /* length of the raw data */
    int index;               /* index in repeating blocks */
    int repeating;           /* is this value part of a repeating block? */
    time_t unixtime;         /* optional value-specific timestamp */
    int usec;                /* optional microsecond component */
    char *units;             /* optional units string */
    char *description;       /* optional descriptive string */    
    char *label;             /* short descriptive label */
} suns_value_t;


/* datasets are a collection of datapoints associated with a single
   sunspec model */
typedef struct suns_dataset {
    suns_model_did_t *did;
    list_t *values;  /* list of suns_value_t */

    /* used for logger upload xml parsing */ 
    char *ns;         /* namespace, for vendor extension blocks */
    
    int index;        /* index, for aggregated devices */
    int unixtime;
    int usec;
} suns_dataset_t;


/* a logical device */
typedef struct suns_device {
    int addr;
    list_t *datasets;
    time_t unixtime;
    int usec;

    /* common is set when the common block dataset is added,
       for easy reference */
    suns_dataset_t *common;

    /* these are set when the common block dataset is added or
       the logger xml is parsed, also for easy reference */
    char *manufacturer;
    char *model;
    char *serial_number;

    /* attributes set in the logger xml format */
    char *cid;    /* correlation id */
    char *id;     /* optional device id string (overrides man, mod and sn) */
    char *iface;  /* optional interface id string (only if d.id is used) */
    char *lid;    /* logger id string; required by default */
    char *ns;     /* domain namespace for the logger id */
} suns_device_t;
    

typedef struct suns_attribute {
    char *name;
    char *value;
    list_t *list;
} suns_attribute_t;


typedef enum suns_audit_type_t {
    SUNS_AUDIT_NOTICE,
    SUNS_AUDIT_WARN,
    SUNS_AUDIT_ERR,
} suns_audit_type_t;


typedef struct suns_audit {
    suns_audit_type_t type;
    char *name;
    char *msg;
} suns_audit_t;



char * suns_type_string(suns_type_t type);
suns_type_pair_t *suns_type_pair_new(void);
suns_dp_block_t *suns_dp_block_new(void);
suns_dp_t *suns_dp_new(void);
char * suns_value_meta_string(suns_value_meta_t meta);
suns_type_t suns_type_from_name(char *name);
void suns_model_free(suns_model_t *model);
void suns_model_dp_block_free(suns_dp_block_t *dp_block);
suns_data_t *suns_data_new(void);
int suns_data_add_reg(suns_data_t *block, uint16_t reg);
int suns_data_check_size(suns_data_t *block,
			       size_t needed_size);
void suns_data_free(suns_data_t *block);
suns_data_block_t * suns_data_block_new(void);
void suns_data_block_free(suns_data_block_t *block);
suns_define_block_t * suns_define_block_new(void);
void suns_define_block_free(suns_define_block_t *block);

int suns_type_size(suns_type_t type);
int suns_type_pair_size(suns_type_pair_t *tp);
int suns_value_to_buf(suns_value_t *v, unsigned char *buf, size_t len);
int suns_value_to_buffer(buffer_t *buf, suns_value_t *v);
suns_value_meta_t suns_check_not_implemented(suns_type_pair_t *tp,
                                             suns_value_t *v);
int suns_buf_to_value(unsigned char *buf,
                      suns_type_pair_t *tp,
                      suns_value_t *v);

int suns_value_is_numeric(suns_value_t *v);
int suns_value_is_acc(suns_value_t *v);
int suns_value_acc_is_zero(suns_value_t *v);

suns_dataset_t *suns_dataset_new(void);
void suns_dataset_free(suns_dataset_t *d);

suns_device_t *suns_device_new(void);
void suns_device_free(suns_device_t *d);
int suns_device_add_dataset(suns_device_t *d, suns_dataset_t *data);

/* suns_value_t stuff */
suns_model_t *suns_model_new(void);
suns_model_did_t *suns_model_did_new(char *name,
                                     uint16_t id);
int suns_string_to_value(const char *string,
                         suns_value_t *v,
                         suns_type_pair_t *tp);

suns_value_t *suns_value_new(void);
void suns_value_free(suns_value_t *v);
void suns_value_init(suns_value_t *v);
int suns_value_set_index(suns_value_t *v, int index);
int suns_value_set_name(suns_value_t *v, char *name);
void suns_value_set_null(suns_value_t *v);
void suns_value_set_uint16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_uint16(suns_value_t *v);
void suns_value_set_int16(suns_value_t *v, int16_t i16);
int16_t suns_value_get_int16(suns_value_t *v);
void suns_value_set_uint32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_uint32(suns_value_t *v);
void suns_value_set_enum32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_enum32(suns_value_t *v);
void suns_value_set_int32(suns_value_t *v, int32_t i32);
int32_t suns_value_get_int32(suns_value_t *v);
void suns_value_set_float32(suns_value_t *v, float32_t f32);
void suns_value_set_uint64(suns_value_t *v, uint64_t u64);
uint64_t suns_value_get_uint64(suns_value_t *v);
void suns_value_set_int64(suns_value_t *v, int64_t i64);
int64_t suns_value_get_int64(suns_value_t *v);
float32_t suns_value_get_float32(suns_value_t *v);
void suns_value_set_float64(suns_value_t *v, float64_t f64);
float64_t suns_value_get_float64(suns_value_t *v);
float suns_value_get_float32(suns_value_t *v);
void suns_value_set_enum16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_enum16(suns_value_t *v);
void suns_value_set_bitfield16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_bitfield16(suns_value_t *v);
void suns_value_set_bitfield32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_bitfield32(suns_value_t *v);
void suns_value_set_sunssf(suns_value_t *v, uint16_t u16);
int16_t suns_value_get_sunssf(suns_value_t *v);
void suns_value_set_string(suns_value_t *v, char *string, size_t len);
char * suns_value_get_string(suns_value_t *v);
void suns_value_set_acc16(suns_value_t *v, uint16_t u16);
uint16_t suns_value_get_acc16(suns_value_t *v);
void suns_value_set_acc32(suns_value_t *v, uint32_t u32);
uint32_t suns_value_get_acc32(suns_value_t *v);
void suns_value_set_acc64(suns_value_t *v, uint64_t u64);
uint64_t suns_value_get_acc64(suns_value_t *v);
void suns_value_set_ipaddr(suns_value_t *v, uint32_t ipaddr);
uint32_t suns_value_get_ipaddr(suns_value_t *v);
suns_attribute_t *suns_attribute_new(void);
void suns_attribute_list_free(list_t *l);
void suns_attribute_free(void *a);


suns_model_did_t *suns_find_did(list_t *did_list, uint16_t did);
suns_dataset_t *suns_decode_data(list_t *did_list,
				 unsigned char *buf,
				 size_t len);

int suns_decode_dp_block(suns_dp_block_t *dp_block,
			 unsigned char *buf,
			 size_t len,
			 list_t *value_list);

void suns_model_fill_offsets(suns_model_t *m);

suns_define_block_t *suns_search_define_blocks(list_t *list, char *name);
suns_define_t *suns_define_new(void);
void suns_define_free(suns_define_t *d);
suns_define_t *suns_search_enum_defines(list_t *list, unsigned int value);
suns_define_t *suns_search_bitfield_defines_i(list_t *list,
                                              unsigned int value,
                                              list_node_t **c);

suns_dp_t *suns_search_dp_block_for_dp_by_name(suns_dp_block_t *dp_block,
                                               char *name);
suns_dp_t *suns_search_model_for_dp_by_name(suns_model_t *model, char *name);
suns_value_t *suns_search_value_list(list_t *list, char *name);
int suns_resolve_scale_factors(suns_dataset_t *dataset);
char * suns_find_attribute(suns_dp_t *dp, char *name);
void suns_model_xml_define_block_fprintf(FILE *stream,
                                         suns_define_block_t *block);
void suns_model_xml_define_fprintf(FILE *stream,
                                   suns_define_t *define);

#endif /* _SUNS_MODEL_H_ */
