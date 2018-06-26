
#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "shell.h"
#include "saul.h"
#include "saul_reg.h"

int initialize_sensors(void) {
    puts("Nothing to do to initialize sensors ...[ok].");
    return 1;
}

int print_phydat(phydat_t *res){
    if ( res->scale != 0 ) {
        puts("Scaling is not implemented, go out of the sun or something");
        return 1;
    }

    if ( res->unit != UNIT_CD ) {
        printf("unknown unit returned (%i)\n", res->unit);
        return 1;
    }

    printf("values: %i %i %i\n", (int16_t)res->val[0], (int16_t)res->val[1], (int16_t)res->val[2]);
    return 0;
}


int read_humidity(phydat_t *res) {
    saul_reg_t* dev = saul_reg_find_name("hdc1000");

    int dim;

    dim = saul_reg_read(dev, res);
    if (dim <= 0) {
        printf("error: failed to read from sensor\n");
        return -1;
    }

    printf("Reading from sensor (%s|%s)\n", dev->name,
         saul_class_to_str(dev->driver->type));

    return dim;
}


/* int read_humidity_shell(int argc, char **argv) { */
/*     (void) argc; */
/*     (void) argv; */

/*     phydat_t res; */

/*     int dim = read_humidity(&res); */
/*     if (dim >= 0) { */
/*         puts("Read humidity:"); */
/*         phydat_dump(&res, dim); */
/*         return 0; */
/*     } */
/*     return -1; */

/* } */

int read_light(phydat_t *res) {
    saul_reg_t* dev = saul_reg_find_name("tcs37727");

    int dim;

    dim = saul_reg_read(dev, res);
    if (dim <= 0) {
        printf("error: failed to read from sensor\n");
        return -1;
    }

    printf("Reading from sensor (%s|%s)\n", dev->name,
         saul_class_to_str(dev->driver->type));

    return dim;
}

/* int read_light_shell(int argc, char **argv) { */
/*     (void) argc; */
/*     (void) argv; */

/*     phydat_t res; */

/*     int dim = read_light(&res); */
/*     if (dim >= 0) { */
/*         puts("Read light:"); */
/*         phydat_dump(&res, dim); */
/*         return 0; */
/*     } */
/*     return -1; */
/* } */
