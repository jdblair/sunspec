/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * date.c
 * $Id: $
 *
 * Date/time processing utility functions.
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
#include <time.h>
#include "date.h"


/**
 * Convert the provided unixtime to a string using RFC 822 date format
 * followed by 24-hour HH:MM:SS format, in GMT.
 *
 * \param buf the buffer to write to
 * \param len the length of the buffer to write the string to
 * \timeval the time value to convert
 * \returns the return value of strftime(): number of characters written to
 *          buf, not including the trailing null byte, or zero if the string
 *          doesn't fit. Note that this differs from snprintf()!!
 */
int date_rfc822_z(char *buf, size_t len, time_t unixtime)
{
    struct tm tm;
    gmtime_r(&unixtime, &tm);  /* use rentrant version */
    return strftime(buf, len, "%a, %d %b %Y %H:%M:%S %z", &tm);
}

/**
 * Convert the provided unixtime to a string using RFC 822 date format
 * followed by 24-hour HH:MM:SS format, in the local timezone.
 *
 * \param buf the buffer to write to
 * \param len the length of the buffer to write the string to
 * \timeval the time value to convert
 * \returns the return value of strftime(): number of characters written to
 *          buf, not including the trailing null byte, or zero if the string
 *          doesn't fit. Note that this differs from snprintf()!!
 */
int date_rfc822(char *buf, size_t len, time_t unixtime)
{
    struct tm tm;
    localtime_r(&unixtime, &tm);  /* use rentrant version */
    return strftime(buf, len, "%a, %d %b %Y %H:%M:%S %z", &tm);
}

/**
 * Convert the provided unixtime to a string using RFC 3339 date format
 * followed by 24-hour HH:MM:SS format, in the local timezone.
 *
 * \param buf the buffer to write to
 * \param len the length of the buffer to write the string to
 * \timeval the time value to convert
 * \returns the return value of strftime(): number of characters written to
 *          buf, not including the trailing null byte, or zero if the string
 *          doesn't fit. Note that this differs from snprintf()!!
 */
int date_snprintf_rfc3339_z(char *buf, size_t len, time_t unixtime)
{    
    struct tm tm;
    gmtime_r(&unixtime, &tm);  /* use rentrant version */
    return strftime(buf, len, "%Y-%m-%dT%TZ", &tm);
}

int date_snprintf_rfc3339(char *buf, size_t len, time_t unixtime)
{    
    struct tm tm;
    localtime_r(&unixtime, &tm);  /* use rentrant version */
    return strftime(buf, len, "%Y-%m-%dT%T-%z", &tm);
}

		    
