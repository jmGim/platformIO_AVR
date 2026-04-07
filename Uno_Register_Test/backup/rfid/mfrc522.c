#include "mfrc522.h"
#include <util/delay.h>
// MFRC522 레지스터에 값 쓰기
void MFRC522_WriteReg(uint8_t reg, uint8_t value) {
    SPI_PORT &= ~(1 << SS_PIN); // SS LOW (칩 선택)
    SPI_Transmit(reg & 0x7E);   // 쓰기 모드 주소 전송 (최하위 0, 최상위 0)
    SPI_Transmit(value);        // 데이터 전송
    SPI_PORT |= (1 << SS_PIN);  // SS HIGH (칩 선택 해제)
}

// MFRC522 레지스터에서 값 읽기
uint8_t MFRC522_ReadReg(uint8_t reg) {
    uint8_t value;
    SPI_PORT &= ~(1 << SS_PIN); // SS LOW
    SPI_Transmit(reg | 0x80);   // 읽기 모드 주소 전송 (최상위 비트 1 설정)
    value = SPI_Transmit(0x00); // 더미 데이터(0x00)를 보내고 값 수신
    SPI_PORT |= (1 << SS_PIN);  // SS HIGH
    return value;
}

// 레지스터의 특정 비트만 1로 설정
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadReg(reg);
    MFRC522_WriteReg(reg, tmp | mask);
}

// 레지스터의 특정 비트만 0으로 해제
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadReg(reg);
    MFRC522_WriteReg(reg, tmp & (~mask));
}

// MFRC522 초기화 함수
void MFRC522_Init(void) {
    SPI_Init();
    
    // 1. 소프트웨어 리셋 (CommandReg에 SoftReset 명령(0x0F) 하달)
    MFRC522_WriteReg(CommandReg, 0x0F);
    _delay_ms(50);
    // 리셋 안정화 대기 (임시 딜레이 대체 로직, 보통 while로 TModeReg가 안정될 때까지 기다립니다)
    // 실제 환경에서는 _delay_ms(50) 등을 사용할 수 있습니다.

    // 2. 타이머 설정 (TModeReg, TPrescalerReg)
    // TAuto=1: 송수신 시작 시 타이머 자동 시작
    // 분주비 셋팅 및 13.56MHz 래퍼런스 클록 사용
    MFRC522_WriteReg(TModeReg, 0x8D);      
    MFRC522_WriteReg(TPrescalerReg, 0x3E); 
    
    // 타이머 리로드 값 설정 (TReloadRegH/L) : 약 25ms 타이머 아웃
    MFRC522_WriteReg(TReloadRegH, 0x00);
    MFRC522_WriteReg(TReloadRegL, 0x1E);

    // 3. 통신 방식 설정
    // 100% ASK 변조 방식 사용 (ISO/IEC 14443A 표준)
    MFRC522_WriteReg(TxASKReg, 0x40); 
    // CRC 코프로세서 초기값 설정 (0x6363)
    MFRC522_WriteReg(ModeReg, 0x3D);  

    // 4. [인터럽트(IRQ) 핵심 설정]
    // ComIEnReg (0x02): IRQ 핀 활성화 및 트리거 조건 설정
    // 최상위 비트(IRqInv) 0 = Active LOW (인터럽트 발생 시 IRQ 핀이 LOW로 떨어짐)
    // RxIEn (bit 5) = 1 : 데이터 수신 완료 시 인터럽트 발생
    // TxIEn (bit 4) = 1 : 데이터 송신 완료 시 인터럽트 발생
    // IdleIEn (bit 0) = 1 : 상태가 Idle로 돌아가면 인터럽트 발생
    MFRC522_WriteReg(ComIEnReg, 0x20); // 예: 수신 완료 인터럽트 허용
    MFRC522_WriteReg(DivIEnReg, 0x80);
    // 안테나 전원 인가
    MFRC522_AntennaOn();
}

void MFRC522_AntennaOn(void) {
    uint8_t value = MFRC522_ReadReg(TxControlReg);
    if ((value & 0x03) != 0x03) {
        MFRC522_SetBitMask(TxControlReg, 0x03); // TX1, TX2 핀 활성화
    }
}



// FIFO 배열 연속 읽기 (Burst Read)
void MFRC522_ReadFIFO(uint8_t count, uint8_t *buffer) {
    if (count == 0) return;
    SPI_PORT &= ~(1 << SS_PIN);       // SS LOW (칩 선택)
    SPI_Transmit(FIFODataReg | 0x80); // 읽기 모드 주소 한 번만 전송
    
    for (uint8_t i = 0; i < count; i++) {
        buffer[i] = SPI_Transmit(0x00); // 클록만 발생시키며 데이터 연속 수신
    }
    SPI_PORT |= (1 << SS_PIN);        // SS HIGH (칩 선택 해제)
}