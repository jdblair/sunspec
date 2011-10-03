/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_app.h
 * $Id: $
 *
 * header file for the command-line UI to the features in the test app.
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

#include <modbus.h>

#include "suns_model.h"

typedef enum suns_transport {
    SUNS_TCP,
    SUNS_RTU
} suns_transport_t;

typedef struct suns_app {
    suns_transport_t transport;
    char *hostname;
    int tcp_port;
    char *serial_port;
    int baud;
    int test_server;
    modbus_t *mb_ctx;  /* libmodbus context */
    int run_mainloop;
    int addr;
    char *export_fmt;  /* model export format */
    char *output_fmt;  /* dataset output format */
    char *logger_id;   /* logger id for sunspec data posts */
    char *namespace;   /* logger id namespace for sunspec data posts */
} suns_app_t;


int suns_app_getopt(int argc, char *argv[], suns_app_t *app);
void suns_app_init(suns_app_t *app);
int suns_app_getopt(int argc, char *argv[], suns_app_t *app);
int suns_app_test_server(suns_app_t *app);
int suns_init_modbus(suns_app_t *app);
int suns_app_swap_registers(uint16_t *reg,
                            int num_regs,
                            unsigned char *buf);
int suns_app_read_device(suns_app_t *app, suns_device_t *device);
int suns_app_read_data_model(modbus_t *ctx);
void suns_app_help(int argc, char *argv[]);
