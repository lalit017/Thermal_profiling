#include "config.h"
#include "timer.h"

static volatile uint32_t sys_ticks = 0;
extern volatile uint8_t rx_timer_active;
extern volatile uint8_t rx_timeout_counter;
extern volatile uint8_t request_flag;

void timer_init(void) {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	OCR1A = 2499;
	TIMSK1 = (1 << OCIE1A);
	TCCR1B |= (1 << CS11) | (1 << CS10);
}

	uint32_t timer_get_ticks(void) {
		uint32_t ticks_copy;
		uint8_t sreg = SREG;
		cli();                               // Critical section protection for 8-bit core
		ticks_copy = sys_ticks;
		SREG = sreg;
		return ticks_copy;
	}

ISR(TIMER1_COMPA_vect) {
	sys_ticks++;
	if(rx_timer_active){
		rx_timeout_counter++;
		if(rx_timeout_counter >= 5){
			request_flag = 1;
			rx_timer_active = 0;
		}
	}
}