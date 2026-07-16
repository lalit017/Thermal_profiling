#include "config.h"
#include "usart.h"

extern volatile uint8_t rxBuffer[MODBUS_FRAME_SIZE];
extern volatile uint8_t rxIndex;
extern volatile uint8_t request_flag;
volatile uint8_t rx_timer_active = 0;
volatile uint8_t rx_timeout_counter = 0;

void USART0_init(uint32_t baudrate){
	uint16_t ubrr_value = (F_CPU / (16UL * baudrate)) - 1;
	UBRR0H = (uint8_t)(ubrr_value >> 8);
	UBRR0L = (uint8_t)ubrr_value;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	DDRD |= (1 << PD2); // Assuming PD2 is wired to DE and /RE pins
	PORTD &= ~(1 << PD2); // Start in listening/Receiver mode
}

void USART0_send_byte(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

ISR(USART0_RX_vect){
	uint8_t receivedByte = UDR0;
	if(rxIndex < MODBUS_FRAME_SIZE){
		rxBuffer[rxIndex++] = receivedByte;
		rx_timeout_counter = 0;
		rx_timer_active = 1;
	}
	else{
		rxIndex = 0;
		rx_timer_active = 0;
	}
}