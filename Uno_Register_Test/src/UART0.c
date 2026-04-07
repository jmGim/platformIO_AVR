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





// // /*
// //  * UART0.c
// //  * ATmega328P Hardware UART0 Driver
// //  *
// //  * [핀 연결]
// //  *   PD0 (RXD0) ← GPS 모듈 TX
// //  *   PD1 (TXD0) → PC 시리얼 모니터 (USB-to-Serial 어댑터)
// //  *
// //  * [보율 계산]
// //  *   F_CPU = 16 MHz, 목표 Baud = 9600
// //  *   U2X0 = 1 (2배속 모드) 사용 시:
// //  *     UBRR = F_CPU / (8 × Baud) − 1
// //  *          = 16,000,000 / (8 × 9600) − 1
// //  *          = 207.33 → 207 (오차 ≈ 0.16%, 허용 범위 내)
// //  */

// // #include "UART0.h"

// // /* =======================================================================
// //  * UART0_init
// //  * =======================================================================
// //  * 역할 : UART0 하드웨어를 9600 baud, 8비트 데이터, 패리티 없음, 정지 1비트
// //  *        (8N1) 설정으로 초기화한다.
// //  *
// //  * [수정 사항 - 핵심 버그 수정]
// //  *   기존 : UCSR0A |= _BV(U2X0);
// //  *   문제 : |= (OR 연산)으로 초기화하면 리셋 직후 UCSR0A 의 TXC0(비트6),
// //  *          RXC0(비트7) 같은 상태 비트가 의도치 않게 세트된 채 남을 수 있다.
// //  *          특히 TXC0 는 "1을 쓰면 클리어"되는 비트인데, OR 연산으로 1을
// //  *          써버리면 클리어 동작이 발생해 송신 완료 플래그가 의도와 다르게
// //  *          초기화될 수 있다.
// //  *   수정 : UCSR0A = (1 << U2X0) 로 레지스터 전체를 명시적으로 대입한다.
// //  *          → U2X0 만 세트, 나머지 비트는 0으로 확정.
// //  * ===================================================================== */
// // void UART0_init(void)
// // {
// //     /* ── 보율 설정 (반드시 UART 활성화 전에 설정) ── */
// //     UBRR0H = 0x00;
// //     UBRR0L = 207;                       // 9600 baud @ 16MHz, U2X0=1

// //     /* ── 상태/제어 레지스터 A : 2배속 모드 활성화 ── */
// //     /*
// //      * ※ 기존 코드의 버그 포인트 ※
// //      * 기존: UCSR0A |= _BV(U2X0);   ← OR 연산이라 상태 비트를 오염시킬 수 있음
// //      * 수정: UCSR0A  = (1 << U2X0); ← 레지스터 전체를 명시적으로 쓴다
// //      */
// //     UCSR0A = (1 << U2X0);              // 2배속 모드 ON, 나머지 비트 클리어

// //     /* ── 제어 레지스터 C : 프레임 형식 설정 ── */
// //     /*
// //      * UCSR0C 비트 구성 (ATmega328P):
// //      *   Bit 7:6  UMSEL01:00  = 00 → 비동기 UART
// //      *   Bit 5:4  UPM01:00   = 00 → 패리티 없음
// //      *   Bit 3    USBS0      = 0  → 정지 비트 1개
// //      *   Bit 2:1  UCSZ01:00  = 11 → 8비트 데이터 (UCSZ02=0 과 조합)
// //      *   Bit 0    UCPOL0     = 0  → 동기 모드 전용, 비동기에서는 0
// //      * 0x06 = 0b00000110 → UCSZ01=1, UCSZ00=1 → 8비트 데이터 ✓
// //      */
// //     UCSR0C = 0x06;

// //     /* ── 제어 레지스터 B : 송수신 활성화 ── */
// //     UCSR0B = (1 << RXEN0)              // 수신기 활성화 (PD0 = RXD)
// //            | (1 << TXEN0);             // 송신기 활성화 (PD1 = TXD)
// //     /*
// //      * 기존 코드처럼 |= 를 두 번 나눠 쓰면 첫 번째 |= 직후 잠깐 TXEN0 없이
// //      * RXEN0 만 켜진 상태가 생길 수 있다. 한 번에 대입하는 것이 안전하다.
// //      */
// // }

// // /* =======================================================================
// //  * UART0_transmit
// //  * =======================================================================
// //  * 역할 : 문자 1개를 UART0 TX(PD1)로 송신한다.
// //  *        UDRE0(송신 버퍼 비어있음 플래그)가 세트될 때까지 블로킹된다.
// //  * ===================================================================== */
// // void UART0_transmit(char data)
// // {
// //     /* UDR0(송신 버퍼)가 비워질 때까지 대기 */
// //     while (!(UCSR0A & (1 << UDRE0)));
// //     UDR0 = data;
// // }

// // /* =======================================================================
// //  * UART0_receive
// //  * =======================================================================
// //  * 역할 : UART0 RX(PD0)에서 문자 1개를 수신한다.
// //  *        RXC0(수신 완료 플래그)가 세트될 때까지 블로킹된다.
// //  * ===================================================================== */
// // unsigned char UART0_receive(void)
// // {
// //     /* 수신 버퍼에 데이터가 들어올 때까지 대기 */
// //     while (!(UCSR0A & (1 << RXC0)));
// //     return UDR0;
// // }

// // /* =======================================================================
// //  * UART0_print_string
// //  * =======================================================================
// //  * 역할 : 널(NULL) 종료 문자열을 한 문자씩 UART0로 송신한다.
// //  * ===================================================================== */
// // void UART0_print_string(char *str)
// // {
// //     for (int i = 0; str[i] != '\0'; i++)
// //         UART0_transmit(str[i]);
// // }

// // /* =======================================================================
// //  * UART0_print_1_byte_number
// //  * =======================================================================
// //  * 역할 : uint8_t 값을 10진수 문자열로 변환하여 UART0로 출력한다.
// //  *        예) 123 → "123"
// //  * ===================================================================== */
// // void UART0_print_1_byte_number(uint8_t n)
// // {
// //     char numString[4] = "0";
// //     int i, index = 0;

// //     if (n > 0) {
// //         for (i = 0; n != 0; i++) {
// //             numString[i] = (n % 10) + '0';
// //             n /= 10;
// //         }
// //         numString[i] = '\0';
// //         index = i - 1;
// //     }

// //     /* 역순으로 저장된 숫자를 올바른 순서로 출력 */
// //     for (i = index; i >= 0; i--)
// //         UART0_transmit(numString[i]);
// // }

// // /* =======================================================================
// //  * UART0_print_hex
// //  * =======================================================================
// //  * 역할 : 1바이트 데이터를 "XX " 형태의 16진수 문자열로 UART0에 출력한다.
// //  *        예) 0xA3 → "A3 "
// //  * ===================================================================== */
// // void UART0_print_hex(uint8_t data)
// // {
// //     const char hex_chars[] = "0123456789ABCDEF";

// //     UART0_transmit(hex_chars[(data >> 4) & 0x0F]);  /* 상위 니블 */
// //     UART0_transmit(hex_chars[ data       & 0x0F]);  /* 하위 니블 */
// //     UART0_transmit(' ');                             /* 구분 공백  */
// // }




// #ifndef F_CPU
// #define F_CPU 16000000UL // 아두이노 우노 동작 클럭 (16MHz)
// #endif

// #include "UART0.h"
// #include <util/delay.h>

// // 핀 정의: PB3 = RX (아두이노 11번), PB4 = TX (아두이노 12번)
// #define RX_PIN PB3
// #define TX_PIN PB4

// // 9600 보율의 1비트 전송 시간 (1,000,000 / 9600 = 약 104us)
// #define BAUD_DELAY_US 104
// #define HALF_BAUD_DELAY_US 52

// void UART0_init(void)
// {
//     // TX_PIN (PB4) 설정: 출력 모드, 초기 상태 High (Idle)
//     DDRB |= (1 << TX_PIN);
//     PORTB |= (1 << TX_PIN);

//     // RX_PIN (PB3) 설정: 입력 모드, 내부 풀업 저항 활성화
//     DDRB &= ~(1 << RX_PIN);
//     PORTB |= (1 << RX_PIN);
// }

// void UART0_transmit(char data)
// {
//     // 1. Start bit (Low)
//     PORTB &= ~(1 << TX_PIN);
//     _delay_us(BAUD_DELAY_US);

//     // 2. 8 Data bits 전송 (LSB First)
//     for(int i = 0; i < 8; i++) {
//         if(data & (1 << i)) {
//             PORTB |= (1 << TX_PIN);
//         } else {
//             PORTB &= ~(1 << TX_PIN);
//         }
//         _delay_us(BAUD_DELAY_US);
//     }

//     // 3. Stop bit (High)
//     PORTB |= (1 << TX_PIN);
//     _delay_us(BAUD_DELAY_US);
// }

// unsigned char UART0_receive(void)
// {
//     unsigned char data = 0;

//     // 1. Start bit 대기 (핀이 High에서 Low로 떨어질 때까지)
//     while(PINB & (1 << RX_PIN));

//     // 신호의 정중앙에서 샘플링하기 위해 반 주기 대기
//     _delay_us(HALF_BAUD_DELAY_US);

//     // 2. 8 Data bits 수신 (LSB First)
//     for(int i = 0; i < 8; i++) {
//         _delay_us(BAUD_DELAY_US);
//         if(PINB & (1 << RX_PIN)) {
//             data |= (1 << i);
//         }
//     }

//     // 3. Stop bit 처리 시간 대기
//     _delay_us(BAUD_DELAY_US);

//     return data;
// }

// // 아래 출력 함수들은 UART0_transmit()을 기반으로 하므로 수정할 필요가 없습니다.
// void UART0_print_string(char *str)
// {
//     for(int i = 0; str[i]; i++)
//         UART0_transmit(str[i]);
// }

// void UART0_print_1_byte_number(uint8_t n)
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
//         UART0_transmit(numString[i]);
// }

// void UART0_print_hex(uint8_t data)
// {
//     char hex_chars[] = "0123456789ABCDEF";
//     UART0_transmit(hex_chars[(data >> 4) & 0x0F ]); 
//     UART0_transmit(hex_chars[data & 0x0F]);        
//     UART0_transmit(' ');                           
// }





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