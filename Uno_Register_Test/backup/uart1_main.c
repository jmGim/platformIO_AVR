#include <avr/io.h>
#include "UART1.h" // 라이브러리 포함

int main(void)
{
    UART1_init(); // 초기화

    char str[] = "Test using UART1 Library";
    uint8_t num = 128;

    UART1_print_string(str); // 문자열 출력
    UART1_print_string("\r\n");
    UART1_print_1_byte_number(num); // 숫자 출력
    UART1_print_string("\r\n");

    while(1) { }
    return 0;
}