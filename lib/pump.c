#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mbox.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "saul.h"
#include "saul_reg.h"
#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include <math.h>
#include <xtimer.h>

//Number of sensors
#define NUM_SENSORS 5

//ID of the water level sensor
#define ID_WATER_LEVEL_SENSOR COLLECTOR_NODE_ID

//Lowest Humidicity level accepted for the water level sensor
#define HUMIDICITY_LEVEL_ACCEPTED 80

//Border values for the sensors we should edit the values
#define PUMP_THRESHOLD_VERYHIGH  80
#define PUMP_THRESHOLD_HIGH  60
#define PUMP_THRESHOLD_LOW  40
#define PUMP_THRESHOLD_VERYLOW  20

#ifdef BOARD_SAMR21_XPRO
#define GPIO_POWER_PORT		(PA)
#define GPIO_POWER_PIN_PUMP		(16)
#define GPIO_POWER_PUMP	 	GPIO_PIN(GPIO_POWER_PORT,GPIO_POWER_PIN_PUMP)
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

//Table to storage sensors data the size of the table depens on the number of sensors
int table [NUM_SENSORS][3];
bool pump_is_on = false;
bool pump_is_empty = false;

//For the PID Controller
int integral =0;
int last_error=0;

//Each time we open/close the pump we should reset the table values
void reset_table( int table[][3])
{
    for (int i=0;i<NUM_SENSORS;i++){
        table[i][0]=0;
        table[i][1]=0;
	table[i][2]=0;
    }
	//Reset the variables for the PID Controller
	last_error = 0;
	integral = 0;
}

//Function that prints the table with the values just for visual information
void print_table( int table[][3])
{
    for(int i=0;i<NUM_SENSORS;i++){
        printf("id: %04X value: %d at: %d \n",table[i][0],table[i][1],table[i][2]);
    }
}
void initialize_pump(void){
#ifdef BOARD_SAMR21_XPRO
   //Initialize a GPIO that powers the pump
   if(gpio_init(GPIO_POWER_PUMP,GPIO_OUT)==0){
       if ( PFLANZEN_DEBUG ) {
           puts("GPIO Initialized");
       }
   }
   else{
       puts("Error: GPIO NOT Initialized");
  }
  //Firts set LOW
  gpio_clear(GPIO_POWER_PUMP);
#endif
}

void make_pump_close(void)
{
#ifdef BOARD_SAMR21_XPRO
   gpio_clear(GPIO_POWER_PUMP);
#endif
   pump_is_on=false;
   printf("PUMP CLOSE \n");
}

//Function that activates the USB Port of the board
void make_pump_open(int aux)
{
#ifdef BOARD_SAMR21_XPRO
   gpio_set(GPIO_POWER_PUMP);
#endif
   printf("OPEN PUMP \n");
   pump_is_on = true;
   xtimer_sleep(aux);
   make_pump_close();
}


void water_level_sensor_control (int data)
{
	if(data < HUMIDICITY_LEVEL_ACCEPTED){
		if(pump_is_on){
			make_pump_close();
		}
        puts("Need to be filled");
	}
}
void add_data_table(int id,int data)
{
    bool repeated_data = false;
                //If the sensor is already present in the table we update his value if not we add it to the table
                for(int i=0;i<NUM_SENSORS;i++) {

                    if(table[i][0]==id){
                        repeated_data = true;
                        table[i][1] = data;
                        table[i][2] = time(NULL);
                        if ( PFLANZEN_DEBUG ) {
                            printf("TableUpdated \n");
                            print_table(table);
                        }
                    }
                }
                if(!repeated_data){
                    int aux=0;
                    //We look for the first free space in the table
                    while(table[aux][0] != 0){
                        aux++;
                    }
                    table[aux][0] = id;
                    table[aux][1] = data;
                    table[aux][2] = time(NULL);
                    if ( PFLANZEN_DEBUG ) {
                        printf("AddedToTable \n");
                        print_table(table);
                    }
                }
}
int add_pid_controller(int data)
{
    //PID CONTROLLER-----------------------------------------------------
    int current_data=0;
    int actual_error = 0;
    int target_data = 50;
    int derivative = 0;
    int pwm = 0;
    int aux = 0;

    //To Adjust
    int kp=2;
    int ki=1.5;
    int kd=2;

    current_data = data;

    //Get error
    actual_error = target_data - current_data;

    //Calculate the Integral
    integral = integral + actual_error;

    //Calculate the Derivative
    derivative = actual_error - last_error;

    //Calculate the control Variable
    pwm = (kp*actual_error)+(ki*integral)+(kd*derivative);

    //Limit the control within the predefined values
    if(pwm < -150 && pwm > -200){
        aux = 3;
    }

    if(pwm < -90 && pwm > -150){
        aux = 2;
    }

    if(pwm < -30 && pwm > -90){
        aux = 2;
    }
    if(pwm < 0 && pwm > -30){
        aux = 1;
    }

    if(pwm > 150 && pwm < 200){
        aux = 1;
    }

    if(pwm > 90 && pwm < 150){
        aux = 1;
    }

    if(pwm > 30 && pwm < 90){
        aux = 2;
    }

    if(pwm > 0 && pwm < 30){
        aux = 3;
    }

    last_error=actual_error;

    return aux;
    //END PID CONTROLLER-------------------------------------------------
}

void pump_set_data(int id, int data)
{
    if ( PFLANZEN_DEBUG ) {
        printf("pump set data id=%04X, data=%d\n", id, data);
    }

    // mbox_t mbox;
    // msg_t*  msg;
    //  msg_t  mssg;
    // msg->type= 2;
    //    mbox = MBOX_INIT(msg, 1);
    //  mbox_t* m;
    //  mssg.type=2;
    //  msg = &mssg;
    // m = &mbox;
    // mbox_put(m,msg);
    int open_pump = 0;
    int close_pump = 0;
    int sum_hum = 0;
    int avg_hum = 0;
    int aux=1;

    //    time_t timedata; 
    aux = MAX(add_pid_controller(data),aux);

    if(id == ID_WATER_LEVEL_SENSOR) {

        water_level_sensor_control(data);
	}


     else{
         if(!pump_is_empty){

             if(data < PUMP_THRESHOLD_VERYLOW && !pump_is_on){

                    make_pump_open(aux);
                    reset_table(table);
		    aux=1;
                //   pump_is_on = true;


             }
             else if((data  > PUMP_THRESHOLD_VERYHIGH) && (pump_is_on)) {

                make_pump_close();
                reset_table(table);
		aux=1;
              //  pump_is_on = false;
             }
             else {

                add_data_table(id,data);
                
            }
            //When we got the values of all the sensors we operate with the values
            if(table[NUM_SENSORS-1][0] != 0) {
                //Calculate the AvgHum
                for(int i=0;i<NUM_SENSORS;i++){
                    sum_hum = sum_hum + table[i][1];
                }
                avg_hum = sum_hum / NUM_SENSORS;
                if ( PFLANZEN_DEBUG ) {
                    printf("ALL SENSORS SENT THE DATA. Average: %d \n", avg_hum);
                }

                if((avg_hum < PUMP_THRESHOLD_LOW) && (avg_hum > PUMP_THRESHOLD_VERYLOW)){
                    open_pump=1;
                }

                if((avg_hum < PUMP_THRESHOLD_VERYHIGH) && (avg_hum > PUMP_THRESHOLD_HIGH && pump_is_on)){
                    close_pump=1;
                }
                reset_table(table);
		aux=1;
            }

            if(open_pump==1 && !pump_is_on){
                make_pump_open(aux);
               // pump_is_on = true;
            }

            if(close_pump==1 && pump_is_on){
                make_pump_close();
               // pump_is_on = false;

            }
        }
        else if((id = ID_WATER_LEVEL_SENSOR) &&(pump_is_empty) && (data >= HUMIDICITY_LEVEL_ACCEPTED)){

            pump_is_empty = false;
        }
    }
}

int pump_get_data(void)
{

    // while(true){


    // }
    return 0;
}


int shell_pump_set_data( int argc, char * argv[])
{
    //TODO We should change this insted of keyboard parameters, a functions should send the values
    if ( argc < 3 ) {
        printf("Usage: %s pump_id data_value\n", argv[0]);
        return 1;
    }

   initialize_pump();
   int id = strtol( argv[1],NULL,16);
   int data = strtol( argv[2],NULL,10);
   pump_set_data(id, data);
   make_pump_open(1);
   return 0;
}
