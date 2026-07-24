#include "fan.h"
#include "config.h"

volatile uint16_t current_fan_percentage = 0;

void fan_init(){
	DDRB |= (1 << PB2); // Fan at PB2
	TCCR1A = (1 << COM1B1) | (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // Prescaler 64 
	OCR1B = 0;
}

void fan_set_speed(uint16_t percentage){
	if(percentage == 100){
		current_fan_percentage = 100;
		TCCR1A &= ~(1 << COM1B1);
		PORTB &= ~(1 << PB2);
	}
	else if(percentage == 0){
		current_fan_percentage = 0;
		TCCR1A &= ~(1 << COM1B1);
		PORTB |= (1 << PB2);
	}
	else{
		TCCR1A |= (1 << COM1B1);
		OCR1B = 255 - (uint8_t)((percentage * 255) / 100);
		current_fan_percentage = percentage;
	}
}

uint16_t get_fan_speed(){
	return current_fan_percentage;
}