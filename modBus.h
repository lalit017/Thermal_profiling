#ifndef MODBUS_H_
#define MODBUS_H_

#include <stdint.h>

void check_master_request(void);
void transmit_package(void);
uint16_t calculate_modbus_crc(volatile uint8_t *buffer, uint8_t length);

#endif
