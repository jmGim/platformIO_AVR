#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART0.h" // 라이브러리 포함
#define F_CPU 16_000_000UL


#define DHT_PIN PD2 // 외부 인터럽트 0 (D2)
#define LED_PIN PB5 // 아두이노 D13

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);


// 1. 1ms마다 증가할 전역 변수
// ISR 내부에서 값이 변경되므로 컴파일러 최적화 방지를 위해 반드시 volatile 선언
volatile unsigned long timer0_millis = 0;

// 2. 타이머0 하드웨어 초기화 함수
void Timer0_Init(void) {
    TCCR0A = 0x00; // 노멀 모드 (Normal mode)

    // 프리스케일러 64 설정 (CS01, CS00 비트 1)
    TCCR0B = (1 << CS01) | (1 << CS00);  //  CS02 CS01 CS00 : 0 1 1 --> clk/64 from prescaler  8 times difference

    // 1ms(250틱)를 맞추기 위해 TCNT0 초기값 6 설정 (256 - 250 = 6)
    TCNT0 = 6;  // compare timer clock

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

// DDRD &= ~(1 << DHT_PIN);
//  PORTD &= ~(1<<DHT_PIN);
//     _delay_ms(20);
//     PORTD |= (1<<DHT_PIN);
//     _delay_us(1);
//     PORTD &= ~(1<<DHT_PIN);
//     _delay_us(39);


//     ////////////
//     if((PIND & (1 << DHT_PIN))) 
//     _delay_us(80);

//     if(!(PIND & (1 << DHT_PIN))) 
//     _delay_us(80);
// ... (위쪽 전역 변수 및 타이머 설정 코드는 기존과 동일) ...

int dht_init () {
    // 안전한 데이터 수신을 위해 변수들을 0으로 명시적 초기화합니다.
    uint8_t data[5] = {0, 0, 0, 0, 0}; 
    uint8_t int_humid = 0, int_temp = 0;
    uint8_t double_humid = 0, double_temp = 0;
    uint8_t checksum = 0;

    DDRB |= (1 << LED_PIN);
    
    // DHT11은 연속 측정 시 최소 1초~2초의 딜레이가 필요합니다.
    _delay_ms(2000); 

    // 1. MCU Start 신호 전송 (출력 모드)
    DDRD |= (1 << DHT_PIN); 
    PORTD &= ~(1 << DHT_PIN); // LOW 신호 전송
    _delay_ms(20);            // 최소 18ms 이상 유지
    PORTD |= (1 << DHT_PIN);  // HIGH로 전환
    _delay_us(30);            // 20~40us 대기 후 센서 응답 확인

    // 2. 센서 데이터 수신을 위해 입력 모드로 전환 (핵심 수정 사항)
    DDRD &= ~(1 << DHT_PIN);  

    // 3. DHT11 준비 응답 신호 동기화 대기
    // while(PIND & (1 << DHT_PIN));    // LOW로 떨어질 때까지 대기
    // while(!(PIND & (1 << DHT_PIN))); // HIGH로 올라갈 때까지 대기
    // while(PIND & (1 << DHT_PIN));    // 실제 데이터 전송 전 다시 LOW로 떨어질 때까지 대기

    // DDRD &= ~(1 << DHT_PIN);
    // PORTD &= ~(1<<DHT_PIN);
    // _delay_ms(20);
    // PORTD |= (1<<DHT_PIN);
    // _delay_us(1);
    // PORTD &= ~(1<<DHT_PIN);
    // _delay_us(39);


    ////////////
    if(!(PIND & (1 << DHT_PIN))) 
        PIND &= ~(1<<DHT_PIN);
    _delay_us(80);

    if((PIND & (1 << DHT_PIN))) 
        PIND |= (1<<DHT_PIN);
    _delay_us(80);


    if(!(PIND & (1 << DHT_PIN)))
        PIND &= ~(1<<DHT_PIN);
    _delay_us(10);

    // 4. 40비트(5바이트) 데이터 읽기 루프
    for (uint8_t j=0; j<5; j++) { 
        uint8_t result=0;
        for(uint8_t i=0; i<8; i++) {
            while(!(PIND & (1<<DHT_PIN))); // 비트 시작 (HIGH로 올라갈 때까지 대기)
            _delay_us(30);                 // 30us 대기 후 상태 확인 ('0'은 26~28us, '1'은 70us 유지됨)
            if(PIND & (1<<DHT_PIN)) {      // 30us 후에도 여전히 HIGH라면 이 비트는 '1'
                result |= (1<<(7-i));
                while(PIND & (1<<DHT_PIN));// 해당 비트의 HIGH 신호가 끝날 때까지 대기
            }
        }
        data[j] = result;
    }

    // 5. 체크섬 검증 및 올바른 인덱스 매핑
    // 상위 4바이트의 합이 5번째 바이트(체크섬)와 일치하는지 확인 (오버플로를 방지하기 위해 0xFF 마스킹)
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        checksum = data[4];
        int_humid = data[0];       // 습도 정수
        double_humid = data[1];    // 습도 소수
        int_temp = data[2];        // 온도 정수 (인덱스 수정됨)
        double_temp = data[3];     // 온도 소수 (변수명 중복 덮어쓰기 수정됨)
        
        // printf("humid : %d\r\n", int_humid); // 정상 수신 시 확인용 출력
    } else {
        // 통신 에러 또는 체크섬 실패 시 예외 처리 (여기서는 0을 반환하도록 처리)
        return 0; 
    } 
    
    // 통신 종료 후 핀 상태를 안정적으로 유지하기 위해 출력 모드 HIGH로 복귀
    DDRD |= (1<<DHT_PIN); 
    PORTD |= (1<<DHT_PIN); 

    return int_temp; // 정상적으로 파싱된 온도값 반환
}




int main() { 
    uint8_t curr_temp;

    DDRB |= (1 << LED_PIN);
    UART0_init();
    stdout = &OUTPUT;


    while(1){

        curr_temp = dht_init();
        UART0_print_1_byte_number(curr_temp); // 숫자 출력
        UART0_print_string("\r\n");

        if (curr_temp > 23){
            PORTB |= (1 << LED_PIN); 
        } else {
            PORTB &= ~(1 << LED_PIN); 
        }
        // _delay_ms(2000);

    }
}