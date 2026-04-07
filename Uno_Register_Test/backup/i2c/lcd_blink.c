#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd1602_i2c.h"
#include "my_uart.h"

int main() { 
    i2c_init();
    lcd_init();
    uart_init();

     DDRB |= (1<<PB5);
     PORTB |= (1<<PB5);
    
    while(1) {
        lcd_init();
        lcd_gotoxy(0, 0);
        lcd_print("Hello, jongmin!!!");
        PORTB &= ~(1<<PB5);
        _delay_ms(1000);

        
        lcd_gotoxy(0, 1);
        lcd_print("AVR I2C System");
        PORTB |= (1<<PB5);
        
        lcd_command(0x0C); // Display ON, Cursor OFF
        uart_print("Display ON");
        uart_print("\r\n");
        _delay_ms(1000);

        lcd_command(0x08); // Display OFF, Cursor OFF
        uart_print("Display OFF");
        uart_print("\r\n");
        _delay_ms(1000);

        lcd_command(LCD_RETURNHOME);
        uart_print("original cursor position ");
        uart_print("\r\n");
        lcd_command(0x0F); // Display ON, Cursor ON,  blinking ON
        uart_print("CURSOR blinking ON");
        uart_print("\r\n");
        _delay_ms(1000);

        lcd_command(0x09); // Display OFF, Cursor ON
        uart_print("CURSOR blinking OFF");
        uart_print("\r\n");
        _delay_ms(1000);

        lcd_clear();  // text clear
        uart_print("LCD Text Clear");
        uart_print("\r\n");
        _delay_ms(100);
        PORTB &= ~(1<<PB5);

        lcd_command(0x0C); // Display ON, Cursor OFF : no text display 
        uart_print("LCD Clear continue");
        uart_print("\r\n");
        _delay_ms(1000); 

        lcd_command(0x08); // Display OFF, Cursor OFF
        uart_print("Display OFF");
        uart_print("\r\n");
        _delay_ms(1000);
        
        lcd_gotoxy(0, 0);
        lcd_print("Hello, jongmin!!!");
        _delay_ms(1000);
        
        PORTB &= ~(1<<PB5);
        lcd_bg_off();  // background OFF
        uart_print("Background OFF");
        uart_print("\r\n");
        _delay_ms(1000);

        PORTB |= (1<<PB5);
        lcd_bg_on();
        uart_print("Background ON");
        uart_print("\r\n");
        _delay_ms(1000);

        PORTB &= ~(1<<PB5);
        lcd_bg_off();
        uart_print("Background OFF");
        uart_print("\r\n");
        uart_print("\r\n");
        _delay_ms(1000);
        
    }
}