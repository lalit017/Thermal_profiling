#include "temperature.h"
#include "config.h"
#include "adc.h"

extern volatile int16_t Data[NUM_SENSORS][HISTORY_SIZE];
extern volatile int16_t SUM[NUM_SENSORS];
extern volatile uint8_t SensorError[NUM_SENSORS];

#define R_FIXED 1350UL


void read_sensor(uint8_t cycle, uint8_t sensor) {
	uint16_t rawADC = get_ADC_value(sensor);
	
	// In case if the sensor is not working and giving garbage value or wire gets cuts
	if(rawADC < 5 || rawADC > 1018){ 
		SensorError[sensor] = 1; // Marks sensor as broken
		return;
	}
	SensorError[sensor] = 0; // Marks sensor as healthy
	
	uint32_t pt_resistance = ((uint32_t)rawADC * R_FIXED) / (1023UL - rawADC);
	
	int32_t temp_calc = ((int32_t)pt_resistance - 1000L) * 1000L / 385L;

	int16_t scaledTemperature = (int16_t)temp_calc;
	
	
	SUM[sensor] = SUM[sensor] - Data[sensor][cycle] + scaledTemperature;
	Data[sensor][cycle] = scaledTemperature;
}

void print_sensor_debug(uint8_t sensor){
	char debug_buffer[64];
	uint16_t rawADC = get_ADC_value(sensor);
	if(rawADC < 5 || rawADC > 1018){
		sprintf(debug_buffer, "Sensor %d: ERROR (Raw ADC: %u)\r\n", sensor, rawADC);
	}
	else{
		uint16_t voltage_mv = (uint16_t)(((uint32_t)rawADC * 5000UL) / 1023UL);
		uint32_t pt_resistance = ((uint32_t) rawADC * R_FIXED) / (1023UL - rawADC);
		int32_t temp_calc = ((uint32_t) pt_resistance - 1000L) * 1000L / 385L;
		sprintf(debug_buffer, "S%d | ADC:%u | V:%u mV | R:%lu Ohm | T:%ld (x10 C)\r\n", sensor, rawADC, voltage_mv, pt_resistance, temp_calc); 
	}
	PORTD |= (1 << PD2);
	USART0_send_string(debug_buffer);
	UCSR0A |= (1 << TXC0);
	while(!(UCSR0A & (1 << TXC0)));
	PORTD &= ~(1 << PD2); 
}