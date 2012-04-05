
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>

#include "trx/macros.h"

int base64_encode(char *str, char *buf, int buf_len)
{
    BIO *bio, *b64;
    long len = 0;
    char *bio_buf;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, str, strlen(str));
    BIO_flush(bio);

    if ((len = BIO_get_mem_data(bio, &bio_buf)) > 0) {
        if (len < (buf_len - 1)) {
            memcpy(buf, bio_buf, len);
            buf[len] = 0;
        } else {
            len = 0;
        }
    } else {
        len = 0;
    }

    BIO_free_all(bio);

    return len;
}


int create_fst_basic_hdr(char *user_id,
			 char *password,
			 char *api_key,
			 char *creds_fst_basic,
			 int len)
{
    char creds_user_id_password[BUFFER_SIZE];

    strncpy(creds_user_id_password, user_id, BUFFER_SIZE);
    strncat(creds_user_id_password, ":",
	    BUFFER_SIZE - strlen(creds_user_id_password));
    strncat(creds_user_id_password, password,
	    BUFFER_SIZE - strlen(creds_user_id_password));
    strncpy(creds_fst_basic, "Authorization: FstBasic ", len);

    if ((len = base64_encode(creds_user_id_password,
			     &creds_fst_basic[strlen(creds_fst_basic)],
			     BUFFER_SIZE)) <= 0) {
        return 1;
    }

    strncat(creds_fst_basic, " ", (len - strlen(creds_fst_basic)));
    strncat(creds_fst_basic, api_key, (len - strlen(creds_fst_basic)));

    return 0;
}


int main(int argc, char *argv[])
{
    char fst_header[BUFFER_SIZE];

    create_fst_basic_hdr(argv[1], argv[2], argv[3], fst_header, BUFFER_SIZE);
    printf("%s\n", fst_header);
}

