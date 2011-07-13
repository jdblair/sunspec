


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

#include "trx/macros.h"

int main(int argc, char *argv[])
{
    modbus_t *m;
    uint8_t *q;
    int header_length;
    int socket;
    int rc;

    m = modbus_new_tcp("127.0.0.1", 1502);
    q = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    header_length = modbus_get_header_length(m);

    modbus_set_debug(m, TRUE);
    modbus_set_error_recovery(m, TRUE);

    socket = modbus_tcp_listen(m, 1);
    modbus_tcp_accept(m, &socket);

    while (1) {
	rc = modbus_receive(m, -1, q);
	if (rc < 0) {
	    error("modbus_receive() returned %d", rc);
	}

	dump_buffer(stdout, q, rc);
    }
}
