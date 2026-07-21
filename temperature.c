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