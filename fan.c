#include "fan.h"
#include "config.h"

volatile uint16_t current_fan_percentage = 0;

void fan_init(){
	DDRB |= (1 << PB4); // PB3 for ATmega358
	TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
	TCCR2B = (1 << CS22);
	OCR2A = 0;
}

void fan_set_speed(uint16_t percentage){
	if(percentage == 100){
		current_fan_percentage = 100;
		TCCR2A &= ~(1 << COM2A1);
		PORTB &= ~(1 << PB4); // PB3 for ATmega328
	}
	else{
		TCCR2A |= (1 << COM2A1);
		OCR2A = 255 - (uint8_t)((percentage * 255) / 100);
		current_fan_percentage = percentage;
	}
}

uint16_t get_fan_speed(){
	return current_fan_percentage;
}