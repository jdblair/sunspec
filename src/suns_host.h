
#include "trx/list.h"
#include "ezxml/ezxml.h"

/* SunSpec host status codes */
typedef enum suns_host_status {
    STATUS_UNDEF = 0,
    STATUS_SUCCESS,
    STATUS_FAILURE,
    STATUS_DR_FAILURE,
} suns_host_status_t;

/* SunSpec status codes */
typedef enum suns_host_code {
	CODE_UNDEF = 0,
    CODE_OK,
    CODE_ACCESS_DENIED,
    CODE_INVALID_MESSAGE,
    CODE_INVALID_CREDENTIALS,
    CODE_LIMIT_EXCEEDED,
    CODE_PROCESSING_EXCEPTION,
    CODE_QUOTA_EXCEEDED,
    CODE_SYSTEM_MAINTENANCE,
    CODE_UNEXPECTED_EXCEPTION,
    CODE_UNKNOWN_DEVICE,
    CODE_UNKNOWN_LOGGER,
} suns_host_code_t;



#define RESULT_STRING_SIZE 256

#define suns_host_err(e, fmt, arg...) {                                 \
        suns_host_error_reason_set((e), "%s:%d in %s", __BASE_FILE__, \
                                   __LINE__, __FUNCTION__);        \
        suns_host_error_message_set((e), fmt, ## arg); \
    }


/*
 * Used to contain the parts of the result message passed back to
 * the logger when there is some kind of failure.
 * May contain one or more suns_device_record_failure_t.
 */
typedef struct suns_host_error_detail {
    suns_host_code_t code;
    char reason[RESULT_STRING_SIZE];
    char message[RESULT_STRING_SIZE];
} suns_host_error_detail_t;    

typedef struct suns_host_result {
    suns_host_status_t status;
    suns_host_error_detail_t error;
    list_t *dr_fails;
} suns_host_result_t; 

typedef struct suns_host_dr_fail {
    char *man;
    char *mod;
    char *sn;
    char *t;
    char *id;
    char *cid;
    suns_host_status_t status;
    suns_host_error_detail_t error;
} suns_host_dr_fail_t;


typedef struct suns_host_parser {
    ezxml_t xml;
    list_t *devices;
    suns_host_result_t *result;
} suns_host_parser_t;


char *suns_host_status_string(suns_host_status_t status);
suns_host_status_t suns_host_status_from_string(char *status);
char *suns_host_status_code_string(suns_host_code_t code);
suns_host_code_t suns_host_status_code_from_string(char *code);
suns_host_result_t *suns_host_result_new(void);
void suns_host_result_free(suns_host_result_t *r);
int suns_host_result_xml(suns_host_result_t *r, char **result_xml);
int suns_host_error_reason_set(suns_host_error_detail_t *e, char *fmt, ...);
int suns_host_error_message_set(suns_host_error_detail_t *e, char *fmt, ...);
suns_host_dr_fail_t *suns_host_dr_fail_new(void);
void suns_host_dr_fail_free(suns_host_dr_fail_t *dr);
int suns_host_error_detail_to_xml(suns_host_error_detail_t *error, ezxml_t xm);
