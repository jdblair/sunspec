
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "trx/debug.h"


int main (int argc, char *argv[])
{
    uint64_t u64 = 3849758027408382884LL;
    int64_t i64 = -2931028394020238096LL;
    uint64_t a64 = 8307395740592438593LL;
    double f64 = 2.7182818284590452354L;

    dump_buffer(stdout, (char *)&u64, 8);
    dump_buffer(stdout, (char *)&i64, 8);
    dump_buffer(stdout, (char *)&a64, 8);
    dump_buffer(stdout, (char *)&f64, 8);

    printf("sizeof(uint64_t) = %d\n", sizeof(uint64_t));
}

