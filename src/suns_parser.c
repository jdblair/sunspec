/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_parser.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>

#include "ezxml/ezxml.h"

#include "trx/macros.h"
#include "trx/debug.h"
#include "suns_model.h"
#include "suns_parser.h"
#include "suns_lang.tab.h"
#include "suns_output.h"


/* global parser state

   we use a global pointer here so bison can access it
   switching to the reentrant version of bison would allow us to
   eliminate this global variable

   accessor functions for the important elements of the parser state.
   other code modules should use these accessors. */

suns_parser_state_t _sps;

/* global *yyin required by bison/yacc */
extern FILE *yyin;

/* yyparse() prototype required by bison/yacc */
int yyparse (void);


suns_parser_state_t *suns_get_parser_state(void)
{
    return &_sps;
}

/* accessor for global model_list */
list_t *suns_get_model_list(void)
{
    return _sps.model_list;
}

/* accessor for global did_list */
list_t *suns_get_did_list(void)
{
    return _sps.did_list;

}

/* accessor for global data_block_list */
list_t *suns_get_data_block_list(void)
{
    return _sps.data_block_list;
}


/* accessor for global define_block_list */
list_t *suns_get_define_list(void)
{
    return _sps.define_list;
}


/* initialize the sunspec parser
   b/c we use the original bison interface we rely on globals
   and we're not re-entrant. */
void suns_parser_init(void)
{
    bzero(&_sps, sizeof(suns_parser_state_t));

    _sps.model_list = list_new();
    _sps.did_list = list_new();
    _sps.data_block_list = list_new();
    _sps.define_list = list_new();
}


/* linear search of a dp_list looking for *name */
suns_dp_t * suns_model_find_dp_by_name(list_t *list, char *name)
{
    list_node_t *c;
    list_for_each(list, c) {
        suns_dp_t *dp = c->data;
        if (strcmp(dp->name, name) == 0)
            return dp;
    }

    /* not found */
    return NULL;
}


/* linear search of a model's dp_block_list looking for *name */
suns_dp_t *suns_dp_find_in_model(suns_model_t *m, char *name)
{
    list_node_t *c;
    list_for_each(m->dp_blocks, c) {
        suns_dp_block_t *block = c->data;
        suns_dp_t *dp = suns_model_find_dp_by_name(block->dp_list, name);
        if (dp)
            return dp;
    }

    /* not found */
    return NULL;
}


/* opens a model file and parses it with yyparse()
   
   this use of bison is not reentrant!!
   it would make sense to get rid of the globals and use bison's
   reentrant API
*/
int suns_parse_model_file(const char *file)
{
    FILE *f;

    debug_s(file);

    f = fopen(file, "r");
    if (f == NULL) {
        debug("cannot open model file %s", file);
        return -1;
    }
    
    /* set up the yacc/bison globals and call yyparse */
    yyin = f;
    yyparse();

    /* close the file */
    fclose(yyin);

    return 0;
}


/*******************************************************/

/**
 * parse the <model> element and return a suns_model_did_t
 * 
 * in the xml representation there is a 1 to 1 mapping between
 * did and model, so it is safe to return the suns_model_did_t
 * to represent both the did and the model
 */
suns_model_did_t *suns_parse_xml_model_elmt(ezxml_t model)
{
    int did_int;
    
    /* parse model id (did) */
    if (sscanf(ezxml_attr(model, "id"), "%d", (int*) &did_int) != 1) {
        error("can't convert id \"%s\" to an integer",
              ezxml_attr(model, "id"));
        return NULL;
    }

    /* create a suns_model_did_t and hang it on the global list */
    suns_model_did_t *did = suns_model_did_new(did_int);
    did->model = suns_model_new();
    did->model->type = "suns"; 
    list_node_add(did->model->did_list, list_node_new(did));

    /* now parse the blocks */
    ezxml_t block;
    for (block = ezxml_child(model, "block"); block; block = block->next) {
        suns_dp_block_t *b = suns_ezxml_to_dp_block(block);
        list_node_add(did->model->dp_blocks, list_node_new(b));
    }

    return did;
}


/**
 * parse the <strings> element in a model file
 *
 * this returns the suns_model_t that the strings are part of.
 * note that it does not allocate the suns_model_t; it must have already
 * been created and attached to the global model list, presumably by
 * suns_parse_xml_model_elmt().
 */ 
suns_model_did_t *suns_parse_xml_strings_elmt(ezxml_t strings,
                                              list_t *did_list)
{
    int did_int;

    /* parse id */
    if (sscanf(ezxml_attr(strings, "id"), "%d", (int*) &did_int) != 1) {
        error("can't convert id \"%s\" to an integer",
              ezxml_attr(strings, "id"));
        return NULL;
    }

    debug("parsing strings for model %d", did_int);
    
    /* only load "en" locale */
    const char *locale = ezxml_attr(strings, "locale");
    if (strcmp(locale, "en") != 0) {
        debug("ignoring strings of locale %s", locale);
        return NULL;
    }

    /* look up model */
    suns_model_did_t *did = suns_find_did(did_list, did_int);
    if (did == NULL) {
        /* skip this strings block - we haven't
           yet parsed the model it refers to */
        return NULL;
    }

    /* parse model element */
    ezxml_t model = ezxml_child(strings, "model");
    if (model == NULL) {
        /* model element is required */
        error("no <model> element in <strings id=\"%d\">", did_int);
        return NULL;
    }

    /* parse elements in <model> */
    ezxml_t label = ezxml_child(model, "label");
    ezxml_t desc = ezxml_child(model, "description");
    /* ezxml_t notes = ezxml_child(model, "notes"); */
    
    /* ezxml_txt will return an empty string if the ezxml_t is null */
    did->name = strdup(ezxml_txt(label));
    did->model->name = strdup(ezxml_txt(label));
    did->model->comment = strdup(ezxml_txt(desc));

    /* parse point elements */
    ezxml_t point;
    for (point = ezxml_child(strings, "point");
         point; point = point->next) {
        char *name = strdup(ezxml_attr(point, "id"));
        if (! name) {
            error("no id attribute for strings.point element");
            return NULL;
        }
        verbose(4, "parsing strings for point %s", name);

        /* look up the suns_dp_t in the model */
        suns_dp_t *dp = suns_dp_find_in_model(did->model, name);
        if (dp == NULL) {
            debug("can't find datapoint id %s", name);
            continue;
        }

        /* make sure attributes list is created */
        if (dp->attributes == NULL)
            dp->attributes = list_new();

        ezxml_t label = ezxml_child(point, "label");
        ezxml_t desc = ezxml_child(point, "description");
        ezxml_t notes = ezxml_child(point, "notes");

        suns_attribute_t *a;    /* pointer is reused for all attributes */

        if (label) {
            a = suns_attribute_new();
            a->name = "label";
            a->value = strdup(ezxml_txt(label));
            list_node_add(dp->attributes, list_node_new(a));
        }
            
        if (desc) {
            a = suns_attribute_new();
            a->name = "description";
            a->value = strdup(ezxml_txt(desc));
            list_node_add(dp->attributes, list_node_new(a));
        }

        if (notes) {
            a = suns_attribute_new();
            a->name = "notes";
            a->value = strdup(ezxml_txt(notes));
            list_node_add(dp->attributes, list_node_new(a));
        }
    }

    return did;
}


int suns_parse_xml_model_file(const char *file)
{
    int rc = 0;
    suns_parser_state_t *sps = suns_get_parser_state();
    
    ezxml_t model;
    ezxml_t x = ezxml_parse_file(file);
    if (! x) {
        error("ezxml returned NULL pointer");
        return -1;
    }
    
    for (model = ezxml_child(x, "model"); model; model = model->next) {
        suns_model_did_t *did = suns_parse_xml_model_elmt(model);
        if (did) {
            /* models and did are 1:1 in the xml format */
            list_node_add(sps->did_list, list_node_new(did));
            list_node_add(sps->model_list, list_node_new(did->model));
        }
    }

    ezxml_t strings;
    for (strings = ezxml_child(x, "strings");
         strings; strings = strings->next) {
        /* parse strings element */
        suns_parse_xml_strings_elmt(strings, sps->did_list);
    }

    /* free the ezxml structure */
    ezxml_free(x);
    
    return rc;
}


/* parse <model> element into a suns_dp_block_t */
suns_dp_block_t *suns_ezxml_to_dp_block(ezxml_t b)
{
    suns_dp_block_t *dp_block = suns_dp_block_new();
    dp_block->dp_list = list_new();
    const char *type =  ezxml_attr(b, "type");
    if (type &&
        (strcasecmp("repeating", type) == 0))
        dp_block->repeating = 1;

    ezxml_t p;
    for (p = ezxml_child(b, "point"); p; p = p->next) {
        suns_dp_t *dp = suns_ezxml_to_dp(p);
        list_node_add(dp_block->dp_list, list_node_new(dp));
    }

    return dp_block;
}


/* parse a <point> element into a suns_dp_t */
suns_dp_t *suns_ezxml_to_dp(ezxml_t p)
{
    suns_dp_t *dp = suns_dp_new();
    if (! dp)
        return NULL;
    
    /* discard const; its ok, dp->name should be a const char * */
    dp->name = strdup(ezxml_attr(p, "id"));
    dp->type_pair = suns_type_pair_new();
    dp->type_pair->type = suns_type_from_name((char *) ezxml_attr(p, "type"));
    if (dp->type_pair->type == SUNS_UNDEF) {
        error("datapoint id=\"%s\" has invalid data type \"%s\"",
              dp->name, ezxml_attr(p, "type"));
    }

    /* parse out the string length */
    if (dp->type_pair->type == SUNS_STRING) {
        int size;
        if (sscanf(ezxml_attr(p, "len"), "%d", &size) != 1) {
            error("can't parse size argument to point %s", dp->name);
        }
        /* size attribute is modbus registers */
        dp->type_pair->len = size * 2;
    }

    /* parse the scale factor - can be an integer, or the name of a point */
    char *sf = (char *) ezxml_attr(p, "sf");
    if (sf) {
        /* see if it is numeric */
        if (sscanf(sf, "%d", &(dp->type_pair->sf)) != 1) {
            /* its not numeric, treat it as a string */
            dp->type_pair->name = strdup(sf);
        }
    }

    char *u = (char *) ezxml_attr(p, "units");
    if (u) {
        if (! dp->attributes)
            dp->attributes = list_new();
        suns_attribute_t *a = malloc(sizeof(suns_attribute_t));
        a->name = "u";
        a->value = strdup(u);
        list_node_add(dp->attributes, list_node_new(a));
    }

    /* debug("found type %s", ezxml_attr(p, "type"));
       suns_type_pair_fprint(stdout, dp->type_pair); */

    return dp;
}
