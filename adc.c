#include "adc.h"
#include "config.h"

void ADC_init(){
	ADMUX &= ~((1 << REFS1) | (1 << REFS0));
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t get_ADC_value(uint8_t sensorNum){
	ADMUX &= 0xf0;
	ADMUX |= sensorNum & 0x0f;
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC)) ;
	return ADC;
}
