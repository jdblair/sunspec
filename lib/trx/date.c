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

#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "date.h"
#include "macros.h"


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
 * \param unixtime the time value to convert
 * \param usec the 

 * \returns the number of characters written to buf, not including the
 *          trailing null byte, or -1 if the string doesn't fit.
 */
int date_snprintf_rfc3339_z(char *buf, size_t len, time_t unixtime, int usec)
{    
    struct tm tm;
    gmtime_r(&unixtime, &tm);  /* use rentrant version */
    return _date_snprintf_rfc3339_tm(buf, len, "%Y-%m-%dT%TZ", &tm, usec);
}

int date_snprintf_rfc3339(char *buf, size_t len, time_t unixtime, int usec)
{    
    struct tm tm;
    localtime_r(&unixtime, &tm);  /* use rentrant version */
        
    return _date_snprintf_rfc3339_tm(buf, len, "%Y-%m-%dT%T%z", &tm, usec);
}

/* this is some logic that is shared between the two rfc3339 date 
   output functions */
int _date_snprintf_rfc3339_tm(char *buf, size_t len, char *fmt,
                              struct tm *tm, int usec)
{
    int offset;

    offset = strftime(buf, len, fmt, tm);
    if (offset == 0)
        return -1;
    if (usec != 0) {
        offset += snprintf(buf + offset, len, ".%d", usec);
        if (offset > BUFFER_SIZE)
            return -1;
    }
    
    return offset;
}    


/**
 * Parse an rfc3339 form time value into a struct tm (broken down time).
 * The timezone value, if present, is stored in the tm_gmtoffset value
 * of struct tm.  It appears this field is ignored by asctime(), so pay
 * attention to it.  Any fractional portion is stored in *milliseconds.
 *
 * \param buf      The string containing the RFC3339 time value.
 * \param tm_time  Pointer to a struct tm which will hold the results.
 * \param usecs    Pointer to an integer to hold the fractional second
 *                 portion, if any, in microseconds.
 * \returns        -1 on parsing error, 0 on success.
 **/
int date_parse_rfc3339_tm(char *buf, struct tm *tm_time, int *usecs)
{
    typedef enum rfc3339_parser_state {
        D_YEAR       =  0,
        D_MON        =  1,
        D_MDAY       =  2,
        D_HOUR       =  3,
        D_MIN        =  4,
        D_SEC        =  5,
        D_SUBSEC     =  6,
        D_ZONE_SIGN  =  7,
        D_ZONE_HOUR  =  8,
        D_ZONE_MIN   =  9,
        D_END        = 10,
    } rfc3339_parser_state_t;

    /* these must match the order of the enums */
    int field_length[] = {
        4,    /* D_YEAR */
        2,    /* D_MON  */
        2,    /* D_MDAY */
        2,    /* D_HOUR */
        2,    /* D_MIN  */
        2,    /* D_SEC  */
    };

    int buf_char = 0;
    int tmpbuf_char = 0;
    char tmpbuf[BUFFER_SIZE];
    rfc3339_parser_state_t state = D_YEAR;
    int zone_sign = 1;
    int zone_hour = 0;
    int zone_min = 0;
    float subsec = 0;

    /* zero out *tm_time */
    memset(tm_time, 0, sizeof(struct tm));

    /* begin parser state machine */
    while (state != D_END) {
        switch (buf[buf_char]) {
        case '-':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_YEAR:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_year)) != 1) {
                    debug("can't parse year field: %s", tmpbuf);
                    return -1;
                }
                /* tm_year starts at 1900 */
                tm_time->tm_year -= 1900;
                state = D_MON;
                break;
            case D_MON:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_mon)) != 1) {
                    debug("can't parse month field: %s", tmpbuf);
                    return -1;
                }
                /* tm_mon starts at zero */
                tm_time->tm_mon--;
                state = D_MDAY;
                break;
            case D_SEC:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_sec)) != 1) {
                    debug("can't parse seconds field: %s", tmpbuf);
                    return -1;
                }
                /* if we're here the timezone starts with a negative sign */
                zone_sign = -1;
                state = D_ZONE_HOUR;
                break;
            default:
                debug("invalid state: found '-' in state %d", state);
                return -1;
            }
            tmpbuf_char = 0;
            break;

        case 'T':
        case 't':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_MDAY:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_mday)) != 1) {
                    debug("can't parse day of month field");
                    return -1;
                }
                state = D_HOUR;
                break;
            default:
                debug("invalid state: found 'T' in state %d", state);
                return -1;
            }
            tmpbuf_char = 0;
             break;

        case ':':
            tmpbuf[tmpbuf_char++] = '\0';            
            switch (state) {
            case D_HOUR:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_hour)) != 1) {
                    debug("can't parse hour field: %s", tmpbuf);
                    return -1;
                }
                state = D_MIN;
                break;
            case D_MIN:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_min)) != 1) {
                    debug("can't parse hour field: %s", tmpbuf);
                    return -1;
                }
                state = D_SEC;
                break;
            case D_ZONE_HOUR:
                if (sscanf(tmpbuf, "%d", &zone_hour) != 1) {
                    debug("can't parse hour field: %s", tmpbuf);
                    return -1;
                }
                state = D_ZONE_MIN;
                break;
            default:
                debug("invalid state: found ':' in state %d", state);
                return -1;
            }
            tmpbuf_char = 0;
            break;

        case '+':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_SEC:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_sec)) != 1) {
                    debug("can't parse seconds field: %s", tmpbuf);
                    return -1;
                }
                state = D_ZONE_HOUR;
                break;
            case D_SUBSEC:
                if (sscanf(tmpbuf, "%f", &subsec) != 1) {
                    debug("can't parse fractional seconds field: %s", tmpbuf);
                    return -1;
                }
                *usecs = (int) round(subsec * 1000000);
                state = D_ZONE_HOUR;
                break;
            default:
                debug("invalid state: found '+' in state %d", state);
                return -1;
            }
            tmpbuf_char = 0;
            /* if we're here the timezone starts with a plus sign */
            zone_sign = 1;
            break;

        case '.':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_SEC:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_sec)) != 1) {
                    debug("can't parse seconds field");
                    return -1;
                }
                state = D_SUBSEC;
                /* include 0. at the start of tmpbuf */
                tmpbuf_char = 0;
                tmpbuf[tmpbuf_char++] = '0';
                tmpbuf[tmpbuf_char++] = '.';
                break;
            default:
               debug("invalid state: found '.' in state %d", state);
               return -1;
            }
            break;

        case 'Z':
        case 'z':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_SUBSEC:
                if (sscanf(tmpbuf, "%f", &subsec) != 1) {
                    debug("can't parse fractional seconds field");
                    return -1;
                }
                *usecs = (int) round(subsec * 1000000);
                state = D_END;
                break;
            case D_SEC:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_sec)) != 1) {
                    debug("can't parse seconds field");
                    return -1;
                }
                state = D_END;
                break;
            default:
               debug("invalid state: found 'Z' in state %d", state);
               return -1;
            }
            zone_hour = 0;
            zone_min = 0;
            tmpbuf_char = 0;
            break;

        case '\0':
            tmpbuf[tmpbuf_char++] = '\0';
            switch (state) {
            case D_ZONE_MIN:
                if (sscanf(tmpbuf, "%d", &zone_min) != 1) {
                    debug("can't parse seconds field: %s", tmpbuf);
                    return -1;
                }
                state = D_END;
                break;
            case D_ZONE_HOUR:
                if (sscanf(tmpbuf, "%d", &zone_hour) != 1) {
                    debug("can't parse hour field: %s", tmpbuf);
                    return -1;
                }
                state = D_END;
                break;
            case D_MDAY:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_mday)) != 1) {
                    debug("can't parse day of month field: %s", tmpbuf);
                    return -1;
                }
                state = D_END;
                break;
            case D_SEC:
                if (sscanf(tmpbuf, "%d", &(tm_time->tm_sec)) != 1) {
                    debug("can't parse seconds field: %s", tmpbuf);
                    return -1;
                }
                state = D_END;
                break;
            default:
                debug("invalid state: found end in state %d", state);
                return -1;
            }
            tmpbuf_char = 0;
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
            /* check for buffer over-run */
            if (tmpbuf_char >= BUFFER_SIZE - 2) {
                debug("temporary buffer is full!  "
                      "this means the date string must be malformed.");
                return -1;
            }

            /* check for limits on numeric fields */
            if (state < D_SUBSEC) {
                if (tmpbuf_char > field_length[state]) {
                    debug("exceeded field_length[%d]", state);
                    return -1;
                }
            }
                    
            tmpbuf[tmpbuf_char++] = buf[buf_char];
            break;

        default:
            /* unexpected character */
            debug("found unexpected character: %c", buf[buf_char]);
            return -1;
        }

        buf_char++;
    }

    tm_time->tm_gmtoff = ((zone_hour * 3600) + (zone_min * 60)) * zone_sign;
    tm_time->tm_isdst = -1;   /* tells mktime() to determine whether or not
                                 daylight savings was in effect */

    return 0;
}


/**
 * Parse the provided rfc3339 formatted string into GMT unixtime.
 * Any fractional second part is left in usecs.
 *
 * \param buf        String containing the rfc3339 time value.
 * \param unixtime	 Pointer to a time_t to store the unixtime in.
 * \param usecs      Pointer to store the microseconds in, if any.
 * \returns          0 on success, -1 on parsing error.
 **/
int date_parse_rfc3339_to_unixtime_z(char *buf, time_t *unixtime, int *usecs)
{
    int rc;
    struct tm tm_time;
    int tm_gmtoff;

    rc = date_parse_rfc3339_tm(buf, &tm_time, usecs);

    if (rc < 0)
        return rc;

    /* stash tm_time.tm_gmtoff b/c mktime() modifies tm_time, setting
       tm_time.tm_gmtoff to the current hosts's offset */
    tm_gmtoff = tm_time.tm_gmtoff;
    *unixtime = mktime(&tm_time);
    
    /* now apply the tm_gmtoff we saved above */
    *unixtime -= tm_time.tm_gmtoff;

    return 0;
}


