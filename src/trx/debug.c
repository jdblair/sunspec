/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * debug.c
 * $Id: debug.c 58 2010-09-25 16:07:06Z jdblair $
 *
 * Useful assert() and debugging functions.
 * 
 * Copyright (c) 2007-2011, John D. Blair <jdb@moship.net>
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
#include <time.h>
#include <sys/time.h>

int verbose_level = 0;
/* now debug.h won't define 'extern int verbose_level */
#define _verbose_level_

#include "debug.h"

#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line)
{
    if (! strcmp(file, base_file)) {
	fprintf(stderr,
		"assert(%s) failed in file %s, line %d\n", exp, file, line);
    } else {
	fprintf(stderr,
		"assert(%s) failed in file %s (included from %s), line %d\n",
		exp, file, base_file, line);
    }

}
#endif /* ASSERT */

void dump_buffer(FILE *fp, unsigned char *buffer, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        fprintf(fp, "%02x ", buffer[i]);
    }
    fprintf(fp, "\n");
}


void timestamp_dump_buffer(FILE *fp, char *comment,
                           unsigned char *buffer, size_t len)
{
    debug_timestamp(fp);
    fprintf(fp, "%s[%02x] ", comment, len);
    dump_buffer(fp, buffer, len);
}

void debug_timestamp(FILE *fp) {
    struct tm *tm;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    fprintf(fp, "%02d:%02d:%02d.%06d ",
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            (int) tv.tv_usec);
}

void debug_mode(void)
{
#ifdef DEBUG
    fprintf(stderr, "debug ON\n");
#else
    fprintf(stderr, "debug OFF\n");
#endif
}
