#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mbox.h"

//Number of sensors
#define NUM_SENSORS 5

//ID of the water level sensor
#define ID_WATER_LEVEL_SENSOR 6

//Lowest Humidicity level accepted for the water level sensor
#define HUMIDICITY_LEVEL_ACCEPTED 5

//Border values for the sensors we should edit the values
#define PUMP_THRESHOLD_VERYHIGH  20
#define PUMP_THRESHOLD_HIGH  15
#define PUMP_THRESHOLD_LOW  10
#define PUMP_THRESHOLD_VERYLOW  5

//Table to storage sensors data the size of the table depens on the number of sensors
int table [NUM_SENSORS][3];
bool pump_is_on = false;
bool pump_is_empty = false;
//For the PID Controller
int integral = 0;
int last_error = 0;
//Each time we open/close the pump we should reset the table values
void reset_table( int table[][3])
{
    for (int i=0;i<NUM_SENSORS;i++){
        table[i][0]=0;
        table[i][1]=0;
        table[i][2]=0;
    }
    //Reset the last_error for the PID Controller
    last_error = 0;
}

//Function that prints the table with the values just for visual information
void print_table( int table[][3])
{
	for(int i=0;i<NUM_SENSORS;i++){
		printf("id: %d value: %d \n",table[i][0],table[i][1]);
	}
}

//Function that activates the USB Port of the board
void make_pump_open(void)
{

printf("OPEN PUMP \n");
}

//Function that shutdown the USB Port of the board
void make_pump_close(void)
{

printf("CLOSE PUMP \n");
}

void pump_get_data(int id,int data){

    (void) id;
    (void) data;
    mbox_t* mbox;
    msg_t* queue;
    msg_t msg;

    msg.type = 3;
    queue = &msg;

    mbox_t m = MBOX_INIT(queue,1);
    mbox = &m;

    mbox_put(mbox,queue);
    msg.type = 16;

    mbox_get(mbox,queue);
    printf("hello %d",queue->type);

}

void pump_set_data(int id, int data)
{
    int open_pump = 0;
    int close_pump = 0;
    int sum_hum = 0;
    int avg_hum = 0;
//    mbox_t* mbox;
//    msg_t* queue;
    int current_data = 0;
    int error = 0;
    int target_data = 12;
    int derivative = 0;
    int pwm = 0;

    //To adjust
    int kp = 1;
    int ki = 1.5;
    int kd = 1.5;



//    mbox_init(mbox,queue,1);
//    mbox_put(mbox,


    //PID Controller

    //Get current data
    current_data = data;

    //Get error
    error  = target_data - current_data;

    //Calculate the Integral
    integral = integral + error;

    //Calculate the Derivative
    derivative = error - last_error;

    //Calculate the control Variable
    pwm = (kp * error) + (ki * integral) + (kd * derivative);

   //Limit the control within the predefined values
   if(pwm < -5 && pwm > -10 ){
	data = data-1;
   }
   if(pwm < -10 && pwm > -15){
	data = data -2;
   }
   if(pwm < -15){
	data = data -3;
   }
   if(pwm > 5 && pwm < 10){
	data = data +1;
  }
  if(pwm > 10 && pwm < 15){
	data = data +2;
  }
  if(pwm > 15){
	data = data +3;
  }

   // Save the current error as last error for next iteration
   last_error =  error;

    if(id == ID_WATER_LEVEL_SENSOR && data < HUMIDICITY_LEVEL_ACCEPTED)
    {
        if(pump_is_on){
            make_pump_close();
            printf("Need to be filled");
        } else {
            printf("Need to be filled");
        }
	pump_is_empty = true;

    } else if(!pump_is_empty && id != ID_WATER_LEVEL_SENSOR) {


        if(data < PUMP_THRESHOLD_VERYLOW && !pump_is_on){
            make_pump_open();
            reset_table(table);
            pump_is_on = true;

        } else if(data  > PUMP_THRESHOLD_VERYHIGH && pump_is_on) {
            make_pump_close();
            reset_table(table);
            pump_is_on = false;
        } else {

            bool repeated_data = false;
            //If the sensor is already present in the table we update his value if not we add it to the table
            for(int i=0;i<NUM_SENSORS;i++) {

                if(table[i][0]==id){
                    repeated_data = true;
                    table[i][1] = data;
                    table[i][2] = time(NULL);
                    printf("TableUpdated \n");
                    print_table(table);
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
                printf("AddedToTable \n");
                print_table(table);
            }
        }
        //When we got the values of all the sensors we operate with the values
        if(table[NUM_SENSORS-1][0] != 0) {
            //Calculate the AvgHum
            for(int i=0;i<NUM_SENSORS;i++){
                sum_hum = sum_hum + table[i][1];
            }
            avg_hum = sum_hum / NUM_SENSORS;
            printf("ALL SENSORS SENT THE DATA. Average: %d \n", avg_hum);

            if((avg_hum < PUMP_THRESHOLD_LOW) && (avg_hum > PUMP_THRESHOLD_VERYLOW)){
                open_pump=1;
            }

            if((avg_hum < PUMP_THRESHOLD_VERYHIGH) && (avg_hum > PUMP_THRESHOLD_HIGH && pump_is_on)){
                close_pump=1;
            }
            reset_table(table);
        }

        if(open_pump==1 && !pump_is_on){
            make_pump_open();
            pump_is_on = true; 
        }

        if(close_pump==1 && pump_is_on){
            make_pump_close();
            pump_is_on = false;

        }
    }
    else if(pump_is_empty == true && id == ID_WATER_LEVEL_SENSOR && data > HUMIDICITY_LEVEL_ACCEPTED){

	pump_is_empty = false;
    }



   //We check that all sensors are working fine
  for (int i=0;i<NUM_SENSORS;i++){

	int wait_time = time(NULL);
	if((table[i][2] - wait_time) >300){
		printf("SENSOR %d is not working",table[i][0]);

	}

 }

}

int shell_pump_set_data( int argc, char * argv[])
{
    //TODO We should change this insted of keyboard parameters, a functions should send the values
    if ( argc < 3 ) {
        printf("Usage: %s pump_id data_value\n", argv[0]);
        return 1;
    }

    int id = strtol( argv[1],NULL,10);
    int data = strtol( argv[2],NULL,10);
    pump_set_data(id, data);
    return 0;
}
