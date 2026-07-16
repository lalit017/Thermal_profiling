#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void ADC_init(void);
uint16_t get_ADC_value(uint8_t sensorNum);

#endif