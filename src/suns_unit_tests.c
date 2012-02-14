/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_unit_tests.c
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

#define _BSD_SOURCE  /* for big/little endian macros */

#define UNIT_ASSERT(exp) if (! (exp)) {			\
	debug("ASSERT FAILED: " #exp);			\
	return -1;					\
    }

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <endian.h>
#include <getopt.h>

#include "trx/debug.h"
#include "trx/macros.h"
#include "suns_unit_tests.h"
#include "suns_model.h"
#include "suns_output.h"


int test_getopt(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
        case 'v':
            verbose_level++;
            break;
        
        default:
            printf("Usage: %s [-vvv]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}


int main (int argc, char *argv[])
{
    int i;
    int pass = 0;
    int fail = 0;
    const char *test_name;

    test_getopt(argc, argv);

    unit_test_f test_list[] = {
        unit_test_type_sizes,
        unit_test_byte_order,
        unit_test_value_to_buf,
        unit_test_buf_to_value,
        unit_test_snprintf_suns_value_t,
        unit_test_type_name_conversion,
        unit_test_suns_value_meta_string,
        unit_test_suns_type_size,
        NULL,
    };

    for (i = 0; test_list[i] != NULL; i++) {
        if (test_list[i](&test_name) < 0) {
            fail++;
            printf("FAIL:  ");
        } else {
            pass++;
            printf("pass:  ");
        }
        printf("%s\n", test_name);
    }

    printf("------------------\n");
    printf("total passed: %d\n", pass);
    printf("total failed: %d\n", fail);

    exit(fail);
}


/* check that the types from stdint.h are the correct size
   this is admittadly a bit paranoid */
int unit_test_type_sizes(const char **name)
{
    *name = __FUNCTION__;

    int total = 0;
    int pass = 0;

    typedef struct type_check_map {
        char *name;
        size_t required;
        size_t actual;
    } type_check_map_t;

    type_check_map_t type_check[] = {
        { "uint16_t", 2, sizeof(uint16_t) },
        { "int16_t",  2, sizeof(int16_t) },
        { "uint32_t", 4, sizeof(uint32_t) },
        { "int32_t", 4, sizeof(int32_t) },
        { "int64_t", 8, sizeof(int64_t) },
        { "uint64_t", 8, sizeof(uint64_t) },
        { "float",   4, sizeof(float) },
        { "double",  8, sizeof(double) },
        { NULL,      0, 0 },
    };

    int i;

    for (i = 0; type_check[i].name != NULL; i++) {
        total++;
        if (type_check[i].required == type_check[i].actual) {
            debug("sizeof(%s) passed", type_check[i].name);
            pass++;
        } else {
            debug("sizeof(%s) failed (required = %d, actual = %d)",
                  type_check[i].name,
                  type_check[i].required,
                  type_check[i].actual);
        }
        
    }

    return pass - total;
}


/* make sure the byte order and byte swapping macros are working correctly */
/* this is also fairly paranoid since these are provided by the OS */
int unit_test_byte_order(const char **name)
{
    *name = __FUNCTION__;

    /* h = host, t = test, number = bytes */
    uint16_t h2, t2;
    uint32_t h4, t4;
    uint64_t h8, t8;

    /* pointers to these test values */
    unsigned char *ph2, *pt2, *ph4, *pt4, *ph8, *pt8;

    int pass = 0;
    int total = 0;

    /* create some endian-neutral test data */
    /* these put the larger value bytes in more signficant bytes
       regardless of host byte ordering, w/o using the byteorder macros */
    h2 = ((0x11 << 8) +   /* most */
          (0x00 << 0));   /* least */
    h4 = ((0x33 << 24) +  /* most */
          (0x22 << 16) +
          (0x11 << 8)  +
          (0x00 << 0));   /* least */
    h8 = ((0x77LL << 56) +  /* most */
          (0x66LL << 48) +
          (0x55LL << 40) +
          (0x44LL << 32) +
          (0x33LL << 24) +
          (0x22LL << 16) +
          (0x11LL << 8)  +
          (0x00LL << 0));   /* least */

    ph2 = (unsigned char *) &h2;
    pt2 = (unsigned char *) &t2;
    ph4 = (unsigned char *) &h4;
    pt4 = (unsigned char *) &t4;
    ph8 = (unsigned char *) &h8;
    pt8 = (unsigned char *) &t8;

    /* test that endian defines are set */
    total++;
#if __BYTE_ORDER == __PDP_ENDIAN
    debug("wtf? PDP_ENDIAN?");
    return 1;
#endif
    /* just to be safe, test values that are 16, 32 and 64 bits wide */
    if ( ph2[0] == 0x11 &&
         ph4[0] == 0x33 &&
         ph8[0] == 0x77 ) {
        debug("host is big endian");
        /* is BIG_ENDIAN defined? */
#if __BYTE_ORDER == __BIG_ENDIAN
        debug("byte order define test passed");
        pass++;
#else
        debug("byte order define test failed");
#endif
    } else {
        debug("host is little endian");
        /* is LITTLE_ENDIAN defined? */
#if __BYTE_ORDER == __LITTLE_ENDIAN
        debug("byte order define test passed");
        pass++;
#else
        debug("byte order define test failed");
#endif
    }

    /* htobe16() */
    total++;
    t2 = htobe16(h2);
    if (pt2[0] == 0x11 &&
        pt2[1] == 0x00) {
        debug("htobe16() passed");
        pass++;
    } else {
        debug("htobe16() failed");
    }

    /* be16toh() */
    total++;
    t2 = be16toh(t2);
    if (t2 == h2) {
        debug("be16toh() passed");
        pass++;
    } else {
        debug("be16toh() failed");
    }

    /* htole16() */
    total++;
    t2 = htole16(h2);
    if (pt2[0] == 0x00 &&
        pt2[1] == 0x11) {
        debug("htole16() passed");
        pass++;
    } else {
        debug("htole16() failed");
    }
        
    /* le16toh() */
    total++;
    t2 = le16toh(t2);
    if (t2 == h2) {
        debug("le16toh() passed");
        pass++;
    } else {
        debug("le16toh() failed");
    }

    /* htobe32() */
    total++;
    t4 = htobe32(h4);
    if (pt4[0] == 0x33 &&
        pt4[1] == 0x22 &&
        pt4[2] == 0x11 &&
        pt4[3] == 0x00) {
        debug("htobe32() passed");
        pass++;
    } else {
        debug("htobe32() failed");
    }

    /* be32toh() */
    total++;
    t4 = be32toh(t4);
    if (t4 == h4) {
        debug("be32toh() passed");
        pass++;
    } else {
        debug("be32toh() failed");
    }

    /* htole32() */
    total++;
    t4 = htole32(h4);
    if (pt4[0] == 0x00 &&
        pt4[1] == 0x11 &&
        pt4[2] == 0x22 &&
        pt4[3] == 0x33) {
        debug("htole32() passed");
        pass++;
    } else {
        debug("htole32() failed");
    }

    /* le32toh() */
    total++;
    t4 = le32toh(t4);
    if (t4 == h4) {
        debug("le32toh() passed");
        pass++;
    } else {
        debug("le32toh() failed");
    }
    
    /* htole64() */
    total++;
    t8 = htole64(h8);
    if (pt8[0] == 0x00 &&
        pt8[1] == 0x11 &&
        pt8[2] == 0x22 &&
        pt8[3] == 0x33 &&
        pt8[4] == 0x44 &&
        pt8[5] == 0x55 &&
        pt8[6] == 0x66 &&
        pt8[7] == 0x77) {
        debug("htole64() passed");
        pass++;
    } else {
        debug("htole64() failed");
    }

    /* le64toh() */
    total++;
    t8 = le64toh(t8);
    if (t8 == h8) {
        debug("le64toh() passed");
        pass++;
    } else {
        debug("le64toh() failed");
    }
    
    debug("%d out of %d byteorder tests passed", pass, total);

    return pass - total;
}


int compare_buf(unsigned char *left, unsigned char *right, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        verbose(4, "left[%d] = %02x, right[%d] = %02x",
                i, left[i], i, right[i]);
        if (left[i] != right[i]) {
            verbose(4, "return -1");
            return -1;
        }
    }

    verbose(4, "return 0");
    return 0;
}

/* test conversion of suns_value_t to a binary buffer */
int unit_test_value_to_buf(const char **name)
{
    *name = __FUNCTION__;

    unsigned char buf[SMALL_BUFFER_SIZE];
    int total = 0;
    int pass = 0;

    suns_value_t *v = suns_value_new();
    /* not much to test here */
    total++;
    if (v != NULL) {
        pass++;
    } else {
        debug("suns_value_new() returned NULL");
        return total - pass;  /* do not pass go */
    }

    total++;
    if (v->tp.type == SUNS_NULL) {
        pass++;
    } else {
        debug("newly minted suns_value_t doesn't have type SUNS_NULL");
    }

    /* negative test */
    total++;
    memset(buf, 0, SMALL_BUFFER_SIZE);
    unsigned char neg_buf[] = { 0xff, 0xff, 0xff, 0xff};
    if (compare_buf(buf, neg_buf, 4) != 0) {
        debug("negative test passed");
        pass++;
    } else {
        debug("negative test failed");
    }

    /* int16 */
    total++;
    unsigned char int16_buf[] = { 0xd8, 0xf1 };
    suns_value_set_int16(v, -9999);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(int16_buf, buf, 2) == 0) {
        debug("int16 passed");
        pass++;
    } else {
        debug("int16 failed");
    }        

    /* int16 not-implemented test */
    total++;
    suns_value_set_int16(v, 0x8000);
    debug("v->value.i16 = %hx", v->value.i16);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int16 not-implemented value passed");
        pass++;
    } else {
        debug("int16 not-implemented value failed");
    }        
    
    /* uint16 */
    total++;
    unsigned char uint16_buf[] = { 0x27, 0x0f };
    suns_value_set_uint16(v, 9999);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(uint16_buf, buf, 2) == 0) {
        debug("uint16 passed");
        pass++;
    } else {
        debug("uint16 failed");
    }        

    /* uint16 not-implemented test */
    total++;
    suns_value_set_uint16(v, 0xFFFF);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint16 not-implemented value passed");
        pass++;
    } else {
        debug("uint16 not-implemented value failed");
    }        

    /* acc16 */
    total++;
    unsigned char acc16_buf[] = { 0x27, 0x0f };
    suns_value_set_acc16(v, 9999);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(acc16_buf, buf, 2) == 0) {
        debug("acc16 passed");
        pass++;
    } else {
        debug("acc16 failed");
    }        

    /* make sure 0xFFFF is possible for acc16 */
    total++;
    suns_value_set_acc16(v, 0xFFFF);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc16 0xFFFF check passed");
        pass++;
    } else {
        debug("acc16 0xFFFF check failed");
    }

    /* int32 */
    total++;
    unsigned char int32_buf[] = { 0xfb, 0x70, 0x0b, 0x16 };
    suns_value_set_int32(v, -76543210);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(int32_buf, buf, 4) == 0) {
        debug("int32 passed");
        pass++;
    } else {
        debug("int32 failed");
    }        
    
    /* int32 not-implemented test */
    total++;
    suns_value_set_int32(v, 0x80000000);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int32 not-implemented value passed");
        pass++;
    } else {
        debug("int32 not-implemented value failed");
    }        

    /* uint32 */
    total++;
    unsigned char uint32_buf[] = { 0x04, 0x8f, 0xf4, 0xea };
    suns_value_set_uint32(v, 76543210);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(htole32(v->value.u32)), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(uint32_buf, buf, 4) == 0) {
        debug("uint32 passed");
        pass++;
    } else {
        debug("uint32 failed");
    }        

    /* uint32 not-implemented test */
    total++;
    suns_value_set_uint32(v, 0xFFFFFFFF);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint32 not-implemented value passed");
        pass++;
    } else {
        debug("uint32 not-implemented value failed");
    }        

    /* acc32 */
    total++;
    unsigned char acc32_buf[] = { 0x04, 0x8f, 0xf4, 0xea };
    suns_value_set_acc32(v, 76543210);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(acc32_buf, buf, 4) == 0) {
        debug("acc32 passed");
        pass++;
    } else {
        debug("acc32 failed");
    }        

    /* make sure 0xFFFFFFFF is possible for acc32 */
    total++;
    suns_value_set_acc32(v, 0xFFFFFFFF);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc32 0xFFFFFFFF check passed");
        pass++;
    } else {
        debug("acc32 0xFFFFFFFF check failed");
    }


    /* float32 */
    total++;
    unsigned char float32_buf[] = { 0x40, 0x49, 0x0f, 0xdb };
    suns_value_set_float32(v, 3.14159265358979323846);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u32), 4);
    debug_dump_buffer(buf, 4);
    if (compare_buf(float32_buf, buf, 4) == 0) {
        debug("float32 passed");
        pass++;
    } else {
        debug("float32 failed");
    }

    /* int64 */
    total++;
    unsigned char int64_buf[] = { 0xd7, 0x52, 0xe5, 0x34,
                                  0x73, 0xd7, 0x0c, 0xf0 };
    suns_value_set_int64(v, -2931028394020238096LL);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u64), 8);
    debug_dump_buffer(buf, 8);
    if (compare_buf(int64_buf, buf, 8) == 0) {
        debug("int64 passed");
        pass++;
    } else {
        debug("int64 failed");
    }        
    
    /* int64 not-implemented test */
    total++;
    suns_value_set_int64(v, 0x8000000000000000LL);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int64 not-implemented value passed");
        pass++;
    } else {
        debug("int64 not-implemented value failed");
    }        

    /* uint64 */
    total++;
    unsigned char uint64_buf[] = { 0x35, 0x6d, 0x16, 0x87,
                                   0x6e, 0xe2, 0x9b, 0xa4, };
    suns_value_set_uint64(v, 3849758027408382884LL);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u64), 4);
    debug_dump_buffer(buf, 8);
    if (compare_buf(uint64_buf, buf, 8) == 0) {
        debug("uint64 passed");
        pass++;
    } else {
        debug("uint64 failed");
    }        

    /* uint64 not-implemented test */
    total++;
    suns_value_set_uint64(v, 0xFFFFFFFFFFFFFFFFLL);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint64 not-implemented value passed");
        pass++;
    } else {
        debug("uint64 not-implemented value failed");
    }        

    /* acc64 */
    total++;
    unsigned char acc64_buf[] = { 0x73, 0x49, 0xcc, 0x69,
                                  0x5e, 0x9a, 0xfd, 0x41, };
    suns_value_set_acc64(v, 8307395740592438593LL);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u64), 8);
    debug_dump_buffer(buf, 8);
    if (compare_buf(acc64_buf, buf, 8) == 0) {
        debug("acc64 passed");
        pass++;
    } else {
        debug("acc64 failed");
    }        

    /* make sure 0xFFFFFFFFFFFFFFFF is possible for acc64 */
    total++;
    suns_value_set_acc64(v, 0xFFFFFFFFFFFFFFFFLL);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u64), 8);
    debug_dump_buffer(buf, 8);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc64 0xFFFFFFFFFFFFFFFF check passed");
        pass++;
    } else {
        debug("acc64 0xFFFFFFFFFFFFFFFF check failed");
    }

    /* float64 */
    total++;
    unsigned char float64_buf[] = { 0x40, 0x05, 0xbf, 0x0a,
                                    0x8b, 0x14, 0x57, 0x69, };
    suns_value_set_float64(v, 2.7182818284590452354L);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
	debug_dump_buffer((unsigned char *)&(v->value.u64), 8);
    debug_dump_buffer(buf, 8);
    if (compare_buf(float64_buf, buf, 8) == 0) {
        debug("float64 passed");
        pass++;
    } else {
        debug("float64 failed");
    }

    /* string */
    total++;
    int i;
    char string_buf[SMALL_BUFFER_SIZE];
    for (i = 0; i < SMALL_BUFFER_SIZE - 1; i++) {
        string_buf[i] = i + 97;
    }
    string_buf[SMALL_BUFFER_SIZE - 1] = '\0';
    suns_value_set_string(v, string_buf, SMALL_BUFFER_SIZE);
    suns_value_to_buf(v, buf, SMALL_BUFFER_SIZE);
    if (compare_buf((unsigned char *) string_buf,
                    (unsigned char *) buf, SMALL_BUFFER_SIZE) == 0) {
        debug("string passed");
        pass++;
    } else {
        debug("string failed");
    }
    
    debug("pass = %d, total = %d", pass, total);

    return pass - total;
}

/* test conversion from a binary buffer a suns_value_t */
int unit_test_buf_to_value(const char **name)
{
    *name = __FUNCTION__;

    int total = 0;
    int pass = 0;
    suns_type_pair_t tp;

    suns_value_t *v = suns_value_new();
    /* not much to test here */
    total++;
    if (v != NULL) {
        pass++;
    } else {
        debug("suns_value_new() returned NULL");
        return total - pass;  /* do not pass go */
    }

    total++;
    if (v->tp.type == SUNS_NULL) {
        pass++;
    } else {
        debug("newly minted suns_value_t doesn't have type SUNS_NULL");
    }

    /* int16 */
    total++;
    unsigned char int16_buf[] = { 0xd8, 0xf1 };
    tp.type = SUNS_INT16;
    suns_buf_to_value(int16_buf, &tp, v);
    if (suns_value_get_int16(v) == -9999) {
        debug("int16 passed");
        pass++;
    } else {
        debug("int16 failed");
    }        

    /* int16 not-implemented */
    total++;
    unsigned char int16_ni_buf[] = { 0x80, 0x00 };
    tp.type = SUNS_INT16;
    suns_buf_to_value(int16_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int16 not-implemented passed");
        pass++;
    } else {
        debug("int16 not-implemented failed");
    }

    /* uint16 */
    total++;
    unsigned char uint16_buf[] = { 0x27, 0x0f };
    tp.type = SUNS_UINT16;
    suns_buf_to_value(uint16_buf, &tp, v);
    if (suns_value_get_uint16(v) == 9999) {
        debug("uint16 passed");
        pass++;
    } else {
        debug("uint16 failed");
    }        

    /* uint16 not-implemented */
    total++;
    unsigned char uint16_ni_buf[] = { 0xFF, 0xFF };
    tp.type = SUNS_UINT16;
    suns_buf_to_value(uint16_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint16 not-implemented passed");
        pass++;
    } else {
        debug("uint16 not-implemented failed");
    }

    /* acc16 */
    total++;
    unsigned char acc16_buf[] = { 0x27, 0x0f };
    tp.type = SUNS_ACC16;
    suns_buf_to_value(acc16_buf, &tp, v);
    if (suns_value_get_acc16(v) == 9999) {
        debug("acc16 passed");
        pass++;
    } else {
        debug("acc16 failed");
    }        

    /* make sure 0xFFFF is possible with acc16 */
    total++;
    unsigned char acc16_buf2[] = { 0xFF, 0xFF };
    tp.type = SUNS_ACC16;
    suns_buf_to_value(acc16_buf2, &tp, v);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc16 0xFFFF check passed");
        pass++;
    } else {
        debug("acc16 0xFFFF check failed");
    }        

    /* int32 */
    total++;
    unsigned char int32_buf[] = { 0xfb, 0x70, 0x0b, 0x16 };
    tp.type = SUNS_INT32;
    suns_buf_to_value(int32_buf, &tp, v);
    if (suns_value_get_int32(v) == -76543210) {
        debug("int32 passed");
        pass++;
    } else {
        debug("int32 failed");
    }        
    
    /* int32 not-implemented */
    total++;
    unsigned char int32_ni_buf[] = { 0x80, 0x00, 0x00, 0x00 };
    tp.type = SUNS_INT32;
    suns_buf_to_value(int32_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int32 not-implemented passed");
        pass++;
    } else {
        debug("int32 not-implemented failed");
    }

    /* uint32 */
    total++;
    unsigned char uint32_buf[] = { 0x04, 0x8f, 0xf4, 0xea };
    tp.type = SUNS_UINT32;
    suns_buf_to_value(uint32_buf, &tp, v);
    if (suns_value_get_uint32(v) == 76543210) {
        debug("uint32 passed");
        pass++;
    } else {
        debug("uint32 failed");
    }

    /* uint32 not-implemented */
    total++;
    unsigned char uint32_ni_buf[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    tp.type = SUNS_UINT32;
    suns_buf_to_value(uint32_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint32 not-implemented passed");
        pass++;
    } else {
        debug("uint32 not-implemented failed");
    }

    /* acc32 */
    total++;
    unsigned char acc32_buf[] = { 0x04, 0x8f, 0xf4, 0xea };
    tp.type = SUNS_ACC32;
    suns_buf_to_value(acc32_buf, &tp, v);
    if (suns_value_get_acc32(v) == 76543210) {
        debug("acc32 passed");
        pass++;
    } else {
        debug("acc32 failed");
    }        

    /* make sure 0xFFFFFFFF is possible with acc32 */
    total++;
    unsigned char acc32_buf2[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    tp.type = SUNS_ACC32;
    suns_buf_to_value(acc32_buf2, &tp, v);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc32 0xFFFFFFFF check passed");
        pass++;
    } else {
        debug("acc32 0xFFFFFFFF check failed");
    }        

    /* float32 */
    total++;
    unsigned char float32_buf[] = { 0x40, 0x49, 0x0f, 0xdb };
    tp.type = SUNS_FLOAT32;
    suns_buf_to_value(float32_buf, &tp, v);
    /* check to 5 decimal places */
    if (suns_value_get_float32(v) > 3.141590 &&
        suns_value_get_float32(v) < 3.141599) {
        debug("float32 passed");
        pass++;
    } else {
        debug("float32 failed");
    }        
    
    /* float32 not-implemented */
    total++;
    unsigned char float32_ni_buf[] = { 0x7F, 0xC0, 0x00, 0x00 };
    tp.type = SUNS_FLOAT32;
    suns_buf_to_value(float32_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("float32 not-implemented passed");
        pass++;
    } else {
        debug("float32 not-implemented failed");
    }

    /* int64 */
    total++;
    unsigned char int64_buf[] = { 0xd7, 0x52, 0xe5, 0x34,
                                  0x73, 0xd7, 0x0c, 0xf0 };
    tp.type = SUNS_INT64;
    suns_buf_to_value(int64_buf, &tp, v);
    if (suns_value_get_int64(v) == -2931028394020238096LL) {
        debug("int64 passed");
        pass++;
    } else {
        debug("int64 failed");
    }        
    
    /* int64 not-implemented */
    total++;
    unsigned char int64_ni_buf[] = { 0x80, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00 };
    tp.type = SUNS_INT64;
    suns_buf_to_value(int64_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("int64 not-implemented passed");
        pass++;
    } else {
        debug("int64 not-implemented failed");
    }

    /* uint64 */
    total++;
    unsigned char uint64_buf[] = { 0x35, 0x6d, 0x16, 0x87,
                                  0x6e, 0xe2, 0x9b, 0xa4 };
    tp.type = SUNS_UINT64;
    suns_buf_to_value(uint64_buf, &tp, v);
    if (suns_value_get_uint64(v) == 3849758027408382884LL) {
        debug("uint64 passed");
        pass++;
    } else {
        debug("uint64 failed");
    }

    /* uint64 not-implemented */
    total++;
    unsigned char uint64_ni_buf[] = { 0xFF, 0xFF, 0xFF, 0xFF,
                                      0xFF, 0xFF, 0xFF, 0xFF };
    tp.type = SUNS_UINT64;
    suns_buf_to_value(uint64_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("uint64 not-implemented passed");
        pass++;
    } else {
        debug("uint64 not-implemented failed");
    }

    /* acc64 */
    total++;
    unsigned char acc64_buf[] = { 0x73, 0x49, 0xcc, 0x69,
                                  0x5e, 0x9a, 0xfd, 0x41 };
    tp.type = SUNS_ACC64;
    suns_buf_to_value(acc64_buf, &tp, v);
    if (suns_value_get_acc64(v) == 8307395740592438593LL) {
        debug("acc64 passed");
        pass++;
    } else {
        debug("acc64 failed");
    }        

    /* make sure 0xFFFFFFFFFFFFFFFF is possible with acc64 */
    total++;
    unsigned char acc64_buf2[] = { 0xFF, 0xFF, 0xFF, 0xFF,
                                   0xFF, 0xFF, 0xFF, 0xFF };
    tp.type = SUNS_ACC64;
    suns_buf_to_value(acc64_buf2, &tp, v);
    if (v->meta != SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("acc64 0xFFFFFFFF check passed");
        pass++;
    } else {
        debug("acc64 0xFFFFFFFF check failed");
    }        

    /* float64 */
    total++;
    unsigned char float64_buf[] = { 0x40, 0x05, 0xbf, 0x0a,
                                    0x8b, 0x14, 0x57, 0x69, };
    tp.type = SUNS_FLOAT64;
    suns_buf_to_value(float64_buf, &tp, v);

    if (suns_value_get_float64(v) > 2.7182818284 && 
        suns_value_get_float64(v) < 2.7182818285) {
        debug("float64 passed");
        pass++;
    } else {
        debug("float64 failed");
    }        
    
    /* float64 not-implemented */
    total++;
    unsigned char float64_ni_buf[] = { 0x7F, 0xFF, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x01 };
    tp.type = SUNS_FLOAT64;
    suns_buf_to_value(float64_ni_buf, &tp, v);
    if (v->meta == SUNS_VALUE_NOT_IMPLEMENTED) {
        debug("float64 not-implemented passed");
        pass++;
    } else {
        debug("float64 not-implemented failed");
    }

    debug("pass = %d, total = %d", pass, total);

    return pass - total;
}

int unit_test_snprintf_suns_value_t(const char **name)
{
    *name = __FUNCTION__;

    int total = 0;
    int pass = 0;

    char buf[BUFFER_SIZE];
    suns_value_t *v = suns_value_new();

    /* check initialized values */
    total++;
    if (v->meta == SUNS_VALUE_NULL) {
        debug("v is initialized to SUNS_NULL");
        pass++;
    } else {
        debug("v->meta != SUNS_NULL");
    }

    total++;
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "null") == 0) {
        debug("null test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("null test failed (buf = %s)", buf);
    }

    /* uint16 */
    total++;
    suns_value_set_uint16(v, 12345);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "12345") == 0) {
        debug("uint16 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("uint16 test failed (buf = %s)", buf);
    }

    /* int16 */
    total++;
    suns_value_set_int16(v, -12345);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "-12345") == 0) {
        debug("int16 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("int16 test failed (buf = %s)", buf);
    }

    /* uint32 */
    total++;
    suns_value_set_uint32(v, 12345678);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "12345678") == 0) {
        debug("uint32 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("uint32 test failed (buf = %s)", buf);
    }

    /* int32 */
    total++;
    suns_value_set_int32(v, -12345678);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "-12345678") == 0) {
        debug("int32 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("int32 test failed (buf = %s)", buf);
    }

    /* float32 */
    total++;
    suns_value_set_float32(v, 9.87654321);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    /* output truncates to 7 digits by default */
    if (strcmp(buf, "9.876543") == 0) { 
        debug("float32 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("float32 test failed (buf = %s)", buf);
    }

    /* enum16 */
    total++;
    suns_value_set_enum16(v, 42);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "42") == 0) {
        debug("enum16 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("enum16 test failed (buf = %s)", buf);
    }

    /* enum32 */
    total++;
    suns_value_set_enum32(v, 70000);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "70000") == 0) {
        debug("enum32 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("enum32 test failed (buf = %s)", buf);
    }

    /* bitfield16 */
    total++;
    suns_value_set_bitfield16(v, 0xcd);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "0x00cd") == 0) {
        debug("bitfield16 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("bitfield16 test failed (buf = %s)", buf);
    }

    /* bitfield32 */
    total++;
    suns_value_set_bitfield32(v, 0xabcd);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "0x0000abcd") == 0) {
        debug("bitfield32 test passed (buf = %s)", buf);
        pass++;
    } else {
        debug("bitfield32 test failed (buf = %s)", buf);
    }

    /* string */
    total++;
    suns_value_set_string(v, "pan galactic gargle blaster", 32);
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "pan galactic gargle blaster") == 0) {
        debug("string test passed (buf = \"%s\")", buf);
        pass++;
    } else {
        debug("string test failed (buf = \"%s\")", buf);
    }

    /* other meta values */
    total++;
    v->meta = SUNS_VALUE_NOT_IMPLEMENTED;
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "not implemented") == 0) {
        debug("SUNS_VALUE_NOT_IMPLEMENTED test passed (buf = \"%s\")", buf);
        pass++;
    } else {
        debug("SUNS_VALUE_NOT_IMPLEMENTED test failed (buf = \"%s\")", buf);
    }
        
    total++;
    v->meta = SUNS_VALUE_ERROR;
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "error") == 0) {
        debug("SUNS_VALUE_ERROR test passed (buf = \"%s\")", buf);
        pass++;
    } else {
        debug("SUNS_VALUE_ERROR test failed (buf = \"%s\")", buf);
    }
    
    total++;
    v->meta = SUNS_VALUE_UNDEF;
    suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (strcmp(buf, "undef") == 0) {
        debug("SUNS_VALUE_UNDEF test passed (buf = \"%s\")", buf);
        pass++;
    } else {
        debug("SUNS_VALUE_UNDEF test failed (buf = \"%s\")", buf);
    }
    

    return pass - total;
}

int unit_test_type_name_conversion(const char **name)
{
    *name = __FUNCTION__;

    suns_type_t t;
    char buf[BUFFER_SIZE];

    /* SUNS_NULL should be the smallest */
    if (SUNS_NULL != 0) {
        debug("SUNS_NULL != 0");
        return -1;
    }

    for (t = SUNS_NULL; t <= SUNS_UNDEF; t++) {
        char *str = suns_type_string(t);
        
        if (str == NULL) {
            debug("suns_type_string(%d) returned NULL", t);
            return -1;
        }

        /* write the type name string to a buffer */
        strncpy(buf, str, BUFFER_SIZE);
        
        /* now convert it back */
        if (suns_type_from_name(buf) != t) {
            debug("suns_type_from_name(%s) != %d", buf, t);
            return -1;
        }
    }

    return 0;
}


int unit_test_suns_value_meta_string(const char **name)
{
    *name = __FUNCTION__;

    int i;

    typedef struct suns_meta_map {
        suns_value_meta_t meta;
        char *string;
    } suns_meta_map_t;

    suns_meta_map_t map[] = {
        { SUNS_VALUE_NULL,                 "null" },
        { SUNS_VALUE_OK,                   "ok" },
        { SUNS_VALUE_NOT_IMPLEMENTED,      "not implemented" },
        { SUNS_VALUE_ERROR,                "error" },
        { SUNS_VALUE_UNDEF,                "undef" },
        { -1,                              NULL }
    };

    for (i = 0; map[i].meta > -1; i++) {
        if (strcmp(map[i].string, suns_value_meta_string(map[i].meta)) != 0) {
            debug("suns_value_meta_string(%d) != \"%s\"",
                  map[i].meta, map[i].string);
            return -1;
        }
    }

    if (suns_value_meta_string(SUNS_VALUE_UNDEF + 1) != NULL) {
        debug("suns_vale_meta_string() did not return NULL for invalid meta value");
        return -1;
    }

    return 0;
}


int unit_test_suns_type_size(const char **name)
{
    *name = __FUNCTION__;
    
    int i;
    
    typedef struct suns_size_map {
        size_t size;
        suns_type_t type;
    } suns_size_map_t;

    suns_size_map_t size_map[] = {
        { 0,     SUNS_NULL },
        { 2,     SUNS_INT16 },
        { 2,     SUNS_UINT16 },
        { 2,     SUNS_ACC16 },
        { 4,     SUNS_INT32 },
        { 4,     SUNS_UINT32 },
        { 4,     SUNS_FLOAT32 },
        { 4,     SUNS_ACC32 },
        { 8,     SUNS_INT64 },
        { 8,     SUNS_UINT64 },
        { 8,     SUNS_FLOAT64 },
        { 2,     SUNS_ENUM16 },
        { 4,     SUNS_ENUM32 },
        { 2,     SUNS_BITFIELD16 },
        { 4,     SUNS_BITFIELD32 },
        { 2,     SUNS_SF },
        { 0,     SUNS_STRING },
        { 0,     SUNS_UNDEF },
        { -1,         -1 },
    };

    for (i = 0; size_map[i].size != -1; i++) {
        debug("testing type %d (%s)", size_map[i].type, suns_type_string(size_map[i].type));
        if (suns_type_size(size_map[i].type) == size_map[i].size) {
            debug("suns_type_size(%s) == %d passed",
                  suns_type_string(size_map[i].type),
                  suns_type_size(size_map[i].type));
        } else {
            debug("suns_type_size(%s) == %d FAIL", 
                  suns_type_string(size_map[i].type),
                  suns_type_size(size_map[i].type));
            return -1;
        }
    }

    if (suns_type_size(SUNS_UNDEF + 1) >= 0) {
        debug("suns_type_size() did not return <= 0 for invalid type");
        return -1;
    }

    return 0;
}


