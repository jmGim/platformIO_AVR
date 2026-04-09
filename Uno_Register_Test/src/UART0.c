// // // #include "UART0.h"



// // // void UART0_init(void)
// // // {
// // //     UBRR0H = 0x00;
// // //     UBRR0L = 207;           // 9,600 보율 설정
// // //     UCSR0A |= _BV(U2X0);    // 2배속 모드
// // //     UCSR0C = 0x06;          // 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트
// // //     UCSR0B |= _BV(RXEN0);   // 수신 가능
// // //     UCSR0B |= _BV(TXEN0);   // 송신 가능
// // // }

// // // void UART0_transmit(char data)
// // // {
// // //     while(!(UCSR0A & (1<<UDRE0))); // 송신 대기
// // //     UDR0 = data;
// // // }

// // // unsigned char UART0_receive(void)
// // // {
// // //     while(!(UCSR0A & (1<<RXC0))); // 수신 대기
// // //     return UDR0;
// // // }

// // // void UART0_print_string(char *str)
// // // {
// // //     for(int i = 0; str[i]; i++)
// // //         UART0_transmit(str[i]);
// // // }

// // // void UART0_print_1_byte_number(uint8_t n)
// // // {
// // //     char numString[4] = "0";
// // //     int i, index = 0;
// // //     if(n > 0) {
// // //         for(i = 0; n != 0; i++) {
// // //             numString[i] = n % 10 + '0';
// // //             n = n / 10;
// // //         }
// // //         numString[i] = '\0';
// // //         index = i - 1;
// // //     }
// // //     for(i = index; i >= 0; i--)
// // //         UART0_transmit(numString[i]);
// // // }

// // // // 1바이트 데이터를 16진수 포맷(예: "A3 ")으로 출력하는 함수
// // // void UART0_print_hex(uint8_t data)
// // // {
// // //     char hex_chars[] = "0123456789ABCDEF";
    
// // //     // 상위 4비트를 16진수 문자로 변환하여 전송
// // //     UART0_transmit(hex_chars[(data >> 4) & 0x0F ]); 
// // //     // 하위 4비트를 16진수 문자로 변환하여 전송
// // //     UART0_transmit(hex_chars[data & 0x0F]);        
// // //     // 데이터 구분을 위해 스페이스바 출력
// // //     UART0_transmit(' ');                           
// // // }




#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "UART0.h"
#include <util/delay.h>

// GPS 수신용 핀: PB3 (아두이노 11번)
#define RX_PIN PB3
#define BAUD_DELAY_US 104
#define HALF_BAUD_DELAY_US 52

void UART0_init(void)
{
    /* ── 1. 하드웨어 UART TX 초기화 (PC 모니터 출력용) ── */
    UBRR0H = 0x00;
    UBRR0L = 207;           // 9600 baud @ 16MHz, U2X0=1
    UCSR0A = (1 << U2X0);   // 2배속 모드 ON
    UCSR0C = 0x06;          // 8N1 설정
    UCSR0B = (1 << TXEN0);  // !!중요!! 송신(TX)만 활성화, RX는 비활성화

}

void UART0_transmit(char data)
{
    /* 하드웨어 TX 방식 유지 (PC로 빠르고 안정적인 전송) */
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

unsigned char UART0_receive(void)
{
    /* 소프트웨어 RX 방식 (PB3 핀을 직접 읽어들임) */
    unsigned char data = 0;

    // 1. Start bit 대기
    while(PINB & (1 << RX_PIN));

    // 정중앙 샘플링을 위한 반 주기 대기
    _delay_us(HALF_BAUD_DELAY_US);

    // 2. 8 Data bits 수신
    for(int i = 0; i < 8; i++) {
        _delay_us(BAUD_DELAY_US);
        if(PINB & (1 << RX_PIN)) {
            data |= (1 << i);
        }
    }

    // 3. Stop bit 처리 대기
    _delay_us(BAUD_DELAY_US);

    return data;
}

void UART0_print_string(char *str)
{
    for(int i = 0; str[i]; i++)
        UART0_transmit(str[i]);
}

void UART0_print_1_byte_number(uint8_t n)
{
    char numString[4] = "0";
    int i, index = 0;
    if(n > 0) {
        for(i = 0; n != 0; i++) {
            numString[i] = (n % 10) + '0';
            n /= 10;
        }
        numString[i] = '\0';
        index = i - 1;
    }
    for(i = index; i >= 0; i--)
        UART0_transmit(numString[i]);
}

void UART0_print_hex(uint8_t data)
{
    const char hex_chars[] = "0123456789ABCDEF";
    UART0_transmit(hex_chars[(data >> 4) & 0x0F]); 
    UART0_transmit(hex_chars[ data       & 0x0F]);        
    UART0_transmit(' ');                           
}