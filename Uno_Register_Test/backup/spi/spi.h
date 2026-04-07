#ifndef SPI_H
#define SPI_H

#include <avr/io.h>


// 핀 정의 (ATmega328P 기준)
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SS_PIN   PB2 // D10
#define SCK_PIN  PB5 // D13
#define MOSI_PIN PB3 // D11
#define MISO_PIN PB4 // D12

void SPI_Init(void);
uint8_t SPI_Transmit(uint8_t data);

#endif