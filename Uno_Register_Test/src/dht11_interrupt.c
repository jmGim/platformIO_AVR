#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART0.h"

#define F_CPU 16000000UL
#define DHT_PIN PD2
#define LED_PIN PB5

// DHT11 데이터를 저장할 전역 변수 (ISR과 main이 공유하므로 volatile 필수)
volatile uint8_t dht_data[5] = {0};
volatile uint8_t bit_count = 0;
volatile uint8_t dht_ready_flag = 0; // 수신 완료 플래그

volatile uint16_t last_time = 0;

// 상태를 기억할 전역 변수 추가 (for 문의 i, j 역할)
volatile uint8_t current_byte = 0; // 현재 채우고 있는 바이트 인덱스 (0 ~ 4)
volatile int8_t current_bit = 7;   // 현재 채우고 있는 비트 위치 (7 -> 0 감소, 음수가 되어야 하므로 int8_t 사용)

// Timer1 초기화 (마이크로초 측정을 위함)
void Timer1_Init(void) {
    TCCR1A = 0;
    // Prescaler 8 설정: 16MHz / 8 = 2MHz (1 카운트당 0.5us)
    TCCR1B = (1 << CS11); 
    TCNT1 = 0;
}

// DHT11 통신 시작 트리거 함수 (main에서 호출)
void DHT11_Trigger(void) {
    current_byte = 0;
    current_bit = 7;
    
    EIMSK &= ~(1 << INT0);   
    
    DDRD |= (1 << DHT_PIN);   // 출력 모드
    PORTD &= ~(1 << DHT_PIN); // LOW 출력
    _delay_ms(20);            // 20ms 대기 (Start 신호)
    PORTD |= (1 << DHT_PIN);  // HIGH 출력
    // _delay_us(30);            // 20~40us 대기 후 센서 응답 확인

    DDRD &= ~(1 << DHT_PIN);
    
   
    bit_count = 0;
    for(int i=0; i<5; i++) dht_data[i] = 0;
    dht_ready_flag = 0;
    
    // Falling Edge(하강 에지)에서 인터럽트 발생 설정
    EICRA |= (1 << ISC01); 
    EICRA &= ~(1 << ISC00);
    
    EIFR |= (1 << INTF0);     // 펜딩된 인터럽트 깃발 클리어
    EIMSK |= (1 << INT0);     // INT0 활성화
}

// 외부 인터럽트 0 (D2 핀 하강 에지 감지)
ISR(INT0_vect) {
    uint16_t current_time = TCNT1;
    uint16_t time_diff = current_time - last_time; 
    last_time = current_time;

    // 1. 초기 준비 신호 2번은 데이터가 아니므로 카운트만 하고 패스
    if (bit_count < 2) {
        bit_count++;
        return;
    }

    if (time_diff > 200) {
        dht_data[current_byte] |= (1 << current_bit); // 해당 위치의 비트를 1로 만듦
    }

    current_bit--; 

    if (current_bit < 0) {       // 8개의 비트(7~0)를 모두 채웠다면
        current_bit = 7;         // 다시 최상위 비트로 위치 리셋
        current_byte++;          // 다음 바이트 배열로 방 이동
    }

    bit_count++; // 전체 신호 개수 카운트 증가

    
    if (bit_count >= 42) { 
        EIMSK &= ~(1 << INT0); 
        dht_ready_flag = 1;    
    }
}
int main(void) {
    
    
    DDRB |= (1 << LED_PIN);
    UART0_init();
    Timer1_Init(); // 타이머 1 초기화 추가
    sei();         // 전역 인터럽트 활성화

    while(1) {
        // 1. 센서 트리거 (논블로킹 타이머 로직과 결합 권장, 여기서는 간단히 2초마다)
        DHT11_Trigger();
        
        // 2초 대기 (실제로는 timer0_millis를 이용해 2초마다 Trigger하도록 논블로킹으로 수정하는 것이 좋습니다)
        _delay_ms(2000); 

        // 2. 수신 완료 시 데이터 처리 및 UART 출력 (Bottom Half)
        if (dht_ready_flag == 1) {
            // 체크섬 검증
            if (dht_data[4] == ((dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) & 0xFF)) {

                UART0_print_string(" Temp: ");
                UART0_print_1_byte_number(dht_data[2]);
                UART0_print_string(" Humid: ");
                UART0_print_1_byte_number(dht_data[0]);
                UART0_print_string("\r\n");

                if (dht_data[2] > 23) {
                    PORTB |= (1 << LED_PIN); 
                } else {
                    PORTB &= ~(1 << LED_PIN); 
                }
            } else {
                UART0_print_string("Checksum Error\r\n");
            }
            dht_ready_flag = 0; // 플래그 초기화
        }
    }
}