#include "thread.h"
#include "xtimer.h"

/* interval of measurements in microseconds*/
#define MEASUREMENTS_INTERVAL   (5 * 1000000)

int enable_debug = 0;

void *sensor_thread(void *arg)
{
    (void) arg;

    initialize_sensors();

    xtimer_ticks32_t last_wakeup = xtimer_now();
    uint32_t period = MEASUREMENTS_INTERVAL;

    while (1) {
        phydat_t res;

        int dim = read_humidity(&res);
        if ( DEBUG_SENSORS ) {
            printf("enable debug %d\n", enable_debug);
            if (dim >= 0) {
                puts("Read humidity:");
                phydat_dump(&res, dim);
            }
        }


        /* broadcast humidity */

        nodeid_t to = strtoul("0xffff", NULL, 16);
        nodeid_t source = NODE_ID;
        H2OP_MSGTYPE type = H2OP_DATA_HUMIDITY;
        int16_t netval = htons(res.val[0]);

        int rv = h2op_send(to, type, (uint8_t*) &netval, sizeof(res.val[0]), source);
        if ( DEBUG_SENSORS ) {
            if ( rv <= 0 ) {
                error(0,-rv, "could not send data");
            } else {
                puts("send humidity");
            }
        }

        /* wakes up periodically, this should get us the interval */ 
        /* as long as it is longer as we take for our measurements */
        xtimer_periodic_wakeup(&last_wakeup, period); 
        
    }

    return NULL;
}
