#include "fan.h"
#include "config.h"

volatile uint16_t current_fan_percentage = 0;

void fan_init(){
	DDRB |= (1 << PB1); // PB1 is an output
	
	// Halt the timer and clear configurations first
	TCCR1A = 0;
	TCCR1B = 0;
	
	// Set the TOP value for 200 Hz (at 8 MHz clock) BEFORE starting the clock
	ICR1 = 9999;
	
	// Configure Mode 14 (Fast PWM, TOP=ICR1) and Enable Output on OC1A
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	
	// Start the timer with Prescaler 8
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	
	// Initialize fan to 0% in start
	fan_set_speed(0);
}

void fan_set_speed(uint16_t percentage){
	if(percentage == 100){
		current_fan_percentage = 100;
		TCCR1A &= ~(1 << COM1A1);
		PORTB &= ~(1 << PB1);
	}
	else if(percentage == 0){
		current_fan_percentage = 0;
		TCCR1A &= ~(1 << COM1A1);
		PORTB |= (1 << PB1);
	}
	else{
		TCCR1A |= (1 << COM1A1);
		OCR1A = 9999 - (uint16_t)(((uint32_t)percentage * (uint32_t)9999) / 100);
		current_fan_percentage = percentage;
	}
}

uint16_t get_fan_speed(){
	return current_fan_percentage;
}