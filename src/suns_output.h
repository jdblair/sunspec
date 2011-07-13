/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output.h
 * $Id: $
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


void suns_dp_fprint(FILE *stream, suns_dp_t *dp);
void suns_define_block_fprint(FILE *stream, suns_define_block_t *block);
void suns_define_fprint(FILE *stream, suns_define_t *define);
int suns_snprintf_type_pair(char *str, size_t size, suns_type_pair_t *tp);
void suns_type_pair_fprint(FILE *stream, suns_type_pair_t *tp);
void suns_model_fprintf(FILE *stream, suns_model_t *model);
void suns_dp_block_fprintf(FILE *stream, suns_dp_block_t *dp_block);
void suns_data_fprintf(FILE *stream, suns_data_t *data);
void suns_data_block_fprintf(FILE *stream, suns_data_block_t *block);
int suns_model_export(char *type, suns_model_t *model, FILE *stream);
int suns_model_export_all(char *fmt, list_t *model_list, FILE *stream);
int suns_dataset_text_fprintf(FILE *stream, suns_dataset_t *data);
int suns_dataset_output(char *fmt, suns_dataset_t *data, FILE *stream);
void suns_model_sql_fprintf(FILE *stream, suns_model_t *model);
int suns_dataset_sql_fprintf(FILE *stream, suns_dataset_t *data);
void suns_model_csv_fprintf(FILE *stream, suns_model_t *model);
int suns_dataset_csv_fprintf(FILE *string, suns_dataset_t *data);