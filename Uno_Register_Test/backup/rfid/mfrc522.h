#ifndef MFRC522_H
#define MFRC522_H

#include <avr/io.h>
#include "spi.h"

// MFRC522 주요 레지스터 주소 정의
#define CommandReg    0x01 << 1
#define ComIEnReg     0x02 << 1 // 인터럽트 활성화 레지스터
#define DivIEnReg     0x03 << 1
#define ComIrqReg     0x04 << 1 // 인터럽트 상태 플래그 레지스터
#define DivIrqReg     0x05 << 1
#define ErrorReg      0x06 << 1
#define Status2Reg    0x08 << 1
#define FIFODataReg   0x09 << 1
#define FIFOLevelReg  0x0A << 1
#define BitFramingReg 0x0D << 1
#define ModeReg       0x11 << 1
#define TxModeReg     0x12 << 1
#define RxModeReg     0x13 << 1
#define TxControlReg  0x14 << 1
#define TxASKReg      0x15 << 1
#define TModeReg      0x2A << 1
#define TPrescalerReg 0x2B << 1
#define TReloadRegH   0x2C << 1
#define TReloadRegL   0x2D << 1

void MFRC522_WriteReg(uint8_t reg, uint8_t value);
uint8_t MFRC522_ReadReg(uint8_t reg);
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
void MFRC522_Init(void);
void MFRC522_AntennaOn(void);

void MFRC522_ReadFIFO(uint8_t count, uint8_t *buffer);

#endif