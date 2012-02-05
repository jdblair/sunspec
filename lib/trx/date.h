/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * date.h
 *
 * Date and time related helper functions.
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

#ifndef _DATE_H_
#define _DATE_H_

#include "time.h"

int date_iso8601_z(char *buf, size_t len, time_t unixtime);
int date_iso8601(char *buf, size_t len, time_t unixtime);
int _date_snprintf_rfc3339_tm(char *buf, size_t len, char *fmt,
                              struct tm *tm, int usec);
int date_snprintf_rfc3339_z(char *buf, size_t len, time_t unixtime, int usec);
int date_snprintf_rfc3339(char *buf, size_t len, time_t unixtime, int usec);
int date_parse_rfc3339_tm(char *buf, struct tm *tm_time, int *usec);
int date_parse_rfc3339_to_unixtime_z(char *buf, time_t *unixtime, int *usec);

#endif /* _DATE_H_ */
