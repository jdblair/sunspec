/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_parser.c
 * $Id: $
 *
 * mostly glue code to interact with yacc
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
#include <strings.h>
#include <unistd.h>
#include <getopt.h>

#include "trx/macros.h"
#include "trx/debug.h"
#include "suns_model.h"
#include "suns_parser.h"
#include "suns_lang.tab.h"



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


/* initialize the sunspec parser
   b/c we use the original bison interface we rely on globals
   and we're not re-entrant. */
void suns_parser_init(void)
{
    bzero(&_sps, sizeof(suns_parser_state_t));

    _sps.model_list = list_new();
    _sps.did_list = list_new();
    _sps.data_block_list = list_new();

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


/* opens a model file and parses it with yyparse()
   
   this use of bison is not reentrant!!
   it would make sense to get rid of the globals and use bison's
   reentrant API
*/
int suns_parse_model_file(const char *file)
{
    FILE *f;

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

