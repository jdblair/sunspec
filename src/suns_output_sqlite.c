/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

/*
 * suns_output_sqlite.c
 *
 * Copyright (c) 2011-2012, John D. Blair <jdb@moship.net>
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
#include "suns_parser.h"
#include "suns_output.h"
#include "suns_output_sqlite.h"


int suns_output_sqlite_open(char *path, sqlite3 **db, const char **err)
{
    int rc = 0;

    debug_s(path);
    
    if (sqlite3_open_v2(path, db,
                        SQLITE_OPEN_READWRITE,
                        NULL) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(*db));
        *err = sqlite3_errmsg(*db);
        return -1;
    }

    return rc;
}


int suns_output_sqlite_close(sqlite3 *db, const char **err)
{
    int rc = 0;

    if (sqlite3_close(db) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        *err = sqlite3_errmsg(db);
        return -1;
    }

    return rc;
}


int suns_output_sqlite_model_did(sqlite3 *db,
                                 suns_model_did_t *did,
                                 const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    const char *pzTail;
    
    debug_i(did->did);
    
    const char *sql = "INSERT INTO model (did, name) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
        SQLITE_OK) { 
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    if (sqlite3_bind_int(stmt, 1, did->did) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
    
    if (sqlite3_bind_text(stmt, 2, did->model->name,
                          strlen(did->model->name), SQLITE_TRANSIENT) !=
        SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
    
 finalize:
    if (rc < 0)
        *err = sqlite3_errmsg(db);

    (void) sqlite3_finalize(stmt);  /* ignore return */
    
    return rc;
}


int suns_output_sqlite_model_list(sqlite3 *db,
                                  list_t *did_list,
                                  const char **err)
{
    int rc = 0;
    list_node_t *d;

    *err = NULL;

    debug_i(list_count(did_list));
    
        
    list_for_each(did_list, d) {
        if ((rc = suns_output_sqlite_model_did(db, d->data, err)) < 0)
            break;
    }
    
    return rc;
}


int suns_output_sqlite_types(sqlite3 *db, const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    suns_type_t t;
    const char *pzTail;

    for (t = SUNS_NULL; t != SUNS_UNDEF; t++) {
        debug("storing %s of size %d", suns_type_string(t), suns_type_size(t));

        const char *sql = "INSERT INTO type (name, size) VALUES (?, ?);";
        if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
            SQLITE_OK) { 
            debug_s(sqlite3_errmsg(db));
            rc = -1;
            goto finalize;
        } 
       
        if (sqlite3_bind_text(stmt, 1, suns_type_string(t),
                              strlen(suns_type_string(t)), SQLITE_STATIC) !=
            SQLITE_OK) {
            debug_s(sqlite3_errmsg(db));
            rc = -1;
            goto finalize;
        }
        
        if (sqlite3_bind_int(stmt, 2, suns_type_size(t)) != SQLITE_OK) {
            debug_s(sqlite3_errmsg(db));
            rc = -1;
            goto finalize;
        }
            
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            debug_s(sqlite3_errmsg(db));
            rc = -1;
            goto finalize;
        }
        
    finalize:
        (void) sqlite3_finalize(stmt);  /* ignore return */
    }

    return rc;
}


int suns_output_sqlite_init_db(sqlite3 *db, const char **err)
{
    int rc = 0;
    
    if (suns_output_sqlite_model_list(db, suns_get_did_list(), err) < 0) {
        rc = -1;
        goto finish;
    }

    if (suns_output_sqlite_types(db, err) < 0) {
        rc = -1;
        goto finish;
    }

 finish:
    if (rc < 0)
        error("sqlite: %s", *err);

    return rc;
}
                               


static int bind_text_or_null(sqlite3_stmt *stmt, int col, char *text)
{
    if (text != NULL)
        return sqlite3_bind_text(stmt, col, text, strlen(text),
                                 SQLITE_TRANSIENT);

    /* text is a NULL pointer */
    return sqlite3_bind_null(stmt, col);
}


int suns_output_sqlite_device(sqlite3 *db,
                              suns_device_t *d,
                              const char **err)
{
    int rc = 0;
    sqlite3_int64 device_rowid;
    sqlite3_stmt *stmt;
    const char *pzTail;

    /* all of the inserts need to be performed as a single 
       transaction, or it performance will SUCK!  its also the
       right thing to do to maintain consistency */

    const char *begin = "BEGIN TRANSACTION;";
    if (sqlite3_prepare_v2(db, begin, strlen(begin), &stmt, &pzTail) !=
        SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* finalize "BEGIN TRANSACTION" */
    (void) sqlite3_finalize(stmt);  /* ignore return */    


    if ((rc = suns_output_sqlite_device_row(db, d, &device_rowid, err)) < 0)
        goto finalize;

    /* now store all datasets */
    list_node_t *c;
    list_for_each(d->datasets, c) {
        if ((rc = suns_output_sqlite_dataset(db, c->data, device_rowid, err))
            < 0)
            break;
    }

    const char *end = "END TRANSACTION;";
    if (sqlite3_prepare_v2(db, end, strlen(end), &stmt, &pzTail) !=
        SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

 finalize:
    (void) sqlite3_finalize(stmt);  /* ignore return */    

    return rc;
}
     

/**
 * Store a row in the device table, which contains all of the meta-data
 * stored in the suns_device_t struct.
 *
 */
int suns_output_sqlite_device_row(sqlite3 *db,
                                  suns_device_t *d,
                                  sqlite3_int64 *rowid,
                                  const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    const char *pzTail;

    const char *sql = 
        "INSERT INTO device "
        "(unixtime, usec, cid, id, iface, man, mod, ns, sn) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
        SQLITE_OK) { 
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    /* unixtime */
    if (d->unixtime == 0) {
        rc = sqlite3_bind_null(stmt, 1);
    } else {
        rc = sqlite3_bind_int(stmt, 1, d->unixtime);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* usec */
    if (d->usec == 0) {
        rc = sqlite3_bind_null(stmt, 2);
    } else {
        rc = sqlite3_bind_int(stmt, 2, d->usec);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* cid */
    if (bind_text_or_null(stmt, 3, d->cid) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 4, d->id) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 5, d->iface) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 6, d->manufacturer) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 7, d->model) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 8, d->ns) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (bind_text_or_null(stmt, 9, d->serial_number) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* stash the rowid */
    *rowid = sqlite3_last_insert_rowid(db);

 finalize:
    (void) sqlite3_finalize(stmt);  /* ignore return */

    return rc;
}



int suns_output_sqlite_dataset_row(sqlite3 *db,
                                   suns_dataset_t *ds,
                                   sqlite3_int64 device_rowid,
                                   sqlite3_int64 *rowid,
                                   const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    const char *pzTail;

    const char *sql = 
        "INSERT INTO dataset "
        "(model, unixtime, usec, ns, x, device) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
        SQLITE_OK) { 
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* model */
    debug_i(ds->did->did);
    if (sqlite3_bind_int(stmt, 1, ds->did->did) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* unixtime */
    if (ds->unixtime == 0) {
        rc = sqlite3_bind_null(stmt, 2);
    } else {
        rc = sqlite3_bind_int(stmt, 2, ds->unixtime);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* usec */
    if (ds->usec == 0) {
        rc = sqlite3_bind_null(stmt, 3);
    } else {
        rc = sqlite3_bind_int(stmt, 3, ds->usec);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* ns */
    if (bind_text_or_null(stmt, 4, ds->ns) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* x */
    if (bind_text_or_null(stmt, 5, ds->index) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* device */
    if (sqlite3_bind_int(stmt, 6, device_rowid) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
 finalize:
    (void) sqlite3_finalize(stmt);  /* ignore return */

    /* stash the rowid */
    *rowid = sqlite3_last_insert_rowid(db);
    debug("*rowid = %ld", (long int) *rowid);

    return rc;
}


int suns_output_sqlite_dataset(sqlite3 *db,
                               suns_dataset_t *ds,
                               sqlite3_int64 device_rowid,
                               const char **err)
{
    int rc = 0;
    sqlite3_int64 rowid;

    if ((rc = suns_output_sqlite_dataset_row(db, ds,
                                             device_rowid,
                                             &rowid, err)) < 0)
        return rc;

    list_node_t *c;
    list_for_each(ds->values, c) {
        if ((rc = suns_output_sqlite_value(db, c->data,
                                           ds->did->did, rowid, err)) < 0)
            break;
    }
    
    return rc;
}


int suns_output_sqlite_value(sqlite3 *db,
                             suns_value_t *v,
                             int did,
                             sqlite3_int64 dataset_rowid,
                             const char **err)
{
    int rc = 0;
    sqlite3_stmt *stmt;
    const char *pzTail;

    const char *sql = 
        "INSERT INTO value "
        "(unixtime, usec, name, type, v, sf, x, dataset) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pzTail) !=
        SQLITE_OK) { 
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* unixtime */
    if (v->unixtime == 0) {
        rc = sqlite3_bind_null(stmt, 1);
    } else {
        rc = sqlite3_bind_int(stmt, 1, v->unixtime);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* usec */
    if (v->usec == 0) {
        rc = sqlite3_bind_null(stmt, 2);
    } else {
        rc = sqlite3_bind_int(stmt, 2, v->usec);
    }
    if (rc != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* name */
    if (bind_text_or_null(stmt, 3, v->name) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* type */
    if (sqlite3_bind_int(stmt, 4, v->tp.type) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* v */
    char buf[BUFFER_SIZE];
    int len;
    len = suns_snprintf_value_text(buf, BUFFER_SIZE, v);
    if (sqlite3_bind_text(stmt, 5, buf, len, SQLITE_TRANSIENT)
        != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* sf */
    if (sqlite3_bind_int(stmt, 6, v->tp.sf) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }

    /* x */
    if (sqlite3_bind_int(stmt, 7, v->index) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    /* dataset */
    if (sqlite3_bind_int(stmt, 8, dataset_rowid) != SQLITE_OK) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        debug_s(sqlite3_errmsg(db));
        rc = -1;
        goto finalize;
    }
        
 finalize:
    (void) sqlite3_finalize(stmt);  /* ignore return */
        
    return rc;    
}
