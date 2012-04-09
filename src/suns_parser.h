
/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_parser.h
 *
 * mostly glue code to interact with yacc
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


#ifndef _SUNS_PARSER_H_
#define _SUNS_PARSER_H_

#include <stdio.h>
#include <stdlib.h>

#include "ezxml/ezxml.h"

#include "suns_model.h"

typedef struct suns_parser_state {
    char *model_file;
    FILE *input_file;

    /* the rest of this is the parsed s-lang document
       which describes the sunspec data models and all
       of the test data blocks */
    list_t *model_list;          /* all models */
    list_t *did_list;            /* index of all dids (dids > models) */
    list_t *data_block_list;     /* static test data blocks */
    list_t *define_list;         /* global defines */
} suns_parser_state_t;

/* prototypes so compiling bison and flex output doesn't cause warnings */
/* crap, there are still warnings.  sigh.  */
void yyerror(const char *str);
int yylex(void);
int yywrap();

void suns_parser_init(void);
suns_dp_t * suns_model_find_dp_by_name(list_t *list, char *name);
int parser_getopt(int argc, char *argv[]);
int suns_parse_model_file(const char *file);

/* accessor used to access the global parser state */
suns_parser_state_t *suns_get_parser_state(void);

/* accessors used to access the parsed suns model definition file */
list_t *suns_get_model_list(void);
list_t *suns_get_did_list(void);
list_t *suns_get_data_block_list(void);
list_t *suns_get_define_list(void);
int suns_parse_xml_model_file(const char *file);
suns_dp_block_t *suns_ezxml_to_dp_block(ezxml_t b);
suns_dp_t *suns_ezxml_to_dp(ezxml_t p);

#endif /* _SUNS_PARSER_H_ */
