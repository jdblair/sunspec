/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * buffer.c
 * $Id: buffer.c 58 2010-09-25 16:07:06Z jdblair $
 *
 * Implements a general purpose data buffer similar to that provided by stdio.
 * Why implement another one?  Mainly this gives us a way to work with
 * varariable data quantities in a convenient manner.  This can be especially
 * useful when working with nonblocking IO.
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "buffer.h"
#include "string.h"

/* allocate a new buffer of the specified size */
buffer_t * buffer_new(size_t size)
{
    buffer_t *new;

    if ((new = (buffer_t *) malloc(sizeof(buffer_t))) == NULL) {
        return NULL;
    }

    new->start = (char *) malloc(size);
    new->in = new->start;
    new->out = new->start;
    new->size = size;
    
    return new;
}


/* realloc() the buffer
   the new buffer size must be bigger than the current size */
int buffer_resize(buffer_t *buf, size_t size)
{
    assert(size > buffer_size(buf));

    void *p;
    
    /* move all the memory to the start of the buffer */
    buffer_compact(buf);

    p = realloc(buf->start, size);
    if (p == NULL) {
        return -1;
    }

    /* point in & out to the correct offset in the new memory chunk */
    buf->in  = p + (buf->in  - buf->start);
    buf->out = p + (buf->out - buf->start);
    /* now move start and set size... */
    buf->start = p;
    buf->size = size;
    /* ta da!  we're bigger */

    return 0;
}


/* initializes the pointers of a precreated buffer_t to use an also
   precreated buffer space.

   this function allows creating statically allocated buffer_t objects
   in situations where use of malloc() is undesired or impossible

   buffer_change_size() and buffer_free() will FAIL when used on a
   buffer_t created in this way
*/
buffer_t * buffer_new_static_init(buffer_t *buf,
                                  size_t size,
                                  char *data)
{
    buf->start = data;
    buf->in = buf->start;
    buf->out = buf->start;
    buf->size = size;

    return buf;
}
    

/* free the memory used by the buffer */
void buffer_free(buffer_t *buf)
{
    free(buf->start);
    free(buf);
}


/* print out information about the buffer for debugging */
void buffer_info(buffer_t *buf)
{
    printf("buffer at 0x%p:\n", buf);
    printf("  total size =   %d\n", (int) buf->size);
    printf("  current size = %ld\n", buffer_len(buf));
    printf("  space left =   %ld\n", buffer_space(buf));
    printf("  buf->in =      %p\n", buf->in);
    printf("  buf->out =     %p\n", buf->out);
    debug("buf->start = \"%s\"", buf->start);
}


/* free up the wasted space in the buffer by moving the data to the beginning
   of the buffer. */
int buffer_compact(buffer_t *buf)
{
    size_t len = buffer_len(buf);

    /* don't bother if we're actually full */
    if (buffer_unusable(buf) <= 0) {
        return 0;
    }

    if (len > 0) {
        memmove(buf->start, buf->out, len);
    }

    buf->out = buf->start;
    buf->in = buf->start + len;

    return len;
}

/* change the size of the buffer.  data will be lost if you make the buffer
   smaller than the current data size (the buffer_len() value) */
void buffer_change_size(buffer_t *buf, size_t size)
{
    assert(buf != NULL);
    assert(size > 0);

    char * new = (char *) malloc(size);
    if (buffer_len(buf) > 0) {
        memcpy(new, buf->out, min(size, buffer_len(buf)));
    }
    free(buf->start);
    buf->start = new;
    buf->out = buf->start;
    buf->in = buf->out + buffer_len(buf);
}



/*
 * buffer_copy_from() and buffer_part_copy_from() copy data out of the buffer
 * into the provided *dest.  The difference is that buffer_part_copy_from()
 * starts copying at start + offset, so it discards data preceding the offset.
 *
 * buffer_copy_from() is simply implemented as a macro of the general version.
 */

int buffer_part_copy_from(buffer_t *buf, size_t offset, char *dest, size_t len)
{
    assert(buf != NULL);
    assert(dest != NULL);
    assert(len > 0);
    assert(offset >= 0);

    size_t copy_len = min(buffer_len(buf), len - offset);

    if (copy_len == 0) {
        debug("nothing in buffer");
        return 0;
    }

    memcpy((void *) dest, (void *) buf->out + offset, copy_len);

    buf->out += copy_len;
    
    return copy_len;
}


/* copy data to the buffer out of the specified src pointer */
int buffer_copy_to(buffer_t *buf, char *src, size_t len)
{
    assert(buf != NULL);
    assert(src != NULL);
    assert(len > 0);

    size_t copy_len;

    if (buffer_space(buf) < len) {
        buffer_compact(buf);
    }

    if ( buffer_space(buf) == 0 ) {
        debug("no space in buffer");
        return 0;
    }
    
    copy_len = min(buffer_space(buf), len);
    
    memcpy((void *) buf->in, (void *) src, copy_len);

    buf->in += copy_len;

    return copy_len;
}

/* read as much into the buffer from the specified file descriptor as possible */
int buffer_read_to(buffer_t *buf, int fd)
{
    assert(buf != NULL);

    int rc;
     
    if ( buffer_space(buf) == 0 ) {
        buffer_compact(buf);
	
        /* stil no space? */
        if (buffer_space(buf) == 0) {
            debug("no space in buffer");
            return 0;
        }
    }
    
    rc = read(fd, buf->in, buffer_space(buf));
    if (rc < 0) {
        return rc;
    }
    buf->in += rc;

    return rc;
}


/* write as much from the buffer to the specified file descriptor as possible
 */
int buffer_write_from(buffer_t *buf, int fd)
{
    assert(buf != NULL);

    int rc;
    
    if ( buffer_len(buf) == 0 ) {
        debug("nothing in buffer");
        return 0;
    }
    
    rc = write(fd, buf->out, buffer_len(buf));
    if (rc < 0) {
        return rc;
    }
    buf->out += rc;

    return rc;
}


/*
 * copy data directly from the "from" buffer to the "to" buffer.
 * only copies len bytes
 *
 * use the macro buffer_to_buffer_copy() to copy all available data
 * that will fit in the destination buffer.
 */
int buffer_to_buffer_copy_len(buffer_t *from, buffer_t *to, size_t len)
{
    assert(from != NULL);
    assert(to != NULL);

    int rc;

    if ( buffer_len(from) == 0 ) {
        debug("nothing in from buffer");
        return 0;
    }

    if ( buffer_space(to) == 0 ) {
        debug("no space in destination buffer");
        return 0;
    }

    rc = buffer_copy_to(to, buffer_data(from), len);
    from->out += rc;

    return rc;
}


/* 
 * printf to a buffer_t
 * 
 * no more data will be printf'd to the buffer_t then what will fit.
 *
 * mimicing the behavior of snprintf(), the number of bytes which
 * would be printed, whether or not it is actually truncated, is
 * returned.
 *
 */
int buffer_printf(buffer_t *buf, char *format, ...)
{
    int rc;
    va_list ap;

    if ( buffer_space(buf) == 0 ) {
        buffer_compact(buf);
	
        /* stil no space? */
        if (buffer_space(buf) == 0) {
            debug("no space in buffer");
            return 0;
        }
    }

    va_start(ap, format);
    rc = vsnprintf(buf->in, buffer_space(buf), format, ap);
    va_end(ap);

    buf->in += max(buffer_space(buf), rc);

    return rc;

}



