#pragma once

#include "saul.h"
#include "saul_reg.h"

#ifdef BOARD_NATIVE
#include "native/sensor.c"
#endif

#ifdef BOARD_SAMR21_XPRO
#include "samr21_xpro/sensor.c"
#endif

#ifdef BOARD_PBA_D_01_KW2X
#include "pba_d_01_kw2x/sensor.c"
#endif
int read_humidity(phydat_t *res);
int read_light(phydat_t *res);

int read_humidity_shell(int argc, char **argv) {
    (void) argc;
    (void) argv;

    phydat_t res;

    int dim = read_humidity(&res);
    if (dim >= 0) {
        puts("Read humidity:");
        phydat_dump(&res, dim);
        return 0;
    }
    return -1;

}


int read_light_shell(int argc, char **argv) {
    (void) argc;
    (void) argv;

    phydat_t res;

    int dim = read_light(&res);
    if (dim >= 0) {
        puts("Read light:");
        phydat_dump(&res, dim);
        return 0;
    }
    return -1;

}
