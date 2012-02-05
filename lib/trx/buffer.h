/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * buffer.h
 * $Id: buffer.h 58 2010-09-25 16:07:06Z jdblair $
 *
 * general purpose data buffer header file
 *
 *
 * Copyright (c) 2008-2011, John D. Blair <jdb@moship.net>
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

#ifndef max
#define max(a,b) (a > b ? a : b)
#endif

#ifndef min
#define min(a,b) (a < b ? a : b)
#endif

/* free space in buffer */
#define buffer_space(buf) ( ((buf)->start + (buf)->size) - (buf)->in)

/* length (or size) of data in buffer */
#define buffer_len(buf) ((buf)->in - (buf)->out)

/* size of the buffer storage area */
#define buffer_size(buf) ((buf)->size)

/* wasted, currently unusable space in the buffer
   (can be recovered with buffer_compact()          */
#define buffer_unusable(buf) ((buf)->in - (buf)->start)

/* pointer to start of data in buffer */
#define buffer_data(buf) ((buf)->out)

/* return char at given data offset (same as buffer_data(buf)[offset]) */
#define buffer_peek(buf, offset) ((buf)->out[offset])

/* writes one char to the end of the buffer */
#define buffer_poke(buf, char) {(buf)->in[0] = char; (buf)->in++;}

/* return a pointer to the start of the last 'len' characters
   useful for reading data just added to the buffer */
#define buffer_last(buf, len) ((buf)->in - len)

/* reset pointers to start over-- data is not erased but is no longer usable */
#define buffer_reset(buf) {(buf)->in = (buf)->out = (buf)->start;}

/* convenience macro to output hex representation of data buffer to stdout */
#define buffer_dump(buf) dump_buffer(stdout, \
                                     (unsigned char*) buffer_data(buf), \
                                     buffer_len(buf))

/* advance the out pointer by the requested len */
#define buffer_eat(buf, len) {                             \
        if ((len) > buffer_len(buf))                       \
            (buf)->out = (buf)->out + buffer_len(buf);     \
        else                                               \
            (buf)->out = (buf)->out + (len); }


/* macro for the most common usage for buffer_part_copy_from() */
#define buffer_copy_from(buf, dest, len) buffer_part_copy_from(buf, 0, dest, len)

/* macro to use buffer_to_buffer_copy_len to copy all available data
   that will fit in the destination buffer */
#define buffer_to_buffer_copy(from, to) \
    buffer_to_buffer_copy_len(from, to, buffer_len(from))


typedef struct buffer {
    char * start;  /* beginning of buffer */
    char * in;     /* where to write new data */
    char * out;    /* where to read data from */
    size_t size;   /* total size of buffer */
} buffer_t;

buffer_t * buffer_new(size_t size);
buffer_t * buffer_new_static_init(buffer_t *buf,
                                  size_t size,
                                  char *data);
void buffer_free(buffer_t *buf);
void buffer_change_size(buffer_t *buf, size_t size);
int buffer_compact(buffer_t *buf);
int buffer_read_to(buffer_t *buf, int fd);
int buffer_write_from(buffer_t *buf, int fd);
int buffer_part_copy_from(buffer_t *buf, size_t offset,
			  char *dest, size_t len);
int buffer_copy_to(buffer_t *buf, char *src, size_t len);
void buffer_info(buffer_t *buf);
int buffer_to_buffer_copy_len(buffer_t *from, buffer_t *to, size_t len);
int buffer_printf(buffer_t *buf, char *format, ...);
int string_escape_quotes(char *in, char *out, size_t out_len);
int buffer_resize(buffer_t *buf, size_t size);

#endif /* _BUFFER_H_ */
