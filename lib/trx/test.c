
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "debug.h"
#include "macros.h"
#include "string.h"
#include "date.h"


extern int verbose_level;

int main(int argc, char **argv)
{
    int rc;
    int milliseconds;
    verbose_level = 0;
    
    char *dates[] = {
	"1985-04-12T23:20:50.52Z",
	"1996-12-19T16:39:57-08:00",
	"1990-12-31T23:59:60Z",
	"1990-12-31T15:59:60-08:00",
	"1937-01-01T12:00:27.87+00:20",
	NULL
    };

    int i;
    struct tm tm_time;
    time_t unixtime;

    for (i = 0; dates[i] != NULL; i++) {
	memset(&tm_time, 0, sizeof(tm_time));
	rc = date_parse_rfc3339_to_gmt(dates[i], &unixtime, &milliseconds);
	printf("%d: %s = %s",
	       rc, dates[i], ctime(&unixtime));
    }
    
    return 0;
}


    
