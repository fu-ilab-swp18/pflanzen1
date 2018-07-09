#include "../util.h"
#include "saul.h"
#include "saul_reg.h"

int initialize_sensors(void) {
    puts("Nothing to do to initialize sensors ...[ok].");
    return 1;
}

int read_humidity(phydat_t *res) {
    res->unit = UNIT_PERCENT;
    res->scale = 0;
    
    if ( PFLANZEN_DEBUG ) {
        puts("Mocking val for read humidity as on native");
    }

    res->val[0] = 50;
    res->val[1] = 0;
    res->val[2] = 0;

    return 1;
}


int read_light(phydat_t *res) {
    res->unit = UNIT_CD;
    res->scale = 0;
    
    if ( PFLANZEN_DEBUG ) {
        puts("Mocking val for read light as on native");
    }

    res->val[0] = 50;
    res->val[1] = 51;
    res->val[2] = 52;

    return 3;
}

