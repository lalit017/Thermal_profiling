#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include <stdint.h>
#include <stdio.h>
#include "usart.h"

void print_sensor_debug(uint8_t sensor);
void read_sensor(uint8_t cycle, uint8_t sensor);
void print_all_temperatures();

#endif