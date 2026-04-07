// #ifndef UART0_H_
// #define UART0_H_

// #include <avr/io.h>

// void UART0_init(void);
// void UART0_transmit(char data);
// unsigned char UART0_receive(void);
// void UART0_print_string(char *str);
// void UART0_print_1_byte_number(uint8_t n);

// void UART0_print_hex(uint8_t data);

// #endif /* UART0_H_ */



/*
 * UART0.h
 * ATmega328P Hardware UART0 Driver
 *
 * PD0 (RXD0) : 수신 핀 - GPS 모듈의 TX 핀과 연결
 * PD1 (TXD0) : 송신 핀 - PC (시리얼 모니터)와 연결
 */
/*
 * UART0.h
 * ATmega328P Hybrid UART Driver
 *
 * PB3 (디지털 11번) : 소프트웨어 RX - GPS 모듈의 TX 핀과 연결 (충돌 방지)
 * PD1 (디지털  1번) : 하드웨어 TX - PC (시리얼 모니터) 출력 유지
 */

#ifndef UART0_H_
#define UART0_H_

#include <avr/io.h>
#include <stdint.h>

void UART0_init(void);
void UART0_transmit(char data);
unsigned char UART0_receive(void);
void UART0_print_string(char *str);
void UART0_print_1_byte_number(uint8_t n);
void UART0_print_hex(uint8_t data);

#endif /* UART0_H_ */