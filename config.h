#ifndef CONFIG_H_
#define	CONFIG_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#define NUM_SENSORS 10
#define HISTORY_SIZE 10
#define MY_SLAVE_ID 0x05
#define MODBUS_FRAME_SIZE 32

#endif