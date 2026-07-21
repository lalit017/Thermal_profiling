#include "fan.h"
#include "config.h"

void fan_init(){
	DDRB |= (1 << PB4); // PB3 for ATmega358
	TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
	TCCR2B = (1 << CS22);
	OCR2A = 0;
}

void fan_set_speed(uint16_t percentage){
	if(percentage > 100) percentage = 100;
	OCR2A = 255 - (uint8_t)((percentage * 255) / 100);
}