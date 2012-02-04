/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ezxml.h"
#include "macros.h"


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


static suns_host_status_map_t host_status_map {
    { CODE_UNDEF,                  NULL };
    { CODE_OK,                     HTTP_STATUS_200 };
    { CODE_ACCESS_DENIED,          HTTP_STATUS_403 };
    { CODE_INVALID_MESSAGE,        HTTP_STATUS_400 };
    { CODE_INVALID_CREDENTIALS,    HTTP_STATUS_401 };
    { CODE_LIMIT_EXCEEDED,         HTTP_STATUS_400 };
    { CODE_PROCESSING_EXCEPTION,   HTTP_STATUS_500 };
    { CODE_QUOTA_EXCEEDED,         HTTP_STATUS_400 };
    { CODE_SYSTEM_MAINTENANCE,     HTTP_STATUS_503 };
    { CODE_UNEXPECTED_EXCEPTION,   HTTP_STATUS_500 };
    { CODE_UNKNOWN_DEVICE,         HTTP_STATUS_400 };
    { CODE_UNKNOWN_LOGGER,         HTTP_STATUS_400 };
    { CODE_UNDEF,                  NULL };
};


suns_host_result_t *suns_host_result_new(void)
{
    suns_host_result_t *r = malloc(sizeof(suns_host_result_t));
    
    memset(r, 0, sizeof(suns_host_result_t));

    r->dr_failures = list_new();

    return r;
}

void suns_host_result_free(suns_host_result_t *r)
{
    list_free(r->dr_failures, free);
    free(r);
}


int suns_host_result_set_reason(suns_host_result_t *r, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(r->reason, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}


int suns_host_result_set_message(suns_host_result_t *r, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(r->message, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}


suns_dr_fail_t *suns_dr_fail_new(void)
{
    suns_dr_fail_t *dr;

    dr = malloc(sizeof(suns_dr_fail_t));
    memset(dr, 0, sizeof(suns_dr_fail_t));

    return r;
}

void suns_dr_fail_free(suns_dr_fail_t *dr)
{
    free(dr);
}

int suns_dr_fail_set_reason(suns_dr_fail_t *dr, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(r->reason, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}


int suns_dr_fail_set_message(suns_dr_fail_t *r, char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsnprintf(r->message, RESULT_STRING_SIZE, fmt, ap);
    va_end(ap);

    return rc;
}
    
