#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi.h"
#include "mfrc522.h"
#include "UART0.h"

#define IRQ_PIN PD2
#define LED_PIN PD7

volatile uint8_t card_detected_flag = 0;

void EXTI_Init(void) {
    DDRD &= ~(1 << IRQ_PIN);
    PORTD |= (1 << IRQ_PIN); 
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00); 
    EIMSK |= (1 << INT0);
}

// [수정됨] ISR은 최대한 짧게 플래그만 세팅하고 빠져나옵니다. (SPI 함수 호출 금지)
ISR(INT0_vect) {
    card_detected_flag = 1; 
    // MFRC522_WriteReg(ComIrqReg, 0x7F); // 인터럽트 플래그 전체 클리어
}

// 1단계: REQA 명령
void MFRC522_Send_REQA(void) {
    MFRC522_WriteReg(CommandReg, 0x00);         // Idle
    MFRC522_WriteReg(ComIrqReg, 0x7F);          // 인터럽트 클리어
    
    // [핵심 수정] SetBitMask 금지! 직접 0x80을 써서 FIFO를 강제로 비웁니다.
    MFRC522_WriteReg(FIFOLevelReg, 0x80);       

    MFRC522_WriteReg(BitFramingReg, 0x07);      // 7비트 전송
    MFRC522_WriteReg(FIFODataReg, 0x26);        // REQA 명령

    MFRC522_WriteReg(CommandReg, 0x0C);         // Transceive
    MFRC522_SetBitMask(BitFramingReg, 0x80);    // StartSend
}

// 2단계: Anti-collision 명령
void MFRC522_Send_AntiColl(void) {
    MFRC522_WriteReg(CommandReg, 0x00);
    MFRC522_WriteReg(ComIrqReg, 0x7F);
    
    // [핵심 수정] 여기서도 직접 0x80을 써서 FIFO를 완전히 비웁니다.
    MFRC522_WriteReg(FIFOLevelReg, 0x80);       

    MFRC522_WriteReg(BitFramingReg, 0x00);      // 8비트 전송
    MFRC522_WriteReg(FIFODataReg, 0x93); 
    MFRC522_WriteReg(FIFODataReg, 0x20);

    MFRC522_WriteReg(CommandReg, 0x0C); 
    MFRC522_SetBitMask(BitFramingReg, 0x80); 
}

int main(void) {
    UART0_init();
    MFRC522_Init();
    EXTI_Init();
    DDRD |= (1 << LED_PIN);
    
    sei();
    UART0_print_string("RFID Reader Started...\r\n");

    uint8_t state = 0; 

    while (1) {
        // [상태 0] 카드가 있는지 탐색 (REQA)
        if (state == 0) {
            card_detected_flag = 0;
            MFRC522_Send_REQA();
            state = 1;
            
            _delay_ms(10); 

            if (card_detected_flag == 0) {
                state = 0;     
                _delay_ms(90); 
                continue;      
            }
        }

        // 인터럽트가 발생했다면 (카드가 대답함!)
        if (card_detected_flag == 1) {
            card_detected_flag = 0; 
            
            // 1. 하드웨어 인터럽트 찌꺼기 클리어
            MFRC522_WriteReg(ComIrqReg, 0x7F); 
            
            // 2. [가장 중요한 핵심] 전송 중지 및 Idle 모드 복귀
            // 이 처리가 없으면 MFRC522가 계속 Transceive 상태에 빠져 FIFO가 오작동합니다.
            MFRC522_ClearBitMask(BitFramingReg, 0x80); // StartSend 비트 끄기
            MFRC522_WriteReg(CommandReg, 0x00);        // Idle 명령으로 상태 강제 복귀

            // 3. 버퍼에 들어온 데이터 크기 확인
            uint8_t fifo_length = MFRC522_ReadReg(FIFOLevelReg);

            // [상태 1 -> 2] REQA에 대한 응답(ATQA)을 성공적으로 수신함
            if (state == 1 && fifo_length >= 2) {
                MFRC522_Send_AntiColl();
                state = 2; 
                _delay_ms(10);

                if (card_detected_flag == 0) {
                    state = 0; 
                }
            } 
            // [상태 2 -> 완료] Anti-collision 응답 수신 (정상적인 UID는 BCC포함 5바이트)
            else if (state == 2 && fifo_length >= 5) {
                // PORTD |= (1 << LED_PIN);
                UART0_print_string("Card UID: ");
                
                // 새로 만든 Burst Read 함수로 5바이트를 한 번에 안전하게 가져옴
                uint8_t uid_buffer[5];
                // 가장 안전한 방법: 1바이트씩 레지스터에 접근하여 순차적으로 읽어옴
                for (uint8_t i = 0; i < 5; i++) {
                    uid_buffer[i] = MFRC522_ReadReg(FIFODataReg);
                }   
                // MFRC522_ReadFIFO(5, uid_buffer); 

                // [체크섬(BCC) 검증 로직] 
            // 앞의 4바이트(UID)를 XOR 연산한 결과가 5번째 바이트(BCC)와 일치해야 정상입니다.
                uint8_t bcc_check = uid_buffer[0] ^ uid_buffer[1] ^ uid_buffer[2] ^ uid_buffer[3];
                
                if (bcc_check == uid_buffer[4]) {
                    PORTD |= (1 << LED_PIN);
                    UART0_print_string("Valid UID Read: ");
                    
                    // 실제 UID 4바이트 출력 (5번째는 BCC이므로 출력 생략)
                    for (uint8_t i = 0; i < 4; i++) {
                        UART0_print_hex(uid_buffer[i]);
                    }
                    UART0_print_string("\r\n");
                    
                    
                }
                else {
                    UART0_print_string("UID Checksum Error!\r\n");
                }
                    _delay_ms(1000); // 중복 인식 방지
                    PORTD &= ~(1 << LED_PIN);
                    state = 0; 
            }
        } 
    }
    return 0;
}
// UART0_print_string("state : ");

// UART0_print_1_byte_number(state);

// UART0_print_string("\r\n");