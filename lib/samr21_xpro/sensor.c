#include <stdio.h>
#include <math.h>

#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "saul.h"
#include "saul_reg.h"

#define ADC_USED_LINE               ADC_LINE(0)
#define RES                         ADC_RES_12BIT
#define ADC_SLEEP1                  (900000)

#define GPIO_POWER_PORT		        (PA)
#define GPIO_POWER_PIN 		        (13)
#define GPIO_POWER                  GPIO_PIN(GPIO_POWER_PORT, GPIO_POWER_PIN)

#define ADC_MEASURED_OFFSET         (16.0f)
#define ADC_MEASURED_MAX            (3395.0f)
#define ADC_MAX                     (4095.0f)


int initialize_sensors(void) {
    /* initialize a GPIO that powers the sensor just during a measure */
    if (gpio_init(GPIO_POWER, GPIO_OUT)== 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return -1;
    }
    puts("\n");
    /* first set LOW */  
    gpio_clear(GPIO_POWER);
    
    if (adc_init(ADC_USED_LINE) == 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return -1;
    }
    return 1;
}


int read_humidity(phydat_t *res) {
    
    res->unit = UNIT_PERCENT;
    res->scale = 0;

    gpio_set(GPIO_POWER);
    xtimer_usleep(ADC_SLEEP1);

    int measurement = adc_sample(ADC_USED_LINE, RES);
    gpio_clear(GPIO_POWER);

    if ( PFLANZEN_DEBUG ) {
        printf("adc sample value: %d\n", measurement);
    }

    double value = (double) measurement;
    double gain = ADC_MAX / (ADC_MEASURED_MAX - ADC_MEASURED_OFFSET);
    value = (value - ADC_MEASURED_OFFSET) * gain;
    value = (100.0/ADC_MAX) * value;

    res->val[0] = value;
    res->val[1] = 0;
    res->val[2] = 0;

    /* always return 1 as humidity has a dim of 1 */
    return 1;
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
    (void) res;

    error(-1, 0, "Sensor not present or read light not implemented for this board.");    
    return -1;
}

/* int read_light_shell(int argc, char **argv) { */

/*     (void) argc; */
/*     (void) argv; */

/*     error(-1, 0, "Sensor not present or read light not implemented for this board."); */    
/*     return -1; */
/* } */
