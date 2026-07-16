#include "dummyTemp.h"
#include "config.h"

extern volatile int16_t Data[NUM_SENSORS][HISTORY_SIZE];
extern volatile int16_t SUM[NUM_SENSORS];
extern volatile uint8_t SensorError[NUM_SENSORS];

void read_dummy_sensor(uint8_t cycle, uint8_t sensor){
	int16_t dummyTemperature = 250 + (sensor * 10);
	if(sensor == 3){
		SensorError[sensor] = 1;
		return;
	}
	SensorError[sensor] = 0;
	SUM[sensor] = SUM[sensor] - Data[sensor][cycle] + dummyTemperature;
	Data[sensor][cycle] = dummyTemperature;
}