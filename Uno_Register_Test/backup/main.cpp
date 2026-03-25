// #include <Arduino.h> // PlatformIO에서는 이 헤더가 필수입니다.

// boolean flag = false;
// void setup() {
//   // DDRB 레지스터 주소 (0x24) 직접 접근 -> 5번 비트 출력 설정
//   *((volatile unsigned char *)0x24) |= 0x30;
  
// }

// void loop() {
//   // PORTB 레지스터 (0x25) -> LED ON
//   *((volatile unsigned char *)0x25) |= 0x10;
  
//   delay(500);

//   // PORTB 레지스터 (0x25) -> LED OFF
//   if (flag == true){
//     *((volatile unsigned char *)0x25) |= 0x20;

//   } else *((volatile unsigned char *)0x25) &= ~0x20;

//  *((volatile unsigned char *)0x25) &= ~0x10;
//   // PORTB 레지스터 (0x25) -> LED OFF  
//   delay(500);

//   flag = !flag;

// }