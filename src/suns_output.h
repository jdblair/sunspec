/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output.h
 *
 * header file for the output functions
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


#ifndef _SUNS_OUTPUT_H_
#define _SUNS_OUTPUT_H_

typedef int (*suns_value_snprintf_f)(char *buf, size_t len,
                                     suns_value_t *value);

/* this vector table describes functions for converting each value type
   into a string.  this lets us describe a "driver" that handles the
   output idiosyncracies of any one given format.

   for example, the xml format needs to xml-escape all strings.
*/
typedef struct suns_value_output_vector {
    suns_value_snprintf_f null;
    suns_value_snprintf_f undef;
    suns_value_snprintf_f int16;
    suns_value_snprintf_f uint16;
    suns_value_snprintf_f acc16;
    suns_value_snprintf_f int32;
    suns_value_snprintf_f uint32;
    suns_value_snprintf_f float32;
    suns_value_snprintf_f acc32;
    suns_value_snprintf_f enum16;
    suns_value_snprintf_f bitfield16;
    suns_value_snprintf_f bitfield32;
    suns_value_snprintf_f sunssf;
    suns_value_snprintf_f string;
    suns_value_snprintf_f meta;
} suns_value_output_vector_t;
    
typedef int (*suns_model_list_fprintf_f)(FILE *stream,
                                          char *type,
                                          list_t *model_list);

typedef struct suns_model_list_export_format {
    char *name;
    suns_model_list_fprintf_f fprintf;
} suns_model_list_export_format_t;

typedef void (*suns_model_fprintf_f)(FILE *stream,
                                    suns_model_t *model);

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


typedef int (*suns_device_fprintf_f)(FILE *stream,
				      suns_device_t *data);

typedef struct suns_device_output_format {
    char *name;
    suns_device_fprintf_f fprintf;
} suns_device_output_format_t;


void suns_dp_fprint(FILE *stream, suns_dp_t *dp);
void suns_define_block_fprint(FILE *stream, suns_define_block_t *block);
void suns_define_fprint(FILE *stream, suns_define_t *define);
int suns_snprintf_type_pair(char *str, size_t size, suns_type_pair_t *tp);
void suns_type_pair_fprint(FILE *stream, suns_type_pair_t *tp);
void suns_model_fprintf(FILE *stream, suns_model_t *model);
void suns_dp_block_fprintf(FILE *stream, suns_dp_block_t *dp_block);
void suns_data_fprintf(FILE *stream, suns_data_t *data);
void suns_data_block_fprintf(FILE *stream, suns_data_block_t *block);
int suns_model_export(FILE *stream, char *type, suns_model_t *model);
int suns_model_export_all(FILE *stream, char *type, list_t *model_list);
int suns_dataset_text_fprintf(FILE *stream, suns_dataset_t *data);
int suns_dataset_output(char *fmt, suns_dataset_t *data, FILE *stream);
int suns_device_output(char *fmt, suns_device_t *device, FILE *stream);
void suns_model_sql_fprintf(FILE *stream, suns_model_t *model);
int suns_dataset_sql_fprintf(FILE *stream, suns_dataset_t *data);
void suns_model_csv_fprintf(FILE *stream, suns_model_t *model);
int suns_dataset_csv_fprintf(FILE *string, suns_dataset_t *data);
int suns_dataset_xml_fprintf(FILE *stream, suns_dataset_t *data);
int suns_device_xml_fprintf(FILE *stream, suns_device_t *device);

int suns_snprintf_value(char *str, size_t size,
                        suns_value_t *v, suns_value_output_vector_t *fmt);
void suns_model_fprintf(FILE *stream, suns_model_t *model);
int suns_snprintf_value_text(char *str, size_t size,
                             suns_value_t *v);
int suns_snprintf_value_sf_text(char *str, size_t size,
                                suns_value_t *v);
int suns_snprintf_value_sql(char *str, size_t size,
                            suns_value_t *v);
int suns_device_text_fprintf(FILE *stream, suns_device_t *device);
void suns_registers_fprintf(FILE * stream,
                            unsigned char *buf, size_t len,
                            char *line_prefix);
void suns_binary_model_fprintf(FILE *stream, list_t *did_list,
                               unsigned char *buf, size_t len);
void suns_model_xml_fprintf(FILE *stream, suns_model_t *model);
int suns_model_xml_export_all(FILE *stream, char *type, list_t *list);
void suns_model_xml_dp_block_fprintf(FILE *stream, suns_dp_block_t *dp_block);
void suns_model_xml_dp_fprintf(FILE *stream, suns_dp_t *dp);

#endif /* _SUNS_OUTPUT_H_ */
