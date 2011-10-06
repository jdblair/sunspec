/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_unit_tests.h
 *
 * unit tests for internal data transformations
 *
 * Copyright (c) 2011, John D. Blair <jdb@moship.net>
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


#ifndef _SUNS_UNIT_TESTS_H_
#define _SUNS_UNIT_TESTS_H_

#include "trx/debug.h"
#include "trx/macros.h"

typedef int (*unit_test_f)(const char **name);
int test_getopt(int argc, char *argv[]);
int unit_test_type_sizes(const char **name);
int unit_test_byte_order(const char **name);
int unit_test_value_to_buf(const char **name);
int unit_test_buf_to_value(const char **name);
int unit_test_snprintf_suns_value_t(const char **name);
int unit_test_type_name_conversion(const char **name);
int unit_test_suns_value_meta_string(const char **name);
int unit_test_suns_type_size(const char **name);

#endif /* _SUNS_UNIT_TESTS_H_ */
