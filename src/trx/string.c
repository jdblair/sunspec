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

#include "macros.h"
#include "string.h"
#include "debug.h"

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


/* escapes the 5 xml entities which cannot appear in strings
   also escapes non-printing ascii characters (1-32 and 127)
   returns the number of characters actually written to *out */
int string_escape_xml(char *in, char *out, size_t out_len)
{
    typedef struct xml_escape_map {
        char glyph;
        char *entity;
        size_t len;   /* optimization */
    } xml_escape_map_t;

    /* the 5 xml escape entities */
    xml_escape_map_t map[] = {
        { '"',  "&quot;",  6 },
        { '\'', "&apos;",  6 },
        { '<',  "&lt;",    4 },
        { '>',  "&gt;",    4 },
        { '&',  "&amp;",   5 },
        { '\0', NULL,      0 }
    };

    int i;
    int in_offset = 0;
    int out_offset = 0;
    
    /* read *in one character at a time */
    while (in[in_offset] != '\0') {
        
        /* check against our table of escape values */
        for (i = 0; map[i].glyph != '\0'; i++) {
            if (in[in_offset] == map[i].glyph) {
                if ((out_len - out_offset) > map[i].len) {
                    /* we know we have space for map[i].entity */
                    strcpy(out + out_offset, map[i].entity);
                    out_offset += map[i].len;
                    break;
                } else {
                    /* no space for entity; no sense copying just
                       part of it */
                    debug("not enough space to escape entire string");
                    break;
                }
            }
        }

        /* are we on some other character?  use the ascii escape code */
        /* these escape codes are for html
           i'm not actually sure they're ok for use generally in xml */
        if (map[i].glyph == '\0') {
            /* check for non-printing characters */
            if (in[in_offset] < 32) {
                if ((out_len - out_offset) > 5) {
                    /* always takes up 5 characters */
                    sprintf(out + out_offset, "&#%02x;", (int) in[in_offset]);
                    out_offset += 5;
                }
            } else if (in[in_offset] == 127) {
                if ((out_len - out_offset) > 6) {
                    /* always takes up 6 characters */
                    sprintf(out + out_offset, "&#127;");
                    out_offset += 6;
                }
            } else {
                /* copy ordinary non-escape character */
                out[out_offset] = in[in_offset];
                out_offset++;
            }
        }

        in_offset++;
    }            

    /* tie off the string */
    out[out_offset] = '\0';
    in_offset++;

    return out_offset;
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


/*

int string_fmt_decimal(char *buf, size_t len,
                       long value, int power_of_ten)
{
    char *tmp_buf[BUFFER_SIZE];
    int digits;
    int rc = 0;

    digits = snprintf(buf, len, "%l", &value);

    if (digits > BUFFER_SIZE) {
        debug("ascii encoded decimal number is larger than tmp buffer size");
        error("internal memory error");
        return -1;
    }

    return rc;
}
    
*/
