/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output_sqlite.c
 *
 * Copyright (c) 2012, John D. Blair <jdb@moship.net>
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
#include <sqlite3.h>
#include <string.h>
 
#include "trx/list.h"
#include "trx/macros.h"
#include "trx/debug.h"
#include "suns_model.h"
#include "suns_read_sqlite.h"

int suns_read_sqlite_open(char *path, sqlite3 **db, const char **err)
{
    int rc = 0;

    debug_s(path);
    
    if (sqlite3_open_v2(path, db,
                        SQLITE_OPEN_READONLY,
                        NULL) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(*db));
        *err = sqlite3_errmsg(*db);
        return -1;
    }

    return rc;
}

/* read a single device from the database */
int suns_read_sqlite_device(sqlite3 *db,
                            suns_read_sqlite_device_callback_f callback,
                            void *ptr, const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    const char *pzTail;
    sqlite3_int64 rowid;

    suns_device_t *d = suns_device_new();

    const char *sql = "SELECT rowid, unixtime, usec, cid, id, iface, man, mod, ns, sn FROM device ORDER BY unixtime ASC;";

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
        SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
    
    rowid =             sqlite3_column_int64(stmt, 1);
    d->unixtime =       sqlite3_column_int(stmt, 2);
    d->usec =           sqlite3_column_int(stmt, 3);
    d->cid =            sqlite3_column_text(stmt, 4);
    d->id =             sqlite3_column_text(stmt, 5);
    d->iface =          sqlite3_column_text(stmt, 6);
    d->lid =            sqlite3_column_text(stmt, 7);
    d->ns =             sqlite3_column_text(stmt, 8);
    d->serial_number =  sqlite3_column_text(stmt, 9);

    debug_li(rowid);
    debug_i(d->unixtime);
    debug_i(d->usec);
    debug_s(d->cid);
    debug_s(d->id);
    debug_s(d->iface);
    debug_s(d->lid);
    debug_s(d->ns);
    debug_s(d->serial_number);

    /* here we select all datasets where device = rowid */
    /* for each dataset we select all values where dataset = dataset's rowid */

 finalize:
    (void) sqlite3_finalize(stmt);

    return rc;
}
