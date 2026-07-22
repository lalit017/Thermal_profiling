#ifndef USART_H_
#define USART_H_

void USART0_init(uint32_t baudrate);
void USART0_send_byte(uint8_t data);
void USART0_send_string(const char* str);

#endif