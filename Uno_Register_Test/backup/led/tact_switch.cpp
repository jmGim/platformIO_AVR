/////////////////////////////////////////////////
// Tact Switch - Internal Pull UP , External Pull-UP ( )


// #define F_CPU 16000000UL
// #include <Arduino.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include <stdio.h>

// int main() {
//     int data = 0x01;
  
//     // Set PD2 as input (0)
//     DDRD &= ~(1 << DDD7); 
//     DDRD &= ~(1 << DDD6);
//     DDRD &= ~(1 << DDD5);

//     // // Enable internal pull-up for PD2
//     // PORTD |= (1 << PORTD7); 
//     // PORTD |= (1 << PORTD6); 
//     // PORTD |= (1 << PORTD5); 

//     // Set PB5 (LED) as output (1)
//     DDRB |= (1 << DDB7);
//     DDRB |= (1 << DDB6);
//     DDRB |= (1 << DDB5);

//     while(1) {
//         if ( (PIND7 & data) == 0 )   // 1. 버튼 눌림 감지 (Active Low 가정)
//         {
//            // _delay_ms(20);          // 2. 채터링 제거
            
//             data <<= 1;             // 3. 데이터 이동 (Shift)
//             if(data == 0) data = 1; // 4. 회전 처리
            
            
//             while( (PIND7 & data) == 1 ); // 6. 버튼 뗄 때까지 대기 (중요)
//            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
//         }

//         if ( (PIND6 & data) == 0 )   // 1. 버튼 눌림 감지 (Active Low 가정)
//         {
//            // _delay_ms(20);          // 2. 채터링 제거
            
//             data <<= 1;             // 3. 데이터 이동 (Shift)
//             if(data == 0) data = 1; // 4. 회전 처리
            
            
//             while( (PIND6 & data) == 1 ); // 6. 버튼 뗄 때까지 대기 (중요)
//            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
//         }


//         if ( (PIND5 & data) == 0 )   // 1. 버튼 눌림 감지 (Active Low 가정)
//         {
//            // _delay_ms(20);          // 2. 채터링 제거
            
//             data <<= 1;             // 3. 데이터 이동 (Shift)
//             if(data == 0) data = 1; // 4. 회전 처리
            
            
//             while( (PIND5 & data) == 1 ); // 6. 버튼 뗄 때까지 대기 (중요)
//            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
//         }
//     }
// }




// Tact Switch - Pull Down Code 



#define F_CPU 16000000UL
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

int main() {
    int data = 0x01;
  
    // Set PD2 as input (0)
    DDRD &= ~(1 << DDD7); 
    DDRD &= ~(1 << DDD6);
    DDRD &= ~(1 << DDD5);

    // // Enable internal pull-up for PD2
    // PORTD |= (1 << PORTD7); 
    // PORTD |= (1 << PORTD6); 
    // PORTD |= (1 << PORTD5); 

    // Set PB5 (LED) as output (1)
    DDRB |= (1 << DDB7);
    DDRB |= (1 << DDB6);
    DDRB |= (1 << DDB5);

    while(1) {
        if ( (PIND7 & data) == 1 )   // 1. 버튼 눌림 감지 (Active Low 가정)
        {
            // _delay_ms(20);          // 2. 채터링 제거
            
            data <<= 1;             // 3. 데이터 이동 (Shift)
            if(data == 0) data = 1; // 4. 회전 처리
            
            
            while( (PIND7 & data) == 0 ); // 6. 버튼 뗄 때까지 대기 (중요)
            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
        }

        if ( (PIND6 & data) == 1 )   // 1. 버튼 눌림 감지 (Active Low 가정)
        {
            // _delay_ms(20);          // 2. 채터링 제거
            
            data <<= 1;             // 3. 데이터 이동 (Shift)
            if(data == 0) data = 1; // 4. 회전 처리
            
            
            while( (PIND6 & data) == 0 ); // 6. 버튼 뗄 때까지 대기 (중요)
            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
        }


        if ( (PIND5 & data) == 1 )   // 1. 버튼 눌림 감지 (Active Low 가정)
        {
            // _delay_ms(20);          // 2. 채터링 제거
            
            data <<= 1;             // 3. 데이터 이동 (Shift)
            if(data == 0) data = 1; // 4. 회전 처리
            
            
            while( (PIND5 & data) == 0 ); // 6. 버튼 뗄 때까지 대기 (중요)
            // _delay_ms(20);          // 7. 뗄 때 채터링 제거
        }
    }
}



