#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

void timer_servo_init();

volatile int8_t step = 0;
volatile uint8_t sec_counter = 0;

int main() {
    timer_servo_init();

    while (1) {

    }
    return 0;
}

ISR(TIMER1_OVF_vect) {

    sec_counter++;
    if (sec_counter >= 50) { // 50Hz -> 50(1s)
        sec_counter = 0;

        switch (step) {
            case 0:
                OCR1A = 150;
                step = 1;
                break;
            case 1:
                OCR1A = 625;
                step = 2;
                break;
            case 2:
                OCR1A = 375;
                step = 0;
                break;
        }
    }
}

void timer_servo_init() {
    cli();

    DDRB |= (1 << PB1); // PB1 output

    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12);

    TCCR1B |= (1 << CS11) | (1 << CS10);    
    TIMSK1 |= (1 << TOIE1);                 // enable ovf intr

    ICR1 = 4999; // 20ms

    OCR1A = 375;

    sei();
}