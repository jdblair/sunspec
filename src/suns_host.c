/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "suns_host.h"
#include "ezxml/ezxml.h"
#include "trx/list.h"
#include "trx/macros.h"
#include "trx/debug.h"


/* http status messages required by SunSpec */
#define HTTP_STATUS_200     "200 OK"
#define HTTP_STATUS_400     "400 Bad Request"
#define HTTP_STATUS_401     "401 Unauthorized"
#define HTTP_STATUS_403     "403 Forbidden"
#define HTTP_STATUS_500     "500 Interal Server Error"
#define HTTP_STATUS_503     "503 Service Unavailable"

/* additional http status messages */
#define HTTP_STATUS_413     "413 Request Entity Too Large"
#define HTTP_STATUS_405     "405 Method Not Allowed"

/* http content types */
#define HTTP_CONTENT_XML    "application/xml"
#define HTTP_CONTENT_HTML   "text/html"


/* map SunSpec host status codes to human-readable strings */
typedef struct suns_host_status_map {
    suns_host_status_t status;
    char *string;
} suns_host_status_map_t;

static suns_host_status_map_t host_status_string_map[] = {
    { STATUS_SUCCESS,    "SUCCESS" },
    { STATUS_FAILURE,    "FAILURE" },
    { STATUS_DR_FAILURE, "DR_FAILURE" },
    { STATUS_UNDEF,      NULL },
};

/* used to map SunSpec status codes to HTTP status messages
   and human-readable strings */
typedef struct suns_host_code_map {
    suns_host_code_t code;
    char *string;
} suns_host_code_map_t;

static suns_host_code_map_t host_status_code_string_map[] = {
    { CODE_OK,                     "OK" },
    { CODE_ACCESS_DENIED,          "ACCESS_DENIED" },
    { CODE_INVALID_MESSAGE,        "INVALID_MESSAGE" },
    { CODE_INVALID_CREDENTIALS,    "INVALID_CREDENTIALS" },
    { CODE_LIMIT_EXCEEDED,         "LIMIT_EXCEEDED" },
    { CODE_PROCESSING_EXCEPTION,   "PROCESSING_EXCEPTION" },
    { CODE_QUOTA_EXCEEDED,         "QUOTA_EXCEEDED" },
    { CODE_SYSTEM_MAINTENANCE,     "SYSTEM_MAINTENANCE" },
    { CODE_UNEXPECTED_EXCEPTION,   "UNEXPECTED_EXCEPTION" },
    { CODE_UNKNOWN_DEVICE,         "UNKNOWN_DEVICE" },
    { CODE_UNKNOWN_LOGGER,         "UNKNOWN_LOGGER" },
    { CODE_UNDEF,                  NULL },
};

/*
static suns_host_code_map_t host_status_code_http_map[] = {
    { CODE_OK,                     HTTP_STATUS_200 },
    { CODE_ACCESS_DENIED,          HTTP_STATUS_403 },
    { CODE_INVALID_MESSAGE,        HTTP_STATUS_400 },
    { CODE_INVALID_CREDENTIALS,    HTTP_STATUS_401 },
    { CODE_LIMIT_EXCEEDED,         HTTP_STATUS_400 },
    { CODE_PROCESSING_EXCEPTION,   HTTP_STATUS_500 },
    { CODE_QUOTA_EXCEEDED,         HTTP_STATUS_400 },
    { CODE_SYSTEM_MAINTENANCE,     HTTP_STATUS_503 },
    { CODE_UNEXPECTED_EXCEPTION,   HTTP_STATUS_500 },
    { CODE_UNKNOWN_DEVICE,         HTTP_STATUS_400 },
    { CODE_UNKNOWN_LOGGER,         HTTP_STATUS_400 },
    { CODE_UNDEF,                  NULL },
};
*/


char *suns_host_status_string(suns_host_status_t status)
{
    int i;

    for (i = 0; host_status_string_map[i].status != STATUS_UNDEF; i++) {
        if (status == host_status_string_map[i].status)
            return host_status_string_map[i].string;
    }
    
    return NULL;
}


suns_host_status_t suns_host_status_from_string(char *status)
{
    int i;

    for (i = 0; host_status_string_map[i].status != STATUS_UNDEF; i++) {
        if (strcmp(host_status_string_map[i].string, status) == 0)
            return host_status_string_map[i].status;
    }

    return STATUS_UNDEF;
}


char *suns_host_code_string(suns_host_code_t code)
{
    int i;

    for (i = 0; host_status_code_string_map[i].code != CODE_UNDEF; i++) {
        if (code == host_status_code_string_map[i].code)
            return host_status_code_string_map[i].string;
    }

    return NULL;
}


suns_host_code_t suns_host_code_from_string(char *code)
{
    int i;

    for (i = 0; host_status_code_string_map[i].code != CODE_UNDEF; i++) {
        if (strcmp(host_status_code_string_map[i].string, code) == 0)
            return host_status_code_string_map[i].code;
    }

    return CODE_UNDEF;
}


suns_host_result_t *suns_host_result_new(void)
{
    suns_host_result_t *r = malloc(sizeof(suns_host_result_t));
    
    memset(r, 0, sizeof(suns_host_result_t));

    r->dr_fails = list_new();

    return r;
}

void suns_host_result_free(suns_host_result_t *r)
{
    list_free(r->dr_fails, free);
    free(r);
}


int suns_host_error_reason_set(suns_host_error_detail_t *e, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(e->reason, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}


int suns_host_error_message_set(suns_host_error_detail_t *e, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(e->message, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}


suns_host_dr_fail_t *suns_host_dr_fail_new(void)
{
    suns_host_dr_fail_t *dr;

    dr = malloc(sizeof(suns_host_dr_fail_t));
    memset(dr, 0, sizeof(suns_host_dr_fail_t));

    return dr;
}

void suns_host_dr_fail_free(suns_host_dr_fail_t *dr)
{
    free(dr);
}

int suns_host_error_detail_to_xml(suns_host_error_detail_t *e, ezxml_t xml)
{
    int rc = 0;
    ezxml_t child;
    char *code_str;

    /* code */
    child = ezxml_add_child(xml, "code", 0);

    code_str = suns_host_code_string(e->code);
    if (code_str) {
        (void) ezxml_set_txt(child, code_str);
    } else {
        /* this should't be possible */
        debug("e->code = %d is not defined", e->code);
    }

    /* reason */
    child = ezxml_add_child(xml, "reason", 0);
    if (e->reason)
        (void) ezxml_set_txt(child, e->reason);

    /* message */
    child = ezxml_add_child(xml, "message", 0);
    if (e->message)
        (void) ezxml_set_txt(child, e->message);

    return rc;
}
    

int suns_host_dr_fail_xml(suns_host_dr_fail_t *dr, ezxml_t xml)
{
    ezxml_t dResult, child;
    int rc = 0;
    char *status_str;

    if (dr == NULL) {
        debug("*dr is NULL");
        return -1;
    }

    dResult = ezxml_add_child(xml, "dResult", 0);

    if (dr->id)
        (void) ezxml_set_attr(dResult, "id", dr->id);

    if (dr->t)
        (void) ezxml_set_attr(dResult, "t", dr->t);

    /* cid is optional */
    if (dr->cid)
        (void) ezxml_set_attr(dResult, "cid", dr->cid);

    /* status */
    status_str = suns_host_status_string(dr->status);
    child = ezxml_add_child(dResult, "status", 0);
    if (status_str) {
        ezxml_set_txt(child, status_str);
    } else {
        debug("dr->status = %d is not defined", dr->status);
        return -1;
    }

    /* add error detail */
    rc = suns_host_error_detail_to_xml(&(dr->error), xml);

    return rc;
}


int suns_host_result_xml(suns_host_result_t *r, char **result_xml)
{
    ezxml_t xml, child;
    int rc = 0;
    char *status_str;

    if (r == NULL) {
        debug("*r is NULL");
        return -1;
    }

    xml = ezxml_new("sunSpecDataResponse");
    
    /* overall status */
    child = ezxml_add_child(xml, "status", 0);

    status_str = suns_host_status_string(r->status);
    if (status_str) {
        (void) ezxml_set_txt(child, status_str);
    } else {
        /* this shouldn't be possible */
        debug("r->status = %d is not defined", r->status);
        rc = -1;
        goto finish;
    }

    /* no additional detail for STATUS_SUCCESS */
    if (r->status == STATUS_SUCCESS) {
        /* we are done */
        goto finish;
    }

    /* for DR_FAILURE we list individual dResult elements */
    if (r->status == STATUS_DR_FAILURE) {
        debug("number of dr_fail: %d", list_count(r->dr_fails));
        list_node_t *c;
        list_for_each(r->dr_fails, c) {
            rc = suns_host_dr_fail_xml(c->data, xml);
            if (rc < 0)
                break;
        }

        /* we are done */
        goto finish;
    }

    /* add error detail */
    rc = suns_host_error_detail_to_xml(&(r->error), xml);

 finish:
    *result_xml = ezxml_toxml(xml);
    ezxml_free(xml);

    return rc;
}



    
