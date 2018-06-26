/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for the SEN0114 moisture sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <math.h>

#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES                         ADC_RES_12BIT
/* #define ADC_IN_USE                  ADC_0 */
#define ADC_IN_USE                  (0)

#define ADC_SLEEP1                  (1)
#define ADC_SLEEP2                  (1)

#define GPIO_POWER_PORT		    (PA)
#define GPIO_POWER_PIN 		    (13)
/* static unsigned int value; */

int main(void)
{
    puts("\nRIOT test for a moisture sensor\n");
    printf("\nusing GPIO port %d, pin %d", GPIO_POWER_PORT, GPIO_POWER_PIN);

    /* initialize a GPIO that powers the sensor just during a measure */
    gpio_t gpio_power = GPIO_PIN(GPIO_POWER_PORT, GPIO_POWER_PIN);
    if (gpio_init(gpio_power, GPIO_OUT)== 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return 1;
    }
    puts("\n");
    /* first set LOW */  
    gpio_clear(gpio_power);
    
    /* initialize the adc device */ 
    adc_t line = ADC_LINE(0);
    printf("Initializing ADC_%i @ %i bit resolution", 0, (6 + (2* RES)));
    if (adc_init(line) == 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return 1;
    }
    while(1) {
        gpio_set(gpio_power);
        xtimer_sleep(ADC_SLEEP1);

        int value = adc_sample(line, RES);
        printf("Value: ADC_%i: %4i\n", line, value);
        xtimer_sleep(ADC_SLEEP2);
        gpio_clear(gpio_power);
    }    
    return 0;
}
