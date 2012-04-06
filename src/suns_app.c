/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_app.c
 *
 * This implements the command-line UI to the features in the test app.
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h> 
#include <modbus.h>
#include <errno.h>
#include <endian.h>
#include <getopt.h>
#include <dirent.h>

#include "trx/macros.h"
#include "trx/debug.h"
#include "suns_app.h"
#include "suns_model.h"
#include "suns_output.h"
#include "suns_parser.h"
#include "suns_lang.tab.h"
#include "suns_host.h"
#include "suns_host_parser.h"


void suns_app_init(suns_app_t *app)
{
    memset(app, 0, sizeof(suns_app_t));
    
    app->baud = 9600;
    app->serial_port = "/dev/ttyUSB0";
    app->hostname = "127.0.0.1";
    app->tcp_port = 502;
    app->test_server = 0;
    app->logger_host = 0;
    app->transport = SUNS_TCP;
    app->run_mainloop = 1;
    app->addr = 1;
    app->export_fmt = NULL;
    app->output_fmt = "text";
    app->lid = NULL;
    app->timeout = 2000;
    app->max_modbus_read = 125;  /* max defined in the modbus spec */
    app->retries = 2;
    app->override_model_searchpath = 0;

    /* override model_searchpath with SUNS_MODELPATH_ENV if it is set */
    if ((app->model_searchpath = getenv(SUNS_MODELPATH_ENV)) == NULL)
        app->model_searchpath = SUNS_MODELPATH;
}


/* parse command-line options */
int suns_app_getopt(int argc, char *argv[], suns_app_t *app)
{
    int opt;
    float timeout_tmp;

    /* option_error is used to signal that some invalid combination of
       arguments has been used.  if option_error is non-zero getopt()
       will exit the application after trying to parse all options */

    int option_error = 0;

    /* FIXME: add long options */

    while ((opt = getopt(argc, argv, "t:i:P:p:b:M:m:o:sx:va:I:l:X:T:r:M:hH"))
           != -1) {
        switch (opt) {
        case 't':
            if (strcasecmp(optarg, "tcp") == 0) {
                app->transport = SUNS_TCP;
                break;
            }
            if (strcasecmp(optarg, "rtu") == 0) {
                app->transport = SUNS_RTU;
                break;
            }
            error("unknown transport type: %s, must choose \"tcp\" or \"rtu\"",
                  optarg);
            option_error = 1;
            break;
            
        case 'i':
            app->hostname = optarg;
            break;
            
        case 'P':
            if (sscanf(optarg, "%d", &(app->tcp_port)) != 1) {
                error("unknown port number format: %s, "
                      "must provide decimal number", optarg);
                option_error = 1;
            }
            app->transport = SUNS_TCP;
            break;
            
        case 'p':
            app->serial_port = optarg;
            app->transport = SUNS_RTU;
            break;
            
        case 'b':
            if (sscanf(optarg, "%d", &(app->baud)) != 1) {
                error("unknown baud rate format: %s, "
                      "must provide decimal number", optarg);
                option_error = 1;
            }
            break;
            
        case 'm':
            /* keep running even if there are parsing errors */
            verbose(1, "parsing model file %s", optarg);
            app->override_model_searchpath = 1;
            (void) suns_parse_model_file(optarg);
            break;

        case 'X': 
            /* keep running even if there are parsing errors */
            verbose(1, "parsing xml model file %s", optarg);
            app->override_model_searchpath = 1;
            suns_parse_xml_model_file(optarg);
            break;
            
        case 's':
            app->test_server = 1;
            break;
            
        case 'v':
            verbose_level++;
            break;

        case 'a':
            if (sscanf(optarg, "%d", &(app->addr)) != 1) {
                error("must provide decimal number modbus address");
                option_error = 1;
            }
            break;

        case 'x':
            app->export_fmt = optarg;
            break;

        case 'o':
            app->output_fmt = optarg;
            break;

        case 'I':
            app->lid = optarg;
            break;

        case 'N':
            app->ns = optarg;
            break;

        case 'l':
            if (sscanf(optarg, "%d", &(app->max_modbus_read)) != 1) {
                error("must provide decimal max modbus read length");
                option_error = 1;
            }
            break;

        case 'T':
            if (sscanf(optarg, "%f", &timeout_tmp) != 1) {
                error("can't parse provided timeout: %s", optarg);
                option_error = 1;
            }
            app->timeout = timeout_tmp * 1000;
            break;

        case 'r':
            if (sscanf(optarg, "%d", &(app->retries)) != 1) {
                error("must provide decimal number of retries");
                option_error = 1;
            }
            break;

        case 'M':
            suns_app_model_search_dir(app, optarg);
            break;

        case 'h':
            suns_app_help(argc, argv);
            exit(EXIT_SUCCESS);
            break;

        case 'H':
            app->logger_host = 1;
            break;

        default:
            suns_app_help(argc, argv);
            exit(EXIT_SUCCESS);
        }
    }    
    /* always force localhost for server mode */
    if (app->test_server) {
        debug("forcing hostname to localhost (127.0.0.1)");
        app->hostname = "127.0.0.1";
    }

    /* bail if we hit an error */
    if (option_error) {
        exit(EXIT_FAILURE);
    }

    return 0;
}


void suns_app_help(int argc, char *argv[])
{
    printf("Usage: %s: \n", argv[0]);
    printf("      -o: output mode for data (text, csv, sql)\n");
    printf("      -x: export model description (slang, csv, sql)\n");
    printf("      -t: transport type: tcp or rtu (default: tcp)\n");
    printf("      -a: modbus slave address (default: 1)\n");
    printf("      -i: ip address to use for modbus tcp "
           "(default: localhost)\n");
    printf("      -P: port number for modbus tcp (default: 502)\n");
    printf("      -p: serial port for modbus rtu (default: /dev/ttyUSB0)\n");
    printf("      -b: baud rate for modbus rtu (default: 9600)\n");
    printf("      -T: timeout, in seconds (can be fractional, such as 1.5; default: 2.0)\n");
    printf("      -r: number of retries attempted for each modbus read\n");
    printf("      -m: specify model file\n");
    printf("      -M: specify directory containing model files\n");
    printf("      -s: run as a test server\n");
    printf("      -I: logger id (for sunspec logger xml output)\n");
    printf("      -N: logger id namespace (for sunspec logger xml output, defaults to 'mac')\n");
    printf("      -v: verbose level (up to -vvvv for most verbose)\n");
    printf("\n");
}


int suns_app_logger_host(suns_app_t *app)
{
    list_t *devices = list_new();
    suns_host_result_t *result = suns_host_result_new();
    char *result_xml;
    int rc = 0;

    rc = suns_host_parse_logger_xml(stdin, devices, result);
    debug("rc = %d", rc);

    rc = suns_host_result_xml(result, &result_xml);

    fwrite(result_xml, 1, strlen(result_xml), stdout);
    free(result_xml);

    /*
    list_node_t *c;
    list_for_each(devices, c) {
        suns_device_output(app->output_fmt, c->data, stdout);
        }
    */


    debug("rc = %d", rc);

    return rc;
}


int suns_app_test_server(suns_app_t *app)
{
    modbus_mapping_t *mapping;
    int header_length;
    int offset = 0;
    list_node_t *c;
    int socket;
    int rc = 0;
    uint8_t *q;
    suns_parser_state_t *parser = suns_get_parser_state();

    header_length = modbus_get_header_length(app->mb_ctx);

    if (app->transport == SUNS_TCP) {
        q = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    } else {
        q = malloc(MODBUS_RTU_MAX_ADU_LENGTH);
    }

    /* FIXME: should not use hard-coded mapping size */
    mapping = modbus_mapping_new(0, 0, 4096, 4096);
    
    if (mapping == NULL) {
        error("failed to allocate mapping: %s",
              modbus_strerror(errno));
        return -1;
        modbus_free(app->mb_ctx);
    }

    /* put sunspec id in the first 2 registers */
    mapping->tab_registers[offset] = SUNS_ID_HIGH;
    mapping->tab_registers[offset+1] = SUNS_ID_LOW;
    /* duplicate in the input registers space also */
    mapping->tab_input_registers[offset] = SUNS_ID_HIGH;
    mapping->tab_input_registers[offset+1] = SUNS_ID_LOW;
    
    offset += 2;

    /* add the data blocks in the order they are read in
       note that this means the common block data must be
       defined first */

    list_for_each(parser->data_block_list, c) {
        suns_data_block_t *dblock = c->data;
        debug("copying data block \"%s\" to register map "
              "starting at offset %d", dblock->name, offset);
    
        /* gross hack!

           libmodbus stores registers in host byte order, but
           we store our test data in modbus (be) byte order.
           this means we have to convert the data.  what a pita!
        */
           
        int r;  /* register offset, starting at zero */
        for (r = 0; r < buffer_len(dblock->data); r += 2) {
            mapping->tab_registers[offset] =
                ((uint8_t) buffer_data(dblock->data)[r] << 8) +
                ((uint8_t) buffer_data(dblock->data)[r + 1]);

            mapping->tab_input_registers[offset] =
                ((uint8_t) buffer_data(dblock->data)[r] << 8) +
                ((uint8_t) buffer_data(dblock->data)[r + 1]);

            verbose(4, "mapping->tab_registers[%d] = %04x",
                    offset, mapping->tab_registers[offset]);
                  
            offset++;
        }
    }

    /* tack on the end marker */
    mapping->tab_registers[offset] = 0xFFFF;
    mapping->tab_input_registers[offset] = 0xFFFF;

    /* start listening if we are a modbus tcp server */
    if (app->transport == SUNS_TCP) {
        socket = modbus_tcp_listen(app->mb_ctx, 1);
        if (socket < 0) {
            error("modbus_tcp_listen() returned %d: %s",
                  socket, modbus_strerror(errno));
            return rc;
        }
    }
    
    while (1) {
        /* wait for connection if modbus tcp */
        if (app->transport == SUNS_TCP) {
            rc = modbus_tcp_accept(app->mb_ctx, &socket);
            if (rc < 0) {
                error("modbus_tcp_accept() returned %d: %s",
                      rc, modbus_strerror(errno));
                return rc;
            }
        }

        /* loop forever, servicing client requests */
        while (1) {
            debug("top of loop");
            
            rc = modbus_receive(app->mb_ctx, q);
            if (rc < 0) {
                debug("modbus_receive() returned %d: %s",
                      rc, modbus_strerror(errno));
                break;
            }

            /* the libmodbus machinery will service the
               request */
            rc = modbus_reply(app->mb_ctx, q, rc, mapping);
            if (rc < 0) {
                debug("modbus_reply() returned %d: %s",
                      rc, modbus_strerror(errno));
                break;
            }
        }
    }

    debug("exited main loop");

    if (app->transport == SUNS_TCP) {
        close(socket);
    }
    modbus_mapping_free(mapping);
    free(q);
    modbus_free(app->mb_ctx);

    return 0;
}


int suns_init_modbus(suns_app_t *app)
{
    int rc = 0;
    struct timeval timeout;

    if (app->transport == SUNS_TCP) {
        debug("modbus tcp mode: %s:%d", app->hostname, app->tcp_port);
        app->mb_ctx = modbus_new_tcp(app->hostname, app->tcp_port);
    } else if (app->transport == SUNS_RTU) {
        debug("modbus rtu mode");
        /* note we don't let the user set the parity,
           byte length or stop bit */
        app->mb_ctx = modbus_new_rtu(app->serial_port, app->baud,
                                     'N', 8, 1);
    } else {
        error("invalid transport type: %d", app->transport);
        exit(EXIT_FAILURE);
    }

    if (app->mb_ctx == NULL) {
        error("cannot initialize modbus context: %s",
              modbus_strerror(errno));
        return -1;
    }

    if (verbose_level > 3) {
        debug("setting libmodbus debug mode = 1");
        modbus_set_debug(app->mb_ctx, 1);
    }

    rc = modbus_set_slave(app->mb_ctx, app->addr);
    if (rc < 0) {
        debug("modbus_set_slave() failed: %s",
              modbus_strerror(errno));
    }

    /* this something else a bit tricky about libmodbus
       we need to call modbus_connect() for RTU mode on both the
       master and the slave.  However, in TCP mode we get an error
       if we call modbus_connect() on the slave (test server) side. */
    if (app->transport == SUNS_RTU ||
        (app->transport == SUNS_TCP && ! app->test_server)) {
        rc = modbus_connect(app->mb_ctx);
        if (rc < 0) {
            error("modbus_connect() returned %d: %s",
                  rc, modbus_strerror(errno));
            return rc;
        }
    }

    /* modbus_get_response_timeout(app->mb_ctx, &timeout); */

    /* set timeout */
    debug("app->timeout = %d", app->timeout);
    timeout.tv_sec = app->timeout / 1000;
    timeout.tv_usec = (app->timeout % 1000) * 1000;
    modbus_set_response_timeout(app->mb_ctx, &timeout);

    debug("timeout.tv_sec = %d", (int) timeout.tv_sec);
    debug("timeout.tv_usec = %d", (int) timeout.tv_usec);

    return 0;
}


/* FIXME: here lies a gross hack

   the purpose of this function is to copy data from an array of
   uint16_t and put it into an array of unsigned char in big-endian
   byte order

   libmodbus presents retrieved registers in an array of uint16_t in
   host byte order.  on x86, which is little ending, this means
   they've been swapped from the modbus big-endian on-the-wire format.

   this wouldn't be such a big deal if we were only reading 16 bit
   integers, but sunspec is composed of other structured data types,
   and the rest of the code i've already developed assumes it is
   reading an un-tampered buffer representing data pulled straight off
   the wire.

   my plan is to eliminate this by submitting a patch to libmodbus
   which fixes the issue, or by changing the assumptions i make about
   the data i'm decoding so words, not bytes are the smallest data
   unit.
*/
   
int suns_app_swap_registers(uint16_t *reg,
                            int num_regs,
                            unsigned char *buf)
{
    int i;

    for (i = 0; i < num_regs; i++) {
        ((uint16_t*)buf)[i] = htobe16(reg[i]);
    }

    return 0;
}



int suns_app_read_device(suns_app_t *app, suns_device_t *device)
{
    int rc = 0;
    int i;
    uint16_t regs[1024];
    unsigned char buf[2048];
    suns_model_did_t *did;
    int offset = 0;
    uint16_t len;
    suns_dataset_t *data;  /* holds decoded datapoints */

    /* we need the parser state to gain access to the data model definitions */
    suns_parser_state_t *sps = suns_get_parser_state();    

    /* datasets (aka models) are indexed in case more than one of the 
       same kind occurs on a single device */
    int dataset_index = 1;  /* start numbering at 1 */

    /* places to look for the sunspec signature */
    int search_registers[] = { 1, 40001, 50001, 0x40001, -1 };
    int base_register = -1;

    /* look for sunspec signature */
    for (i = 0; search_registers[i] >= 0; i++) {
        int retries;
        rc = -1;
        for (retries = 0; retries < app->retries && rc < 0; retries++) {
            /* libmodbus uses zero as the base address */
            debug("read register %d, try %d", search_registers[i], retries);
            rc = modbus_read_registers(app->mb_ctx, search_registers[i] - 1,
                                       2, regs);
        }

        if (rc < 0) {
            debug("modbus_read_registers() returned %d: %s",
                  rc, modbus_strerror(errno));
            error("modbus_read_registers() failed: ");
            error("register %d on address %d",
                  search_registers[i], app->addr);
        } else if ( (regs[0] == SUNS_ID_HIGH) &&
                    (regs[1] == SUNS_ID_LOW) ) {
            base_register = search_registers[i];
            verbose(1, "found sunspec signature at register %d",
                    base_register);
            break;
        }
    }

    /* check if we found the sunspec block */
    if (base_register < 0) {
        error("sunspec block not found on device");
        return -1;
    }

    if (base_register == 0x40001)
        error("sunspec block found at 0x40001, not decimal 40001!");
    
    offset = 2;
    
    /* loop over all data models as they are discovered */
    while (1) {
        /* read the next 2 registers to get the did and the length */
        debug("looking for sunspec data block at %d",
              base_register + offset);

        int retries;
        rc = -1;
        for (retries = 0; retries < app->retries && rc < 0; retries++) { 
            debug("read register %d, try %d", base_register + offset, retries);
            rc = modbus_read_registers(app->mb_ctx,
                                       base_register + offset - 1,
                                       2, regs);
        }
            
        if (rc < 0) {
            debug("modbus_read_registers() returned %d: %s",
                  rc, modbus_strerror(errno));
            error("modbus_read_registers() failed: register %d on address %d",
                  base_register + offset, app->addr);
            rc = -1;
            break;
        }

        /* did we stumble upon an end marker? */
        if ((regs[0] == 0xFFFF) ||
            (regs[1] == 0x0000)) {
            verbose(1, "found end marker at register %d and %d",
                    base_register + offset - 1, base_register + offset);
            rc = 0;
            break;
        }

        /* since we're a test program we need to check for a missing
           end marker.  all we can really do is check for zero, since
           we can't tell the difference between a did we don't know
           and some other data. */
        if (regs[0] == 0) {
            verbose(1, "found 0x0000 where we should have found "
                    "an end marker or another did.");
            rc = -1;
            break;
        }

        len = regs[1];
        verbose(1, "found did = %d, len = %d", regs[0], len);
        did = suns_find_did(sps->did_list, regs[0]);
        
        if (did == NULL) {
            warning("unknown did: %d", regs[0]);
            /* skip ahead the reported length */
            /* offset += len + 2; */
        } else {
            /* we found a did we know about */
            
            /* is the length what we expect? */
            /* check out this pointer indirection!! */
            suns_dp_block_t *last_dp_block = did->model->dp_blocks->tail->data;
            if ((did->model->len != len) &&
                (last_dp_block->repeating &&
                 (((len - did->model->base_len) %
                   (did->model->len - did->model->base_len)) != 0))) {
                error("data model length %d does not match expected length %d",
                      len, did->model->len);
            }
        }

        rc = suns_app_read_registers(app, base_register + offset - 1,
                                   len + 2, regs);
        if (rc < 0) {
            debug("suns_app_read_registers() returned %d: %s",
                  rc, modbus_strerror(errno));
            rc = -1;
            break;
        }

        /* kludge around the way libmodbus works */
        suns_app_swap_registers(regs, len + 2, buf);

        /* dump the binary data in test model form */
        if (verbose_level > 2) {
            /* suns_binary_model_fprintf requires the length in bytes,
               not modbus registers */
            suns_binary_model_fprintf(stdout, sps->did_list,
                                      buf, ((len + 2) * 2));
        }

        /* if the did for this blob is known, decode it and
           attach it to the suns_device_t */
        if (did) {
            /* suns_decode_data() requires the length in bytes, not
               modbus registers */

            /* add 2 to len to include did & len registers */
            data = suns_decode_data(sps->did_list, buf, (len + 2) * 2);
            /* assign index */
            data->index = dataset_index;

            /* add the dataset to the device */
            suns_device_add_dataset(device, data);
        } else {
            /* unknown data block */
            if (verbose_level > 0) {
                dump_buffer(stdout, buf, (len + 2) * 2);
            }
        }
        
        /* jump ahead to next data block */
        offset += len + 2;

        dataset_index++;
    }

    return rc;
}



int suns_app_read_data_model(modbus_t *ctx)
{
    return 0;
}


int main(int argc, char **argv)
{
    suns_app_t app;
    list_node_t *c;

    /* to put our results */
    suns_device_t *device;

    /* global parser state */
    suns_parser_state_t *sps = suns_get_parser_state();

    /* initialize suns app global state */
    suns_app_init(&app);

    debug("app.timeout = %d", app.timeout);

    /* initialize parser globals */
    suns_parser_init();

    /* parse options */
    /* this has the side effect of parsing any specified model files */
    suns_app_getopt(argc, argv, &app);

    /* now load models found in the model searchpath */
    /* ignore errors */
    if (! app.override_model_searchpath)
        suns_app_model_search_path(&app, app.model_searchpath);

    /* check if we've parsed any models */
    if ((list_count(sps->model_list) <= 0) &&
        (list_count(sps->data_block_list) <= 0)) {
        error("No models or data defines were parsed.");
        error("Check your model search path or specify a model file explicitly.");
        error("use -D, -m or the SUNS_MODELPATH environment variable.");
        error("model searchpath: %s", app.model_searchpath);
        exit(EXIT_FAILURE);
    }

    /* fill in offset data in any parsed model files */
    /* maybe this should be part of suns_parser.c? */
    list_for_each(sps->model_list, c) {
        suns_model_fill_offsets(c->data);
    }

    /* display options in debug mode */
    if (app.transport == SUNS_TCP) {
        debug("transport: TCP");
    } else if (app.transport == SUNS_RTU) {
        debug("transport: RTU");
    } else {
        debug("unknown transport: %d", app.transport);
    }

    debug("hostname: %s", app.hostname);
    debug("tcp_port: %d", app.tcp_port);
    debug("serial_port: %s", app.serial_port);
    debug("baud: %d", app.baud);
    debug("test_server: %d", app.test_server);
    debug("export_fmt: %s", app.export_fmt);
    debug("retries: %d", app.retries);
    debug("timeout: %d", app.timeout);
    debug("model_searchpath: %s", app.model_searchpath);
    
    /* are we invoked in model export mode? */
    if (app.export_fmt != NULL) {
        suns_model_export_all(stdout, app.export_fmt, sps->model_list);
        exit(EXIT_SUCCESS);
    }

    /* are we invoked in host logger xml parse mode? */
    if (app.logger_host) {
        if (suns_app_logger_host(&app) < 0)
            exit(EXIT_FAILURE);
        else
            exit(EXIT_SUCCESS);
    }

    /* initialize the modbus layer (same for server and client) */
    suns_init_modbus(&app);

    /* run server / slave */
    if (app.test_server) {
        debug("test server mode - acting as modbus slave");
        suns_app_test_server(&app);
    } else {
        /* run client / master */
        debug("suns client (master) mode");
        device = suns_device_new();

        if (device == NULL) {
            error("memory error: suns_device_new() failed");
            exit(EXIT_FAILURE);
        }
        
        device->lid = app.lid;
        device->ns = app.ns;
                  
        if (suns_app_read_device(&app, device) < 0) {
            error("failure to read device");
            exit(EXIT_FAILURE);
            suns_device_free(device);
        }
        
        suns_device_output(app.output_fmt, device, stdout);
    }
    
    exit(EXIT_SUCCESS);
}



/**
 * This is a wrapper around modbus_read_registers() to allow us to read
 * blocks of registers larger than what is allowed in a single modbus
 * read.  A maximum of 125 registers may be read in a single read, per
 * the modbus application protocol spec.  The number of registers read
 * in a single modbus read can be set using app->max_modbus_read.
 *
 * \param app app context
 * \param start starting registers
 * \param len number of registers (not bytes)
 * \param regs buffer to write the results to
 *
 */
int suns_app_read_registers(suns_app_t *app,
                            int start,
                            int len,
                            uint16_t *regs)
{
    int rc = 0;
    int reg_offset = 0;
    
    while (reg_offset < len) {
        int read_len = min((len - reg_offset), app->max_modbus_read);
        
        debug("start = %d, read_len = %d, (len - reg_offset) = %d", start, read_len, (len - reg_offset));

        int retries;
        rc = -1;
        for (retries = 0; retries < app->retries && rc < 0; retries++) { 
            debug("read register %d, try %d", start + 1, retries);
            rc = modbus_read_registers(app->mb_ctx, start,
                                       read_len, regs + reg_offset);
        }

        if (rc < 0) {
            debug("modbus_read_registers() returned %d: %s",
                  rc, modbus_strerror(errno));
            error("modbus_read_registers() failed: register %d, "
                  "length %d on address %d",
                  start, read_len, app->addr);
            return -1;
        }

        reg_offset += read_len;
    }

    return rc;
}


int suns_app_model_search_path(suns_app_t *app, char const *path)
{
    int rc = 0;
    char *pathdup;
    char *saveptr = NULL;
    char *token;

    pathdup = strdup(path);

    token = strtok_r(pathdup, ":", &saveptr);
    while (token) {
        debug("token = %p, '%s'", token, token);
        rc = suns_app_model_search_dir(app, token);
        /* ignore errors - if a part of the search path is not
           accessible or a file can't be parsed we should
           just keep searching */
        if (rc < 0) {
            debug("suns_app_model_search_dir() returned %d: %m", rc);
        }
        token = strtok_r(NULL, ":", &saveptr);
    }

    free(pathdup);

    return rc;
}


int suns_app_model_search_dir(suns_app_t *app, char const *dirpath)
{
    int rc = 0;
    DIR *dir;
    struct dirent *entryp;

    dir = opendir(dirpath);
    if (dir == NULL) {
        debug("can't open dir %s: %m", dirpath);
        return -1;
    }

    entryp = readdir(dir);
    while (entryp) {
        int len;
        char *filename = entryp->d_name;

        len = strlen(filename);

        /* skip files that start . (directories and dot-files) */
        if (filename[0] == '.') {
            debug("skipping %s", filename);
            entryp = readdir(dir);
            continue;
        }

        debug("file: %s", filename);

        /* check for *.xml */
        if (((filename[len - 1] == 'l') || (filename[len - 1] == 'L')) &&
            ((filename[len - 2] == 'm') || (filename[len - 2] == 'M')) &&
            ((filename[len - 3] == 'x') || (filename[len - 3] == 'X')) &&
            (filename[len - 4] == '.')) {
            
            char *buf = malloc(strlen(dirpath) + strlen(filename) + 2);
            strcpy(buf, dirpath);
            strcat(buf, "/");
            strcat(buf, filename);
            debug("parsing model file %s", filename);
            if ((rc = suns_parse_xml_model_file(buf)) < 0)
                return rc;
            free(buf);
        } 

        if (
            /* .model */
            (((filename[len - 1] == 'l') || (filename[len - 1] == 'L')) &&
             ((filename[len - 2] == 'e') || (filename[len - 2] == 'E')) &&
             ((filename[len - 3] == 'd') || (filename[len - 3] == 'D')) &&
             ((filename[len - 4] == 'o') || (filename[len - 4] == 'O')) &&
             ((filename[len - 5] == 'm') || (filename[len - 5] == 'M')) &&
             (filename[len - 6] == '.')) ||
            /* .mdl */
            (((filename[len - 1] == 'l') || (filename[len - 3] == 'D')) &&
             ((filename[len - 2] == 'd') || (filename[len - 4] == 'O')) &&
             ((filename[len - 3] == 'm') || (filename[len - 5] == 'M')) &&
             (filename[len - 4] == '.'))
            ) {

            char *buf = malloc(strlen(dirpath) + strlen(filename) + 2);
            strcpy(buf, dirpath);
            strcat(buf, "/");
            strcat(buf, filename);
            debug("parsing model file %s", buf);
            if ((rc = suns_parse_model_file(buf)) < 0)
                return rc;
            free(buf);
        }
            
        entryp = readdir(dir);
    }

    closedir(dir);

    return rc;
}
