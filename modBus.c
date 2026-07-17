#include "modBus.h"
#include "config.h"
#include "usart.h"

extern volatile int16_t SUM[NUM_SENSORS];
extern volatile uint8_t rxBuffer[MODBUS_FRAME_SIZE];
extern volatile uint8_t rxIndex;
extern volatile uint8_t request_flag;
extern volatile uint8_t SensorError[NUM_SENSORS];
static uint8_t txResponseBuffer[32];

static void transmit_single_sensor(uint8_t sensor_index);

uint16_t calculate_modbus_crc(volatile uint8_t *buffer, uint8_t length) {
	uint16_t crc = 0xFFFF; 
	
	for (uint8_t i = 0; i < length; i++) {
		crc ^= buffer[i]; 
		for (uint8_t j = 0; j < 8; j++) { 
			if (crc & 0x0001) { 
				crc = (crc >> 1) ^ 0xA001; 
				} else {
				crc >>= 1; 
			}
		}
	}
	return crc;
}

static void transmit_execption(uint8_t function_code, uint8_t execption_code){
	txResponseBuffer[0] = MY_SLAVE_ID;
	txResponseBuffer[1] = function_code | 0x80;
	txResponseBuffer[2] = execption_code;
	
	uint8_t crc = calculate_modbus_crc(txResponseBuffer, 3);
	txResponseBuffer[3] = (uint8_t)(crc & 0xFF);
	txResponseBuffer[4] = (uint8_t)(crc >> 8);
	
	PORTD |= (1 << TXC0);
	while(!(UCSR0A & (1 << TXC0)));
	PORTD &= !(1 << PD2);
}

void check_master_request(){
	uint8_t frame_length = rxIndex;
	request_flag = 0;
	rxIndex = 0;
	if(frame_length < 4) return;
	
	if(rxBuffer[0] == MY_SLAVE_ID){
		
		if(rxBuffer[1] != 0x04){
			transmit_execption(rxBuffer[1], 0x01); // Illegal function
			return;
		}
		
		uint16_t received_crc = ((uint16_t)rxBuffer[frame_length - 1] << 8) | rxBuffer[frame_length - 2];
		uint16_t calculated_crc = calculate_modbus_crc(rxBuffer, frame_length - 2);
		if (calculated_crc != received_crc){
			transmit_execption(rxBuffer[1], 0x03); // Illegal Data value
			return;
		}
		uint8_t targetSensor = rxBuffer[3];
		uint8_t quantity     = rxBuffer[5];
		
		if(targetSensor >= NUM_SENSORS || (targetSensor + quantity > NUM_SENSORS)){
			transmit_execption(rxBuffer[1], 0x02); // Illegal Data address
			return;
		}
		
		if (quantity == 1) {
            if (targetSensor < NUM_SENSORS) transmit_single_sensor(targetSensor);
        }
		else transmit_package(); 
	}
}

static void transmit_single_sensor(uint8_t sensor_index) {
	int16_t currentAverage;
	// Check if sensor is working properly or not 
	if(SensorError[sensor_index] == 0){
		currentAverage = SUM[sensor_index] / HISTORY_SIZE; // Sensor working properly transmit normal average
	}
	else{
		currentAverage = 32767; // Sensor not working properly transmit the error code
	}
	
	txResponseBuffer[0] = MY_SLAVE_ID;
	txResponseBuffer[1] = 0x04;
	txResponseBuffer[2] = 2;
	txResponseBuffer[3] = (uint8_t)(currentAverage >> 8);
	txResponseBuffer[4] = (uint8_t)(currentAverage & 0xFF);
	
	uint16_t responseCrc = calculate_modbus_crc(txResponseBuffer, 5);
	txResponseBuffer[5] = (uint8_t)(responseCrc & 0xFF);
	txResponseBuffer[6] = (uint8_t)(responseCrc >> 8);
	
	PORTD |= (1 << PD2);
	
	for(uint8_t i = 0; i < 7; i++) {
		USART0_send_byte(txResponseBuffer[i]);
	}

	UCSR0A |= (1 << TXC0);
	while (!(UCSR0A & (1 << TXC0)));
	PORTD &= ~(1 << PD2); 
}

void transmit_package(){
	txResponseBuffer[0] = MY_SLAVE_ID;
	txResponseBuffer[1] = 0x04;
	txResponseBuffer[2] = NUM_SENSORS * 2;
	
	uint8_t txIndex = 3;
	for (uint8_t i = 0; i < NUM_SENSORS; i++) {
		int16_t currentAverage;
		if(SensorError[i] == 0){
			currentAverage = SUM[i] / HISTORY_SIZE; // Sensor working properly transmit normal average
		}
		else{
			currentAverage = 32767; // Sensor not working properly transmit the error code
		}
		txResponseBuffer[txIndex++] = (uint8_t)(currentAverage >> 8);
		txResponseBuffer[txIndex++] = (uint8_t)(currentAverage & 0xFF);
	}
	
	uint16_t responseCrc = calculate_modbus_crc(txResponseBuffer, txIndex);
	txResponseBuffer[txIndex++] = (uint8_t)(responseCrc & 0xFF);
	txResponseBuffer[txIndex++] = (uint8_t)(responseCrc >> 8);
	PORTD |= (1 << PD2);

	for(uint8_t i = 0; i < txIndex; i++) {
		USART0_send_byte(txResponseBuffer[i]);
	}
	
	UCSR0A |= (1 << TXC0);
	while (!(UCSR0A & (1 << TXC0)));
	PORTD &= ~(1 << PD2);
}