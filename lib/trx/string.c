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


/**
 * encapsulates the logic to safely copy a string of arbitrary
 * length.  uses double pointers to modify the existing buffer pointer
 * and accompanying length notation in-place.  the **to buffer may
 * grow to accomidate the string, but it will never shrink.
 *
 * \param **to  double pointer to the destination buffer
 * \param **to_len length of the destination buffer
 * \param *from buffer to copy from
 * \returns 0 on success, -1 if realloc() was needed and failed
 */
int string_set_any_length(char **to, size_t *to_len, const char *from)
{
    int from_len = strlen(from);
    char *ptr = *to;
    int rc = 0;
    
    if (from_len + 1 > *to_len) {
        size_t new_size = from_len + 1;
        ptr = realloc(*to, new_size);
        /* if realloc() failed, don't change *to and *to_len */
        if (ptr) {
            *to = ptr;
            *to_len = new_size;
        }
    }
    
    if (ptr) {
        strncpy(*to, from, *to_len);
    } else {
        rc = -1;
        error("realloc() failed!");
    }

    return rc;
}


int string_parse_decimal_long(char *buf, long int *base, int *exp)
{
    char base_str[BUFFER_SIZE];

    if (string_decompose_decimal(buf, base_str, BUFFER_SIZE, exp) < 0)
        return -1;

    if (sscanf(base_str, "%ld", base) != 1) {
        debug("can't parse base string '%s'", base_str);
        return -1;
    }


    return 0;
}


/**
 * This is a state-machined based parser used to break a decimal floating point
 * into its component parts.  This function allows this logic to be
 * implemented in a single place while allowing us to coerce the resulting
 * values into different c datatypes.
 *
 * The parts are:
 *  base: an string integer with no decimal point, optional sign
 *  e:    an integer power-of-ten to be applied to the base
 *
 * \param *buf       string to parse
 * \param *base      string to put the resulting integer into
 * \param *base_len  length of buffer to store the base in
 * \param *e         pointer to an integer to store the power-of-ten exponent
 *
 **/
int string_decompose_decimal(const char *buf,
                             char *base, size_t base_len, int *e)
{
    typedef enum decimal_parser_state {
        T_START = 0,
        T_BASE = 1,
        T_FRACTION = 2,
        T_EXP  = 3,
        T_END  = 4,
    } decimal_parser_state_t;

    char tmpbuf[BUFFER_SIZE];
    int buf_char = 0;
    int tmpbuf_char = 0;
    int decimal_places = 0;
    decimal_parser_state_t state = T_START;

    *base = 0;
    *e = 0;

    while (state != T_END) {
        switch(buf[buf_char]) {
        case '-':
        case '+':
            switch (state) {
            case T_START:
                tmpbuf[tmpbuf_char++] = buf[buf_char++];
                state = T_BASE;
                break;
            case T_EXP:
                tmpbuf[tmpbuf_char++] = buf[buf_char++];
                break;
            default:
                debug("%c found in location other than start, offset %d",
                      tmpbuf[tmpbuf_char], tmpbuf_char);
            }
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* check for buffer overflow */
            if (tmpbuf_char >= BUFFER_SIZE) {
                debug("temporary buffer size %d exceeded", BUFFER_SIZE);
                return -1;
            }
            tmpbuf[tmpbuf_char++] = buf[buf_char++];
            switch (state) {
            case T_START:
                state = T_BASE;
                break;
            case T_FRACTION:
                /* count how many digits are to the right of the point */
                decimal_places++;
                break;
            default:
                /* all other states are allowed */
                break;
            }

            break;

        case '.':
            switch (state) {
            case T_START:
            case T_BASE:
                state = T_FRACTION;
                buf_char++;
                break;
            default:
                debug("decimal point not expected at offset %d, state = %d",
                      buf_char, state);
                return -1;
            }
            break;

        case 'e':
        case 'E':
            tmpbuf[tmpbuf_char] = '\0';  /* tie off tmpbuf */
            switch (state) {
            case T_FRACTION:
                strncpy(base, tmpbuf, base_len);
                tmpbuf_char = 0;
                buf_char++;
                state = T_EXP;
                break;
            default:
                debug("unexpected character '%c' at offset %d, state = %d",
                      tmpbuf[tmpbuf_char], tmpbuf_char, state);
                return -1;
            }
            break;
            
        case '\0':
            tmpbuf[tmpbuf_char] = '\0';  /* tie off tmpbuf */
            switch (state) {
            case T_BASE:
            case T_FRACTION:
                /* base and fraction are parsed the same because the
                   decimal point is ignored in tmpbuf */
                strncpy(base, tmpbuf, base_len);
                break;
            case T_EXP:
                if (sscanf(tmpbuf, "%d", e) != 1) {
                    debug("can't parse exponent string '%s'", tmpbuf);
                    return -1;
                }
                break;
            default:
                debug("invalid state: found end in state %d", state);
                return -1;
            }                
            
            state = T_END;
            break;
            
        default:
            debug("unexpected character '%c' at offset %d, state = %d",
                  buf[buf_char], buf_char, state);
            return -1;
        }
    }

    /* subtract any decimal places found in the base from the exponent */
    *e -= decimal_places;

    return 0;
}


int string_read_until_eof(FILE *fp, char *buf, size_t len, size_t chunk_size)
{
    int read_len = 0;
    int offset = 0;
    int num_chunks = len / chunk_size;
    
    /* read until eof or error */
    while ((read_len =
            fread(buf + offset, chunk_size, num_chunks, stdin)) > 0) {
        offset += read_len;
        debug("-> offset = %d", offset);
    }

    /* was it eof or error? */
    if (ferror(fp)) {
        debug("error on stream: %m");
        return -1;
    }

    return offset;
}


int string_read_from_file_or_stdin(char *path, char *buf, size_t len)
{
    FILE *fp;
    int rc;

    debug("path = %s", path);
    
    /* special case: use - to mean stdin */
    if (strcmp(path, "-") == 0) {
        debug("reading from stdin");
        fp = stdin;
    } else {
        fp = fopen(path, "r");
        if (fp == NULL) {
            debug("error opening file: %m");
            return -1;
        }
        debug("read from file %s", path);
    }

    rc = string_read_until_eof(fp, buf, len, 1);

    /* return right now if we're reading from stdin */
    if (strcmp(path, "-") == 0)
        return rc;

    if (rc < 0) {
        /* we've already failed so ignore return code from fclose */
        fclose(fp);
        return rc;
    }
    
    /* pass return code from fclose() back to caller
       to propogate possible failure */
    return fclose(fp);
}
