/*
 * Thermal_profiling.c
 *
 * Created: 09-07-2026 12:51:29
 * Author : lalit
 */ 
#include "config.h"
#include "adc.h"
#include "temperature.h"
#include "usart.h"
#include "timer.h"
#include "modBus.h"
#include <avr/wdt.h>
#include "dummyTemp.h"
#include <stdint.h>
#include "fan.h"

volatile int16_t Data[NUM_SENSORS][HISTORY_SIZE] = {0};  // Buffer to store last readings of each sensor 
uint8_t current_cycle = 0; // Keep count of which cycle it is 
volatile int16_t SUM[NUM_SENSORS] = {0}; // Buffer to store sum of last readings of each cycle
volatile uint8_t rxBuffer[MODBUS_FRAME_SIZE]; // Buffer to store incoming mod bus request
volatile uint8_t SensorError[NUM_SENSORS] = {[0 ... (NUM_SENSORS - 1)] = 1}; // Array for sensor health
volatile uint8_t rxIndex = 0; 
volatile uint8_t request_flag = 0;   	

int main(void)
{
	ADC_init(); 
	USART0_init(9600);
	timer_init(); // Implementing a 10ms timer
	fan_init();
	sei();  // Enable Global interrupt
	
	//wdt_enable(WDTO_2S);

	uint32_t previous_sensor_time = 0; // Keep track of sensor is called previously
	uint32_t previous_debug_time = 0; 
	
    while (1) 
    {
		uint32_t current_time = timer_get_ticks(); 
		if((current_time - previous_sensor_time) >= 10){
			previous_sensor_time = current_time;
			for(uint8_t sensor = 0; sensor < NUM_SENSORS; sensor++){
				read_sensor(current_cycle, sensor);
				//read_dummy_sensor(current_cycle, sensor);
			}
			current_cycle++;
			if(current_cycle >= HISTORY_SIZE) current_cycle = 0;
		}
		/*if(current_time - previous_debug_time >= 50){
			previous_debug_time = current_time;
			print_all_temperatures();
		}*/
		if(request_flag == 1) check_master_request();
		wdt_reset();
    }
}




