// #include <avr/io.h>
// #include <avr/interrupt.h>

// #define LED_PIN PB5 // 아두이노 우노 내장 LED (D13)
// #define BTN_PIN PD2 // 외부 인터럽트 0 (D2)

// volatile int state = 0;			// 현재 LED의 상태
// void int0_init(void) {
//     // 1. PD2 핀을 입력으로 설정 및 내부 풀업 저항 활성화
//     DDRD &= ~(1 << BTN_PIN);
//     PORTD |= (1 << BTN_PIN);

//     // 2. 인터럽트 감지 조건 설정 (EICRA 레지스터)
//     // ISC01 = 1, ISC00 = 0 : 하강 에지(Falling Edge) 감지
//     // 스위치를 눌러 전압이 GND로 떨어지는 순간을 포착함
//     EICRA |= (1 << ISC01);
//     EICRA &= ~(1 << ISC00);

//     // 3. INT0 외부 인터럽트 허용 (EIMSK 레지스터)
//     EIMSK |= (1 << INT0);

//     // 4. 전역 인터럽트 활성화
//     sei(); 
// }

// // INT0 외부 인터럽트 서비스 루틴(ISR)
// ISR(INT0_vect) {
//     // LED 상태 반전 (Toggle)
//     PORTB ^= (1 << LED_PIN); 
// }

// int main(void) {
//     // LED 핀을 출력으로 설정
//     DDRB |= (1 << LED_PIN);

//     // 인터럽트 초기화
//     int0_init();

//     while(1) {
//         // 메인 루프는 비워둠. 
//         // CPU는 다른 작업을 하다가 버튼이 눌리면 즉각 ISR을 실행함.
//     }
//     return 0;
// }



//////////////////////// int0_state_main.c

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN PB5 // 아두이노 우노 내장 LED (D13)
#define BTN_PIN PD2 // 외부 인터럽트 0 (D2)
#define F_CPU 16000000UL

// 1. 1ms마다 증가할 전역 변수
// ISR 내부에서 값이 변경되므로 컴파일러 최적화 방지를 위해 반드시 volatile 선언
volatile unsigned long timer0_millis = 0;


volatile int state = 0;			// 현재 LED의 상태


// 2. 타이머0 하드웨어 초기화 함수
void Timer0_Init(void) {
    TCCR0A = 0x00; // 노멀 모드 (Normal mode)

    // 프리스케일러 64 설정 (CS01, CS00 비트 1)
    TCCR0B = (1 << CS01) | (1 << CS00);

    // 1ms(250틱)를 맞추기 위해 TCNT0 초기값 6 설정 (256 - 250 = 6)
    TCNT0 = 6;

    // 타이머0 오버플로 인터럽트 활성화
    TIMSK0 |= (1 << TOIE0);

    // 전역 인터럽트 활성화 (필수)
    sei();
}

// 3. 타이머0 오버플로 인터럽트 서비스 루틴 (1ms마다 자동 실행)
ISR(TIMER0_OVF_vect) {
    TCNT0 = 6;         // 다음 1ms 간격을 위해 레지스터 초기값 재장전
    timer0_millis++;   // 밀리초 카운터 1 증가
}




// 4. 안전하게 값을 읽어오는 커스텀 millis() 함수
unsigned long millis_custom(void) {
    unsigned long m;
    
    // 상태 레지스터(SREG) 백업
    uint8_t oldSREG = SREG; 

    // 인터럽트 비활성화 (원자적 읽기 보장)
    cli(); 
    
    // 4바이트(unsigned long) 변수 복사
    m = timer0_millis; 
    
    // 인터럽트 상태 원상 복구
    SREG = oldSREG; 

    return m;
}





void int0_init(void) {
    // 1. PD2 핀을 입력으로 설정 및 내부 풀업 저항 활성화
    DDRD &= ~(1 << BTN_PIN);
    PORTD |= (1 << BTN_PIN);

    // 2. 인터럽트 감지 조건 설정 (EICRA 레지스터)
    // ISC01 = 1, ISC00 = 0 : 하강 에지(Falling Edge) 감지
    // 스위치를 눌러 전압이 GND로 떨어지는 순간을 포착함
    // EICRA |= (1 << ISC01);
    // EICRA &= ~(1 << ISC00);

    // (1, 1) : Rising Edge
    EICRA |= (1 << ISC01);
    EICRA |= (1 << ISC00);

    // 3. INT0 외부 인터럽트 허용 (EIMSK 레지스터)
    EIMSK |= (1 << INT0);

    // 4. 전역 인터럽트 활성화
    sei(); 
}

// INT0 외부 인터럽트 서비스 루틴(ISR)
ISR(INT0_vect) {
    state = (state + 1) % 2;			// LED 상태 전환
}

int main(void) {

    Timer0_Init(); // 타이머 시작
    
    DDRB |= (1 << PB5); // 우노 내장 LED(13번 핀) 출력 설정

    unsigned long prev_time = 0;

  
    // LED 핀을 출력으로 설정
    DDRB |= (1 << LED_PIN);

    // 인터럽트 초기화
    int0_init();

    while(1){
        unsigned long current_time = millis_custom();

        // 20ms마다 LED 토글 (논블로킹)
        if (current_time - prev_time >= 20) {
            prev_time = current_time;
            if(state == 1){
				PORTB |= (1 << LED_PIN); 
			}
			else{
				PORTB &=~(1 << LED_PIN); 
			}
        }
			
	}
    return 0;
}