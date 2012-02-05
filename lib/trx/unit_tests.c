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
#include "date.h"
#include "string.h"

/* unit test function prototype */
typedef int (*unit_test_f)(const char **name);

/* unit test prototypes */
int unit_test_date_parse_rfc3339_tm(const char **name);
int unit_test_string_parse_decimal(const char **name);


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


int main (int argc, char *argv[])
{
    int i;
    int pass = 0;
    int fail = 0;
    const char *test_name;

    test_getopt(argc, argv);

    unit_test_f test_list[] = {
        unit_test_date_parse_rfc3339_tm,
        unit_test_string_parse_decimal,
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


int unit_test_date_parse_rfc3339_tm(const char **name)
{
    *name = __FUNCTION__;

    typedef struct rfc3339_test {
        char *date_str;
        struct tm tm;
        int usec;
    } rfc3339_test_t;

    rfc3339_test_t pass_tests[] = {
        /* examples taken from rfc3339.txt */

        /* includes fractional second */
        { "1985-04-12T23:20:50.52Z",
          { .tm_sec =       50,
            .tm_min =       20,
            .tm_hour =      23,
            .tm_mday =      12,
            .tm_mon =        3,
            .tm_year =      85,
            .tm_gmtoff =     0,
          },
          520000,
        },

        /* set to pacific standard time */
        { "1996-12-19T16:39:57-08:00",
          { .tm_sec =       57,
            .tm_min =       39,
            .tm_hour =      16,
            .tm_mday =      19,
            .tm_mon =       11,
            .tm_year =      96,
            .tm_gmtoff = -28800,
          },
          0,
        },

        /* leap second at the end of 1990 */
        { "1990-12-31T23:59:60Z",
          { .tm_sec =       60,
            .tm_min =       59,
            .tm_hour =      23,
            .tm_mday =      31,
            .tm_mon =       11,
            .tm_year =      90,
            .tm_gmtoff =     0,
          },
          0,
        },

        /* the same leap second in pacific standard time */
        { "1990-12-31T15:59:60-08:00",
          { .tm_sec =       60,
            .tm_min =       59,
            .tm_hour =      15,
            .tm_mday =      31,
            .tm_mon =       11,
            .tm_year =      90,
            .tm_gmtoff = -28800,
          },
          0,
        },

        /* weird timezone in netherlands in 1937 (20 minutes off gmt) */
        { "1937-01-01T12:00:27.87+00:20",
          { .tm_sec =       27,
            .tm_min =        0,
            .tm_hour =      12,
            .tm_mday =       1,
            .tm_mon =        0,
            .tm_year =      37,
            .tm_gmtoff =  1200,
          },
          870000,
        },

        /* other tests not from rfc3339.txt examples */
        /* only provide date, no time */
        { "1973-04-03",
          { .tm_sec =        0,
            .tm_min =        0,
            .tm_hour =       0,
            .tm_mday =       3,
            .tm_mon =        3,
            .tm_year =      73,
            .tm_gmtoff =     0,
          },
          0,
        },

        /* lower-case t and z */
        { "1985-04-12t23:20:50.52z",
          { .tm_sec =       50,
            .tm_min =       20,
            .tm_hour =      23,
            .tm_mday =      12,
            .tm_mon =        3,
            .tm_year =      85,
            .tm_gmtoff =     0,
          },
          520000,
        },

        /* set to eastern standard time */
        { "1981-04-12T07:00:03-05:00",
          { .tm_sec =       03,
            .tm_min =       00,
            .tm_hour =      07,
            .tm_mday =      12,
            .tm_mon =       03,
            .tm_year =      81,
            .tm_gmtoff = -18000,
          },
          0,
        },

        /* end marker */
        { NULL },
    };


    char *fail_tests[] = {
        /* field length too long */
        "1985-0004-12T23:20:50.52Z",
        /* malformed time */
        "1985-04-12T23:20",
        /* no T separator */
        "1990-12-31 15:59:60-08:00",
        /* malformed year */
        "199089-31-11T15:59:60-08:00",
        /* up to no good */
        "1990-31T1500000000000000000;drop table people;:59:60-08:00",
        /* bogus data */
        "bogus!",
        /* buffer overrun */
        "0123456789112345678921234567893123456789412345678951234567896123456789712345678981234567899123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789k123456789l123456789m123456789n123456789o123456789p123456789q123456789r123456789s123456789t123456789u123456789v123456789w123456789x123456789y123456789z123456789",
        /* end marker */
        NULL
    };

    int i;

    /* check pass cases */
    for (i = 0; pass_tests[i].date_str != NULL; i++) {
        struct tm tm;
        int usec;

        /* zero out the data structures */
        memset(&tm, 0, sizeof(struct tm));
        usec = 0;

        if (verbose_level > 0)
            printf("test date string: %s\n", pass_tests[i].date_str);

        UNIT_ASSERT(date_parse_rfc3339_tm(pass_tests[i].date_str,
                                          &tm, &usec) == 0);
        UNIT_ASSERT(tm.tm_sec == pass_tests[i].tm.tm_sec);
        UNIT_ASSERT(tm.tm_min == pass_tests[i].tm.tm_min);
        UNIT_ASSERT(tm.tm_hour == pass_tests[i].tm.tm_hour);
        UNIT_ASSERT(tm.tm_mday == pass_tests[i].tm.tm_mday);
        UNIT_ASSERT(tm.tm_mon == pass_tests[i].tm.tm_mon);
        UNIT_ASSERT(tm.tm_year == pass_tests[i].tm.tm_year);
        UNIT_ASSERT(tm.tm_gmtoff == pass_tests[i].tm.tm_gmtoff);
        UNIT_ASSERT(usec == pass_tests[i].usec);
    }

    /* check fail cases */
    for (i = 0; fail_tests[i] != NULL; i++) {
        struct tm tm;
        int usec;

        /* zero out the data structures */
        memset(&tm, 0, sizeof(struct tm));
        usec = 0;

        if (verbose_level > 0)
            printf("negative test date string: %s\n", fail_tests[i]);

        UNIT_ASSERT(date_parse_rfc3339_tm(fail_tests[i], &tm, &usec) < 0);
    }

    return 1;
}


int unit_test_string_parse_decimal(const char **name)
{
    *name = __FUNCTION__;

    typedef struct parse_decimal_test {
        char *name;
        char *string;
        long int base;
        long int e;
    } parse_decimal_test_t;

    parse_decimal_test_t pass_tests[] = {
        { "simple decimal 1", "3.14159", 314159, -5 },
        { "simple decimal 2", "42.9", 429, -1 },
        { "simple decimal 3", "43521.6908", 435216908, -4 },
        { "simple decimal 4", "-43521.6908", -435216908, -4 },
        { "scientific notation 1", "4.12345e10", 412345, 5 },
        { "scientific notation 2", "4.12345e-10", 412345, -15 },
        { "scientific notation 3", "-4.12345e10", -412345, 5 },
        { "scientific notation, capital E", "4.12345E10", 412345, 5 },
        { "large integer", "12345678", 12345678, 0 },
        { "preceeding zero", "00123456789", 123456789, 0 },
        { NULL, 0, 0 }
    };

    char *fail_tests[] = {
        "bogus",
        "012345678911234567892123456789312345678941234567895123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567897123456789812345678991234567890123456789112345678921234567893123456789412345678951234567896123456789712345678981234567899123456789",
        "123.13.54",
        "1.242e13.4",
        NULL,
    };

    int i;
    for (i = 0; pass_tests[i].string != NULL; i++) {
        long int base = 0;
        int e = 0;

        debug("test %s: %s", pass_tests[i].name, pass_tests[i].string);
        UNIT_ASSERT(string_parse_decimal_long(pass_tests[i].string,
                                              &base, &e) == 0);

        debug("base = %ld, e = %d", base, e);

        UNIT_ASSERT(base == pass_tests[i].base);
        UNIT_ASSERT(e == pass_tests[i].e);
    }

    for (i = 0; fail_tests[i] != NULL; i++) {
        long int base = 0;
        int e = 0;

        debug("fail_tests[%d] = %s", i, fail_tests[i]);

        UNIT_ASSERT(string_parse_decimal_long(fail_tests[i], &base, &e) < 0);
    }

    return 1;
}

