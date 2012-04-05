/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * debug.h
 * $Id: debug.h 58 2010-09-25 16:07:06Z jdblair $
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

#ifndef _DEBUG_H_

#include <stdio.h>

#ifndef _verbose_level_
extern int verbose_level;
#define _verbose_level_ 1
#endif /* _verbose_level_ */

#ifndef debug
#ifdef DEBUG

#define debug(format, arg...) \
    if (verbose_level > 2) {                            \
        fprintf(stderr, "%s:%d (in %s): " format "\n",  \
                __BASE_FILE__, __LINE__, __FUNCTION__, ## arg); \
    }

#define debug_dump_buffer(buffer, len) \
    if (verbose_level > 2) { \
        fprintf(stderr, "%s:%d (in %s): " #buffer "[] = ",  \
                __BASE_FILE__, __LINE__, __FUNCTION__); \
        dump_buffer(stderr, buffer, len); \
}

#else /* DEBUG */

/* no-op */
#define debug(format, arg...)
#define debug_dump_buffer(buffer, len)

#endif /* DEBUG */

/* convenience functions for printing variables */
/* these will be no-op of DEBUG is not defined */
#define debug_i(val) debug(#val " = %d", val)
#define debug_li(val) debug(#val " = %ld", val)
#define debug_x(val) debug(#val " = %x", val)
#define debug_p(val) debug(#val " = %p", val)
#define debug_f(val) debug(#val " = %f", val)
#define debug_c(val) debug(#val " = '%c'", val)
#define debug_s(val) debug(#val " = \"%s\"", val)

#endif /* debug */


/* define assert() macro
   this is used instead of the one included in libc b/c i don't want to
   abort() whenever an assertion fails */

#ifdef ASSERT

void assertion_failure(char *_exp, char *file, char *base_file, int line);

#define assert(exp) if (exp); \
    else assertion_failure( #exp, __FILE__, __BASE_FILE__, __LINE__ )

/* assert_type is a convenience macro that tests the type member of the
   specified structure pointer */
#define assert_type(ptr, exp) assert(ptr != NULL); assert(ptr->type == exp)

#else /* ASSERT */

/* define assert() and assert_type() to no-op */

#define assert(exp)

#define assert_type(devptr, exp)

#endif /* ASSERT */

/* remaining prototypes */
void dump_buffer(FILE *fp, unsigned char *buffer, size_t len);
void timestamp_dump_buffer(FILE *fp, char *comment,
                           unsigned char *buffer, size_t len);
void debug_timestamp(FILE *fp);
void debug_mode(void);

#endif /* _DEBUG_H_ */
