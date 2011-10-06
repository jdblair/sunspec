/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * macros.h
 * $Id: buffer.h 58 2010-09-25 16:07:06Z jdblair $
 *
 * John's handy macros.
 *
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


#ifndef _MACROS_H_
#define _MACROS_H_

#include "debug.h"

#define SMALL_BUFFER_SIZE 16
#define BUFFER_SIZE 128
#define BIG_BUFFER_SIZE 1024

/* max and min are universally useful */
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/* logging and reporting facilities */

#ifndef verbose
#define verbose(level, format, arg...) if (verbose_level >= level) \
	fprintf(stderr, format "\n" , ## arg)
#endif /* verbose */

#ifndef error
#define error(format, arg...) fprintf(stderr, "ERROR: " format "\n", ## arg)
#endif /* error */

#ifndef warning
#define warning(format, arg...) \
    fprintf(stderr, "warning: " format "\n" , ## arg)
#endif /* error */

#ifndef log
#define log(facility, level, format, arg...)
#endif

/* macro to simplify checking the return value of a function */
#define return_if_fail(expr) \
    {\
	int r;			\
	if ((r = (expr)) < 0) { \
	    debug("%s returned %d", #expr, r);	\
	    return r; \
	} \
    }

#define error_if_fail(expr) \
    {\
	int r;			\
	if ((r = (expr)) < 0) { \
	    error("%s returned %d", #expr, r);	\
	    return r; \
	} \
    }

#define warning_if_fail(expr) \
    {\
	int r;			\
	if ((r = (expr)) < 0) { \
	    warning("%s returned %d: %m", #expr, r);	\
	} \
    }


#endif /*_MACROS_H_ */
