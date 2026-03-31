// #define F_CPU 16000000L
// #define BIT_DELAY_US 104
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include <stdio.h>
// #include "UART0.h"

// #include <Arduino.h>

// FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

// // 1. 1ms마다 증가할 전역 변수
// // ISR 내부에서 값이 변경되므로 컴파일러 최적화 방지를 위해 반드시 volatile 선언
// volatile unsigned long timer0_millis = 0;

// // 2. 타이머0 하드웨어 초기화 함수
// void Timer0_Init(void) {
//     TCCR0A = 0x00; // 노멀 모드 (Normal mode)

//     // 프리스케일러 64 설정 (CS01, CS00 비트 1)
//     TCCR0B = (1 << CS01) | (1 << CS00);  //  CS02 CS01 CS00 : 0 1 1 --> clk/64 from prescaler  8 times difference

//     // 1ms(250틱)를 맞추기 위해 TCNT0 초기값 6 설정 (256 - 250 = 6)
//     TCNT0 = 6;  // compare timer clock

//     // 타이머0 오버플로 인터럽트 활성화
//     TIMSK0 |= (1 << TOIE0);

//     // 전역 인터럽트 활성화 (필수)
//     sei();
// }


// // 3. 타이머0 오버플로 인터럽트 서비스 루틴 (1ms마다 자동 실행)
// ISR(TIMER0_OVF_vect) {
//     TCNT0 = 6;         // 다음 1ms 간격을 위해 레지스터 초기값 재장전
//     timer0_millis++;   // 밀리초 카운터 1 증가
// }


// // 4. 안전하게 값을 읽어오는 커스텀 millis() 함수
// unsigned long millis_custom(void) {
//     unsigned long m;
    
//     // 상태 레지스터(SREG) 백업
//     uint8_t oldSREG = SREG; 

//     // 인터럽트 비활성화 (원자적 읽기 보장)
//     cli(); 
    
//     // 4바이트(unsigned long) 변수 복사
//     m = timer0_millis; 
    
//     // 인터럽트 상태 원상 복구
//     SREG = oldSREG; 

//     return m;
// }


// void ADC_init(unsigned char channel) {
//     ADMUX = (1 << REFS0); // AVCC 기준 전압
//     ADCSRA = 0x07;        // 128 분주
//     ADCSRA |= (1 << ADEN); // ADC 활성화
//     ADMUX |= ((ADMUX & 0xE0) | channel); // 채널 선택 (PC0 = 0)
//     ADCSRA |= (1 << ADSC); // 변환 시작
// }

// int read_ADC(void) {
//     ADCSRA |= (1<<ADSC);  // Free Running 안쓸 때 활성화
//     while(!(ADCSRA & (1<< ADIF))); // 변환 완료 대기
//     return ADC; // 결과 반환 (0~1023)
// }

// int main() { 
//     DDRC &= ~(1 << DDC0); 

//     DDRB |= (1 << PB7);
//     DDRB |= (1 << PB6);
//     DDRB |= (1 << PB5);
//     PORTB |= (1 << PORTB7); 
//     PORTB |= (1 << PORTB6); 
//     PORTB |= (1 << PORTB5); 


//     UART0_init();     // 초기화
//     stdout = &OUTPUT; // printf 사용 설정

//     ADC_init(0);

//     unsigned long prev_time = 0;
//     while(1){
//         unsigned long current_time = millis_custom();
//         PORTB = 0xff;
//         PORTB = 0x00;

//         int adc_value = read_ADC();

//         if (current_time - prev_time >= 10) {
//             prev_time = current_time;
//             UART0_print_1_byte_number(adc_value); // 숫자 출력
//             UART0_print_string("\r\n");
//         }
//     }
   
// }



#include <Arduino.h>

void adc_init(unsigned char);
int read_adc(void);

void setup() {
    Serial.begin(9600);
    adc_init(0);

    DDRB |= 0x38;  // set LED output (11, 12, 13)
}

void loop() {
    int adc_val = read_adc();
    Serial.println(adc_val);


    if (adc_val >= 768) {
        PORTB = 0x38;
    }
    else if (adc_val >= 512) {
        PORTB = 0x30;
    }
    else if (adc_val >= 256) {
        PORTB = 0x20;
    }
    else {
        PORTB = 0x00;
    }

    delay(100);
}

void adc_init(unsigned char channel) {
    ADMUX = (1 << REFS0); // AVCC 기준 전압
    ADCSRA = 0x07;        // 128 분주
    ADCSRA |= (1 << ADEN); // ADC 활성화
    ADMUX |= ((ADMUX & 0xE0) | channel); // 채널 선택 (PF0 = 0)
    ADCSRA |= (1 << ADSC); // 변환 시작
    //ADCSRA |= (1 << ADATE); // FREE RUNNING
}

int read_adc(void) {
    ADCSRA |= (1 << ADSC);                          // 변환 시작
    while(!(ADCSRA & (1<< ADIF))); // 변환 완료 대기
    return ADC; // 결과 반환 (0~1023)
}