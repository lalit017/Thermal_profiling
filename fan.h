#ifndef FAN_H_
#define FAN_H_

#include <stdint.h>

void fan_init();
void fan_set_speed(uint16_t percentage);
uint16_t get_fan_speed();

#endif
