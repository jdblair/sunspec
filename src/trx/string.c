/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * string.c
 * $Id: $
 *
 * String processing utility functions.
 *
 *
 * Copyright (c) 2010-2011, John D. Blair <jdb@moship.net>
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
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Copy a null-terminated string *in into *out, escaping single-quotes,
 * double-quotes and backslashes.  This was originally written to support
 * US-ASCII strings used in sqlite3 INSERT statements.
 *
 * As with snprintf(), if len is exceeded, the number of bytes which *would*
 * have been printed is returned.
 *
 * Macros are provided for common schemes:
 *    string_escape_cstyle(in, out, out_len)
 *    string_escape_sql(in, out, out_len)
 *
 */
int string_escape(char *in, char *out, size_t out_len,
                  char *esc_chars, char esc)
{
    int in_i = 0;
    int out_i = 0;

    /* copy from in into out, escaping characters in esc_chars with esc

       note that we test out_i, but we increment in_i!!

       we leave an extra byte in out_len for the null byte
       if we have to truncate the string */

    for (in_i = 0; out_i < out_len - 2; in_i++) {
        /* check for terminating null byte */
        if (in[in_i] == '\0') {
            out[out_i] = '\0';
            /* do not include terminating null byte in count */
            return out_i;
        }

        int i;
        for (i = 0; esc_chars[i] != '\0'; i++) {
            if (in[in_i] == esc_chars[i]) {
                out[out_i++] = esc;
                out[out_i++] = in[in_i];
            } else {
                out[out_i++] = in[in_i];
            }
        }
    }

    /* if we're here we've over-run out_len and we need to 
       truncate the output.  first tack on a null byte. */
    out[out_i] = '\0';

    /* now compute the length of over-run, if any
       this mimics the return code behavior of snprintf() */
    for ( ; in[in_i] != '\0'; in_i++) {
        if (in[in_i] == '\"' ||
            in[in_i] == '\'' ||
            in[in_i] == '\\') {
            out_i += 2;
        } else {
            out_i++;
        }
    }

    return out_i;
}


/* trims quotes from the provided string
   this is DESTRUCTIVE!  it modifies the string you pass */
char *string_trim_quotes(char *string)
{ 
    char *p = string;
    size_t last_char = strlen(string) - 1;

    /* look for end quote */
    if (string[last_char] == '"' ||
        string[last_char] == '\'')
            string[last_char] = '\0';

    /* look for start quote */
    if (string[0] == '"' ||
        string[0] == '\'')
        p++;

    return p;
}
