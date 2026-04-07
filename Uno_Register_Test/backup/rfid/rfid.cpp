/* 인터럽트 사용 예제 */
#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

#define SS_PIN 10
#define RST_PIN 9
#define IRQ_PIN 2
#define LED_PIN 7

MFRC522 mfrc522(SS_PIN, RST_PIN);
volatile bool cardDetected = LOW;

// 등록된 카드 UID들 (4바이트 예시)
byte myCard1[4] = {0xB3, 0x27, 0x53, 0x56}; // 내 카드 (4byte)
byte myCard2[4] = {0xEE, 0x2D, 0x4F, 0x06}; // 내 키체인

void cardInterrupt() {
  cardDetected = HIGH;
}

bool compareUID(byte* uid, byte* target, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid[i] != target[i]) return false;
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), cardInterrupt, FALLING);

  // IRQ 설정 (카드 감지 시 인터럽트 발생)
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, 0x20); // 카드 감지 인터럽트만 활성화
  mfrc522.PCD_WriteRegister(mfrc522.DivIEnReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F); // 모든 인터럽트 클리어
  mfrc522.PCD_WriteRegister(mfrc522.FIFOLevelReg, 0x80); // FIFO 초기화
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Idle);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA); // 카드 요청
  Serial.println("Scan a card...");
}

void loop() {
  if (!cardDetected) return;

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // ✅ LED 제어: 등록된 카드인지 확인
  if (
    compareUID(mfrc522.uid.uidByte, myCard1, 4) ||
    compareUID(mfrc522.uid.uidByte, myCard2, 4)
  ) {
    digitalWrite(LED_PIN, HIGH);   // 등록된 카드면 ON
    delay(500);                    // 0.5초 유지
    digitalWrite(LED_PIN, LOW);    // 다시 OFF
  } else {
    digitalWrite(LED_PIN, LOW);    // 등록되지 않은 카드: 꺼짐 유지
  }

  // 카드 통신 종료
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // 다음 인터럽트 준비
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
}
