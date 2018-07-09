#pragma once

#ifndef GLOBAL_C_GUARD
#define GLOBAL_C_GUARD

//XXX this should be const but it seems C forbids this?
typedef uint16_t nodeid_t;
nodeid_t NODE_ID;
char PFLANZEN_DEBUG;

/* interval of measurements in microseconds*/
uint32_t MEASUREMENT_INTERVAL = 5 * 1000000;

#endif /* end of include guard: GLOBAL_C_GUARD */
