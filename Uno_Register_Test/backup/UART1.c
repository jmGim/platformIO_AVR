// #include "UART1.h"
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include <Arduino.h>



// void UART1_init(void)
// {
//     UBRR1H = 0x00;
//     UBRR1L = 207;           // 9,600 보율 설정
//     UCSR1A |= _BV(U2X1);    // 2배속 모드
//     UCSR1C = 0x06;          // 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트
//     UCSR1B |= _BV(RXEN1);   // 수신 가능
//     UCSR1B |= _BV(TXEN1);   // 송신 가능
// }

// void UART1_transmit(char data)
// {
//     while(!(UCSR1A & (1<<UDRE1))); // 송신 대기
//     UDR1 = data;
// }

// unsigned char UART1_receive(void)
// {
//     while(!(UCSR1A & (1<<RXC1))); // 수신 대기
//     return UDR1;
// }

// void UART1_print_string(char *str)
// {
//     for(int i = 0; str[i]; i++)
//         UART1_transmit(str[i]);
// }

// void UART1_print_1_byte_number(uint8_t n)
// {
//     char numString[4] = "0";
//     int i, index = 0;
//     if(n > 0) {
//         for(i = 0; n != 0; i++) {
//             numString[i] = n % 10 + '0';
//             n = n / 10;
//         }
//         numString[i] = '\0';
//         index = i - 1;
//     }
//     for(i = index; i >= 0; i--)
//         UART1_transmit(numString[i]);
// }