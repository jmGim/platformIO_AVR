// #include <avr/io.h>

// int main(void)
// {
//     // PORTB의 0번 핀을 출력으로 설정 (LED 연결)
//     DDRB = 0x38; 
//     DDRC &= ~((1 << PC0) | (1 << PC1)); 
//     DDRD &= ~((1 << PD6) | (1 << PD7));
//     // 아날로그 비교기 설정
//     // ACD=0 (켜기), ACBG=0 (AIN0 핀 사용), 나머지 기본값
//     ACSR = 0x00; 

//     while(1)
//     {
//         // ACSR 레지스터의 ACO 비트(5번 비트) 확인
//         if (ACSR & (1 << ACO)) 
//         {
//             // AIN0 >= AIN1 인 경우: LED 켜기
//             PORTB = 0x01;
//         }
//         else
//         {
//             // AIN0 < AIN1 인 경우: LED 끄기
//             PORTB = 0x00;
//         }
//     }
// }


////////// PD6. PD7 --> PC0, PC1 이용 ///////////////
// #include <Arduino.h>
// #include <avr/io.h>

// void setup() {
//     Serial.begin(9600);

//     DDRB |= 0x38;  // set LED output (11, 12, 13)
//     DDRC |= 0x00;
//     ACSR = 0x00;  // ACD=0 (켜기), ACBG=0 (AIN0 핀 사용), 나머지 기본값
//     ADCSRA &= ~(1 << ADEN);
//     ADCSRB |= (1 << ACME);
//     ADMUX &= ~0x07; // AIN1 만 변경 가능
// }

// void loop() {
//                         // bit(5)
//     PORTB = (ACSR & (1 << ACO)) ? 0x38 : 0x00;
//     Serial.println((ACSR & (1 << ACO)));
//     delay(500);
// }




////////// PD6. PD7 --> A0, A1 이용 ///////////////
// #include <avr/io.h>

// void ADC_init(void) {
//     ADMUX = (1 << REFS0);
//     ADCSRA = (1 << ADEN)
//            | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
// }

// uint16_t ADC_read(uint8_t channel) {
//     ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

//     ADCSRA |= (1 << ADSC);
//     while (ADCSRA & (1 << ADSC));

//     return ADC;
// }

// int main(void) {
//     DDRB |= (1 << PB5);

//     ADC_init();

//     while (1) {
//         uint16_t a0 = ADC_read(0);  // A0
//         uint16_t a1 = ADC_read(1);  // A1

//         if (a1 > a0) {
//             PORTB |= (1 << PB5);
//         } else {
//             PORTB &= ~(1 << PB5);
//         }
//     }
// }



