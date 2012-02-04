
typedef enum suns_host_status {
    STATUS_SUCCESS = 0,
    STATUS_FAILURE,
    STATUS_DR_FAILURE,
} suns_host_status_t;

/* SunSpec status codes */
typedef enum suns_host_code {
    CODE_OK = 0,
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
} suns_host_code_t

/* used to map SunSpec status codes to HTTP status messages */
typedef struct suns_host_status_map {
    suns_host_status_t status;
    char *http_status;
} suns_host_status_map_t;

#define RESULT_STRING_SIZE=256

typedef struct suns_host_parser {
    ezxml_t xml;
    list_t *devices;
    suns_host_result_t *result;
} suns_host_parser_t;
    


/*
 * Used to contain the parts of the result message passed back to
 * the logger when there is some kind of failure.
 * May contain one or more suns_device_record_failure_t.
 */
typedef struct suns_host_result {
    suns_host_status_t status;
    suns_host_code_t code;
    char reason[RESULT_STRING_SIZE];
    char message[RESULT_STRING_SIZE];
    list_t *dr_fails;
} suns_host_result_t;


typedef struct suns_host_dr_fail {
    char *man;
    char *mod;
    char *sn;
    char *t;
    char *id;
    suns_host_status_t status;
    suns_host_code_t code;
    char reason[RESULT_STRING_SIZE];
    char message[RESULT_STRING_SIZE];
} suns_host_dr_fail_t;


suns_host_result_t *suns_host_result_new(void);
void suns_host_result_free(suns_host_result_t *r);
int suns_host_result_set_reason(suns_host_result_t *r, char *fmt, ...);
int suns_host_result_set_message(suns_host_result_t *r, char *fmt, ...);
suns_host_dr_fail_t *suns_host_dr_fail_new(void);
void suns_host_dr_fail_free(suns_host_dr_fail_t *dr);
int suns_host_dr_fail_set_reason(suns_host_dr_fail_t *dr, char *fmt, ...);
int suns_host_dr_fail_set_message(suns_host_dr_fail_t *r, char *fmt, ...);
