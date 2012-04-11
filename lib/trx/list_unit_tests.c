/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * unit_tests.c
 *
 * trx unit tests
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

#define _BSD_SOURCE  /* for big/little endian macros */

#define UNIT_ASSERT(exp) if (! (exp)) {			\
	debug("ASSERT FAILED: " #exp);			\
	return -1;					\
    }

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <endian.h>
#include <getopt.h>
#include <time.h>

#include "debug.h"
#include "macros.h"
#include "list.h"

/* unit test function prototype */
typedef int (*unit_test_f)(const char **name);

/* unit test prototypes */
int unit_test_make_list(const char **name);
int unit_test_insertion_sort(const char **name);
int unit_test_insert(const char **name);
int unit_test_quicksort(const char **name);

int test_getopt(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
        case 'v':
            verbose_level++;
            break;
        
        default:
            printf("Usage: %s [-vvv]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}


char *list_test_data[] = {
                        "alpha",
                        "beta",
                        "delta",
                        "gamma",
                        "epsilon",
                        "zeta",
                        "eta",
                        "theta",
                        "iota",
                        "kappa",
                        "lambda",
                        "mu",
                        "nu",
                        "xi",
                        "omicron",
                        "pi",
                        "rho",
                        "sigma",
                        "tau",
                        "upsilon",
                        "phi",
                        "chi",
                        "psi",
                        "omega",
                        NULL
                        };

char *list_test_data_sorted[] = {
                        "alpha",
                        "beta",
                        "chi",
                        "delta",
                        "epsilon",
                        "eta",
                        "gamma",
                        "iota",
                        "kappa",
                        "lambda",
                        "mu",
                        "nu",
                        "omega",
                        "omicron",
                        "phi",
                        "pi",
                        "psi",
                        "rho",
                        "sigma",
                        "tau",
                        "theta",
                        "upsilon",
                        "xi",
                        "zeta",
                        NULL
                        };


int main (int argc, char *argv[])
{
    int i;
    int pass = 0;
    int fail = 0;
    const char *test_name;

    test_getopt(argc, argv);

    unit_test_f test_list[] = {
        unit_test_make_list,
        unit_test_insert,
        unit_test_insertion_sort,
        /* unit_test_quicksort, */
        NULL,
    };

    for (i = 0; test_list[i] != NULL; i++) {
        if (test_list[i](&test_name) < 0) {
            fail++;
            printf("FAIL:  ");
        } else {
            pass++;
            printf("pass:  ");
        }
        printf("%s\n", test_name);
    }

    printf("------------------\n");
    printf("total passed: %d\n", pass);
    printf("total failed: %d\n", fail);

    exit(fail);
}


int unit_test_make_list(const char **name)
{
    *name = __FUNCTION__;
    list_t *list = list_new();

    int i = 0;
    for (i = 0; list_test_data[i] != NULL; i++)
        list_node_add(list, list_node_new(list_test_data[i]));

    list_node_t *c;
    i = 0;
    list_for_each(list, c) {
        UNIT_ASSERT(strcmp(list_test_data[i], c->data) == 0);
        i++;
    }

    list_free(list, NULL);

    return 1;
}


int unit_test_insert(const char **name)
{
    *name = __FUNCTION__;

    list_t *list = list_new();

    char *names[] = {
        "george",
        "john",
        "paul",
        "ringo" };

    list_node_add(list, list_node_new(names[1]));
    list_node_add(list, list_node_new(names[2]));

    /* insert before first node */
    list_node_insert_before(list, list_head(list), list_node_new(names[0]));

    /* insert after last node */
    list_node_insert_after(list, list_tail(list), list_node_new(names[3]));

    /* now check that they are all in the expected order */
    list_node_t *c;
    int i = 0;
    list_for_each(list, c) {
        debug_s((char*)c->data);
        UNIT_ASSERT(strcmp(names[i], c->data) == 0);
        i++;
    }

    list_free(list, NULL);

    return 1;
}
    

int _strcmp_node(list_node_t *left, list_node_t *right)
{
    return strcmp((char*) left->data, (char*) right->data);
}


int unit_test_insertion_sort(const char **name)
{
    *name = __FUNCTION__;
    list_t *list = list_new();
    list_node_t *c;

    int i = 0;
    for (i = 0; list_test_data_sorted[i] != NULL; i++)
        list_node_add(list, list_node_new(list_test_data_sorted[i]));

    list_shuffle(list, rand);
    debug_i(list_count(list));
    
    list_for_each(list, c) {
        debug_s((char*)c->data);
    }

    list_insertion_sort(list, _strcmp_node);

    c = list_head(list);
    for (i = 0; list_test_data_sorted[i]; i++) {
        UNIT_ASSERT(strcmp(list_test_data_sorted[i], c->data) == 0);
        c = c->next;
    }

    list_for_each(list, c) {
        debug_s((char*)c->data);
    }

    list_free(list, NULL);

    return 1;
}



int unit_test_quicksort(const char **name)
{
    *name = __FUNCTION__;
    list_t *list = list_new();

    int i = 0;
    for (i = 0; list_test_data_sorted[i] != NULL; i++)
        list_node_add(list, list_node_new(list_test_data[i]));
    
    list_shuffle(list, rand);

    debug_i(list_count(list));

    list_quicksort(list, _strcmp_node);

    list_node_t *c = list_head(list);
    for (i = 0; list_test_data_sorted[i]; i++) {
        UNIT_ASSERT(strcmp(list_test_data_sorted[i], c->data) == 0);
        c = c->next;
    }

    list_for_each(list, c) {
        debug_s((char*)c->data);
    }

    list_free(list, NULL);

    return 1;
}


