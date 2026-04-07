#include "spi.h"

void SPI_Init(void) {
    // MOSI, SCK, SS 핀을 출력으로 설정, MISO는 자동으로 입력으로 설정됨
    SPI_DDR |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    
    // SS 핀을 HIGH로 초기화 (SPI는 Active LOW이므로 비활성 상태 유지)
    SPI_PORT |= (1 << SS_PIN);

    // SPE(SPI 활성화), MSTR(마스터 모드), SPR0(분주비 16, 16MHz/16 = 1MHz)
    // MFRC522는 최대 10MHz까지 지원하므로 1MHz는 안정적인 속도입니다.
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

// 1바이트 송수신 함수
uint8_t SPI_Transmit(uint8_t data) {
    SPDR = data; // 데이터 레지스터에 값 쓰기 (전송 시작)
    while (!(SPSR & (1 << SPIF))); // 전송이 완료될 때까지 대기 (SPIF 플래그 확인)
    return SPDR; // 수신된 데이터 반환
}