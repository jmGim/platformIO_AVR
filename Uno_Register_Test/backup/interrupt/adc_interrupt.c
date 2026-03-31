#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "UART0.h" // 라이브러리 포함

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);


volatile uint16_t adc_result = 0; // 변환 결과를 저장할 전역 변수
#define LED_PIN PB5 // 아두이노 내장 LED (D13)
// #define BTN_PIN PD2 // 외부 인터럽트 0 (D2)////

void adc_init(void) {

    DDRB |= (1 << LED_PIN);

    // 1. 기준 전압 설정 (AVCC with external capacitor at AREF pin)
    ADMUX |= (1 << REFS0);

    // 2. 입력 채널 선택 (A0 핀 선택: MUX3:0 = 0000)
    ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));

    // 3. ADCSRA 설정
    // ADEN: ADC 켬
    // ADIE: 변환 완료 시 인터럽트 발생 허용
    // ADPS2:0: 128 분주 (16MHz / 128 = 125kHz, 안정적인 변환 속도)
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    sei(); // 전역 인터럽트 허용
}

// [핵심] ADC 변환 완료 인터럽트 서비스 루틴
ISR(ADC_vect) {
    // ADC 레지스터는 ADCL과 ADCH로 나뉘어 있으나,
    // C 컴파일러가 'ADC'라는 이름으로 한 번에 16비트를 읽도록 지원함
    adc_result = ADC;

    // (옵션) 다음 변환을 자동으로 시작하고 싶다면 여기서 다시 ADSC를 켬
    // 또는 Free Running 모드를 설정할 수도 있음
}

int main(void) {
    adc_init();
    UART0_init();     // 초기화
    stdout = &OUTPUT; // printf 사용 설정

    // char str[] = "Test using UART0 Library";
    uint8_t num = 128;

    // UART0_print_string(str); // 문자열 출력
    // UART0_print_string("\r\n");
    UART0_print_1_byte_number(num); // 숫자 출력
    UART0_print_string("\r\n");

    while (1) {
        // 변환 시작 신호를 보냄 (ADSC 비트 세트)
        ADCSRA |= (1 << ADSC);

        // 변환 도중 메인 루프는 다른 작업을 수행할 수 있음 (Non-blocking)
        // 여기서는 예시로 adc_result 값을 이용해 로직 처리
        if (adc_result > 512) {
            // 전압이 약 2.5V 이상일 때 처리
            PORTB |= (1 << LED_PIN); 
        } else {
            PORTB &= ~(1 << LED_PIN); 
        }
        printf("%d\n\r", adc_result);
        // UART0_print_1_byte_number(adc_result); // 숫자 출력
        // UART0_print_string("\r\n");
        // 너무 자주 읽지 않도록 적절한 딜레이나 타이머 연동 권장
    }
    return 0;
}