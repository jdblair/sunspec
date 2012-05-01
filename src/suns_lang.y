/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_lang.y
 * $Id: *
 *
 * bison (yacc) grammer file for the sunspec model description language.
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


%{
#define _BSD_SOURCE  /* for big/little endian macros */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>

#include "trx/macros.h"
#include "trx/debug.h"
#include "trx/list.h"
#include "trx/buffer.h"
#include "suns_model.h"
#include "suns_parser.h"

    extern suns_parser_state_t _sps;

    suns_parser_state_t *sps = &_sps;

    int line_no = 1;
    char yyerror_buf[BUFFER_SIZE];

    void yyerror(const char *str)
    {
        error("line %d: %s\n", line_no, str);
    }
    
    int yywrap()
    {
        return 1;
    }

%}

%token OBRACE EBRACE OPAREN EPAREN EOL COLON SEMICOLON COMMENT DOT EQUAL;
%token MODELTOK NAMETOK DIDTOK DPTOK LENTOK ENUMTOK BFTOK;
%token DEFINETOK COMMENTTOK DATATOK;
%token CHARS

%union 
{
    double number_f;
    int64_t number_i;
    uint64_t number_u;
    char *string;
    struct suns_type_pair *type_pair;
    struct suns_dp *dp;
    struct list *list;
    struct buffer *buffer;
    struct suns_model *model;
    struct suns_data *data;
    struct suns_value *value;
    struct suns_model_did *did;
    struct named_list *named_list;
    struct suns_define_block *define_block;
    struct suns_define *define;
    struct suns_dp_block *datapoints_block;
    struct suns_data_block *data_block;
    struct suns_attribute *attribute;
}

%token <number_u> UINT
%token <number_i> INT
%token <number_f> FLOAT
%token <string> STRING
%token <string> NAME
%token INT16TOK UINT16TOK INT32TOK UINT32TOK INT64TOK UINT64TOK;
%token FLOAT32TOK FLOAT64TOK ENUM16TOK BITFIELD16TOK BITFIELD32TOK;
%token SFTOK STRINGTOK;

%type <type_pair> suns_type;
%type <dp> suns_dp;
%type <list> suns_dp_list defines attributes;
%type <model> model_elmts model_block;
%type <number_u> len;
%type <string> name;
%type <buffer> register_list;
%type <value> suns_value;
%type <did> did;
%type <define_block> define_block;
%type <define> define;
%type <datapoints_block> datapoints_block;
%type <attribute> attribute;
%type <data_block> data_block;

/* %destructor { list_free($$, free); } config; */
/* %destructor { free($$); } suns_type suns_type_with_name 
   suns_type_with_number suns_type_simple; */

%%

config: blocks;

blocks: /* empty */
      | blocks model_block
{
    /* add the model to the global model list */
    list_node_add(sps->model_list, list_node_new($2));
}
      | blocks data_block
{    
    /* add the data block to the global list */
    debug_s($2->name);
    list_node_add(sps->data_block_list, list_node_new($2));
}
      | blocks define_block
{
    /* add the define block to the global list */
    list_node_add(sps->define_list, list_node_new($2));
    debug_s($2->name);
}


did: DIDTOK UINT STRING
{
    $$ = suns_model_did_new($2);
    $$->name = $3;
    
}
      | DIDTOK UINT
{
    $$ = suns_model_did_new($2);
}

len: LENTOK UINT
{
    $$ = $2;
}

name: NAMETOK STRING
{
    $$ = $2;
}

model_block: MODELTOK NAME OBRACE model_elmts EBRACE
{
    /* model_blocks are the same struct as model_elmts */
    $$ = $4;
    $$->type = $2;
}

model_elmts: /* empty */
{
    $$ = suns_model_new();
}
           | model_elmts datapoints_block
{
    list_node_add($$->dp_blocks, list_node_new($2));
}
           | model_elmts did
{
    $2->model = $$;
    /* add the did to the global did_list */
    list_node_add(sps->did_list, list_node_new($2));
    /* add the did to the list in the model */
    list_node_add($$->did_list, list_node_new($2));
    /* use model name if the did has no name assigned to it */
    /* this will be NULL if the name is defined after the did,
       or there is no name token */
    if ($2->name == NULL)
        $2->name = $$->name;
}
           | model_elmts len
{
    $$->len = $2;
}
           | model_elmts name
{
    $$->name = $2;
}
           | model_elmts define_block
{
    list_node_add($$->defines, list_node_new($2));
}


datapoints_block: DPTOK OBRACE suns_dp_list EBRACE
{
    $$ = suns_dp_block_new();
    $$->feature = NULL;
    $$->repeating = 0;
    $$->dp_list = $3;
}
                | DPTOK NAME OBRACE suns_dp_list EBRACE
{
    $$ = suns_dp_block_new();
    /* FIXME: abstract this operation so we're not doing
       strcmp() here in the parser */
    if (strcmp($2, "repeating") == 0) {
        $$->repeating = 1;
    }
    $$->feature = $2;
    $$->dp_list = $4;
}

suns_dp_list: /* empty */
{
    /* create a new empty list */
    $$ = list_new();
}
| suns_dp_list suns_dp
{
    /* add the data point to the list */
    list_node_add($$, list_node_new($2));
};

suns_dp: NAME OBRACE UINT suns_type attributes EBRACE
{
    $$ = suns_dp_new();
    $$->name = strdup($1);
    $$->offset = $3;
    $$->type_pair = $4;
    $$->attributes = $5;
}
       | NAME OBRACE suns_type attributes EBRACE
{
    $$ = suns_dp_new();
    $$->name = strdup($1);
    $$->offset = 0;
    $$->type_pair = $3;
    $$->attributes = $4;
};

suns_type: NAME DOT NAME
{
    /* type with name subscript */
    $$ = suns_type_pair_new();
    $$->type = suns_type_from_name($1);
    if ($$->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    $$->name = $3;
}
         | NAME DOT INT
{
    /* type with int subscript - could be string or numeric type */
    $$ = suns_type_pair_new();
    $$->type = suns_type_from_name($1);
    if ($$->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    switch ($$->type) {
    case SUNS_STRING:
        $$->len = $3;
        break;
    default:
        $$->sf  = $3;
    }
}
         | NAME DOT UINT
{
    /* type with uint subscript - could be string or numeric type */
    $$ = suns_type_pair_new();
    $$->type = suns_type_from_name($1);
    if ($$->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    switch ($$->type) {
    case SUNS_STRING:
        $$->len = $3;
        break;
    default:
        $$->sf  = $3;
    }
}
         | NAME
{
    /* simple type with no subscript */
    $$ = suns_type_pair_new();
    $$->type = suns_type_from_name($1);
    if ($$->type == SUNS_UNDEF) {
        yyerror("invalid suns type");
        YYERROR;
    }
    $$->name = NULL;
}

attributes: /* empty */
{
    $$ = list_new();
}
          | attributes attribute
{
    list_node_add($$, list_node_new($2));
}

attribute: NAME EQUAL STRING
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->value = strdup($3);
    $$->list = NULL;
}
         | NAME
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->value = NULL;
}
         | NAME EQUAL NAME
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->value = strdup($3);
}
         | NAME EQUAL UINT
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->value = malloc(32);
    snprintf($$->value, 32, "%lld", $3);
}
         | NAME EQUAL INT
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->value = malloc(32);
    snprintf($$->value, 32, "%lld", $3);
}
         | NAME EQUAL OBRACE attributes EBRACE /* nested attributes */
{
    $$ = suns_attribute_new();
    $$->name = strdup($1);
    $$->list = $4;
}
         | UINT
{
    $$ = suns_attribute_new();
    /* malloc() b/c we need a buffer to snprintf() the integer into */
    $$->name = malloc(32);
    snprintf($$->name, 32, "%lld", $1);
    $$->value = NULL;
}
         | INT
{
    $$ = suns_attribute_new();
    /* malloc() b/c we need a buffer to snprintf() the integer into */
    $$->name = malloc(32);
    snprintf($$->name, 32, "%lld", $1);
    $$->value = NULL;
}



/* any string representation of a suns_value_t
   this is a number or string optionally accompanied by a type_pair */
suns_value: UINT
{
    /* debug("UINT %llu", $1); */
    $$ = suns_value_new();
    suns_value_set_uint16($$, $1);
}
    | INT
{
    /* debug("INT %lld", $1); */
    $$ = suns_value_new();
    suns_value_set_int16($$, $1);
}
    | FLOAT
{
    /* debug("FLOAT %f", $1); */
    $$ = suns_value_new();
    suns_value_set_float32($$, $1);
}
/* the repetition of the same code below is kind of gross
   however, since there are three numeric types the parser
   can recognize this is necessary unless i created another union
   type and the associated supporting machinery.
   
   this is a case where a more powerful language would be helpful.
*/
    | suns_type COLON INT
{
    $$ = suns_value_new();
    switch ($1->type) {
    case SUNS_INT16:
        suns_value_set_int16($$, $3);
        break;
    case SUNS_UINT16:
        suns_value_set_uint16($$, $3);
        break;
    case SUNS_INT32:
        suns_value_set_int32($$, $3);
        break;
    case SUNS_UINT32:
        suns_value_set_uint32($$, $3);
        break;
    case SUNS_FLOAT32:
        suns_value_set_float32($$, $3);
        break;
    case SUNS_ENUM16:
        suns_value_set_enum16($$, $3);
        break;
    case SUNS_BITFIELD16:
        suns_value_set_bitfield16($$, $3);
        break;
    case SUNS_BITFIELD32:
        suns_value_set_bitfield32($$, $3);
        break;
    case SUNS_INT64:
        suns_value_set_int64($$, $3);
        break;
    case SUNS_UINT64:
        suns_value_set_uint64($$, $3);
        break;
    case SUNS_FLOAT64:
        suns_value_set_float64($$, $3);
        break;
    case SUNS_SF:
        suns_value_set_sunssf($$, $3);
        break;
    default:
        snprintf(yyerror_buf, BUFFER_SIZE,
                 "unsupported type with int literal: %s",
                 suns_type_string($1->type));
        yyerror(yyerror_buf);
    }
}
    | suns_type COLON UINT
{
    $$ = suns_value_new();
    switch ($1->type) {
    case SUNS_INT16:
        suns_value_set_int16($$, $3);
        break;
    case SUNS_INT32:
        suns_value_set_int32($$, $3);
        break;
    case SUNS_UINT16:
        suns_value_set_uint16($$, $3);
        break;
    case SUNS_UINT32:
        suns_value_set_uint32($$, $3);
        break;
    case SUNS_FLOAT32:
        suns_value_set_float32($$, $3);
        break;
    case SUNS_ENUM16:
        suns_value_set_enum16($$, $3);
        break;
    case SUNS_BITFIELD16:
        suns_value_set_bitfield16($$, $3);
        break;
    case SUNS_BITFIELD32:
        suns_value_set_bitfield32($$, $3);
        break;
    case SUNS_INT64:
        suns_value_set_int64($$, $3);
        break;
    case SUNS_UINT64:
        suns_value_set_uint64($$, $3);
        break;
    case SUNS_FLOAT64:
        suns_value_set_float64($$, $3);
        break;
    case SUNS_SF:
        suns_value_set_sunssf($$, $3);
        break;
    default:
        snprintf(yyerror_buf, BUFFER_SIZE,
                 "unsupported type with uint literal: %s",
                 suns_type_string($1->type));
        yyerror(yyerror_buf);
    }
}
    | suns_type COLON FLOAT
{
    $$ = suns_value_new();
    switch ($1->type) {
    case SUNS_FLOAT32:
        suns_value_set_float32($$, $3);
        break;
    default:
        yyerror("can only use float32 with a float literal");
    }
}
    | suns_type COLON STRING
{
    $$ = suns_value_new();
    if ($1->type != SUNS_STRING)
        yyerror("string literal can only be declared as a string value");
    suns_value_set_string($$, $3, $1->len);
}
    

register_list: /* empty */
{
    $$ = buffer_new(BUFFER_SIZE);
}
    | register_list suns_value 
{
    /* debug("suns_value type = %s(%d)",
       suns_type_string($2->tp.type), $2->tp.type); */
    /* debug("buffer_space($$) = %d, suns_type_size($2->tp.type) = %d", buffer_space($$), suns_type_size($2->tp.type)); */
    if (buffer_space($$) < suns_type_pair_size(&($2->tp))) {
        /* debug("resizing buffer!"); */
        buffer_resize($$, buffer_len($$) + BUFFER_SIZE);
    }
    suns_value_to_buffer($$, $2);
};


data_block: DATATOK NAME OBRACE register_list EBRACE
{
    $$ = suns_data_block_new();
    $$->name = strdup($2);
    $$->data = $4;
}


define_block: DEFINETOK NAME OBRACE defines EBRACE
{
    $$ = suns_define_block_new();
    $$->name = $2;
    $$->list = $4;
}


define: NAME OBRACE INT STRING EBRACE
{
    $$ = suns_define_new();
    $$->name = $1;
    $$->value = $3;
    $$->string = $4;
} 
      | NAME OBRACE UINT STRING EBRACE
{
    $$ = suns_define_new();
    $$->name = $1;
    $$->value = $3;
    $$->string = $4;
}
      | NAME OBRACE STRING EBRACE
{
    $$ = suns_define_new();
    $$->name = $1;
    $$->value = 0;
    $$->string = $3;
}
      | NAME OBRACE attributes EBRACE
{
    $$ = suns_define_new();
    $$->name = $1;
    $$->value = 0;
    $$->attributes = $3;
}

defines: /* empty */
{
    $$ = list_new();
}
       | defines define
{
    list_node_add($$, list_node_new($2));
}



  
%%
