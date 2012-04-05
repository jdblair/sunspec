/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "ezxml/ezxml.h"
#include "trx/macros.h"
#include "trx/list.h"
#include "suns_parser.h"
#include "suns_host.h"
#include "suns_host_parser.h"
#include "suns_output_sqlite.h"

int main (int argc, char *argv[])
{
    int rc = 0;
    char *result_xml;

    verbose_level = 3;
    
    sqlite3 *db;
    const char *err;
    suns_parser_init();

    suns_parse_model_file("../models/device.model");

    if (suns_output_sqlite_open("store.db", &db, &err) < 0) {
        error("sqlite: %s", err);
        return 1;
    }

    /* suns_output_sqlite_init_db(db, &err); */

    list_t *devices = list_new();
    suns_host_result_t *result = suns_host_result_new();

    rc = suns_host_parse_logger_xml(stdin, devices, result);

    if (rc < 0) {
        debug("rc = %d\n", rc);
    }
    
    list_node_t *c;
    suns_device_t *d;
    list_for_each(devices, c) {
        d = c->data;
        /*        sqlite3_int64 rowid; */
        rc = suns_output_sqlite_device(db, d, &err);
        debug_i(rc);
    }
                                  

    if ((rc = suns_host_result_xml(result, &result_xml)) >= 0) {
        fwrite(result_xml, 1, strlen(result_xml), stdout);
    } else {
        debug("rc = %d\n", rc);
    }
    
    suns_output_sqlite_close(db, &err);

    return 0;
}

