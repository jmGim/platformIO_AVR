// // // #ifndef NEO6M_GPS_H_
// // // #define NEO6M_GPS_H_

// // // #define F_CPU 16000000UL // Set your clock speed
// // // #include <avr/io.h>
// // // #include <stdio.h>
// // // #include <string.h>
// // // #include <stdlib.h>
// // // #include <avr/interrupt.h>

// // // // UART Configuration
// // // #define BAUD 9600
// // // #define MYUBRR F_CPU/16/BAUD-1

// // // // GPS Functions
// // // void GPS_Init(void);
// // // char GPS_Read(void);
// // // void GPS_Parse(char* nmeaSentence, char* lat, char* lon);

// // // #endif /* NEO6M_GPS_H_ */



// // #ifndef NEO6M_GPS_H
// // #define NEO6M_GPS_H

// // #include <avr/io.h>
// // #include <string.h>

// // // Structure to hold parsed GPS data
// // typedef struct {
// //     char time[15];
// //     char latitude[15];
// //     char lat_dir;       // N or S
// //     char longitude[15];
// //     char lon_dir;       // E or W
// //     uint8_t is_valid;   // 1 if GPS has a fix, 0 if not
// // } GPS_Data;

// // // Function prototypes
// // void GPS_init(void);
// // void GPS_read_GPGGA(GPS_Data *gps_info);

// // #endif /* NEO6M_GPS_H */



// /*
//  * neo6m_gps.h
//  * GY-NEO6MV2 GPS 모듈 드라이버 헤더 (ATmega328P)
//  *
//  * [하드웨어 연결]
//  *   GPS VCC  → Arduino 3.3V 또는 5V (모듈 스펙 확인)
//  *   GPS GND  → GND
//  *   GPS TX   → ATmega328P PD0 (RXD0, 핀 2)     ← GPS가 데이터를 보냄
//  *   GPS RX   → ATmega328P PD1 (TXD0, 핀 3)     ← 설정 명령 전송용 (선택)
//  *
//  * [중요 주의사항 - 시리얼 모니터에 값이 안 뜰 때]
//  *   Arduino Uno/Nano 보드를 사용하는 경우, USB-to-Serial 칩(CH340/FTDI)이
//  *   PD0/PD1에 항상 연결되어 있습니다.
//  *   GPS TX를 PD0에 연결하면 두 신호가 충돌(bus conflict)하여 데이터가
//  *   깨지거나 전혀 수신되지 않을 수 있습니다.
//  *
//  *   해결 방법:
//  *     A) USB-to-Serial 어댑터를 보드에서 분리하고 별도 어댑터로 PD1 모니터링
//  *     B) SoftwareSerial 라이브러리를 써서 다른 핀(예: PD4/PD5)으로 GPS 수신
//  *     C) ATmega328P 단독(순수 AVR) 환경에서 USB-to-Serial 어댑터를 PD1에만 연결
//  */

// #ifndef NEO6M_GPS_H
// #define NEO6M_GPS_H

// #include <avr/io.h>
// #include <string.h>
// #include <stdint.h>

// /* =======================================================================
//  * GPS_Data 구조체
//  * =======================================================================
//  * GPGGA 문장에서 파싱한 GPS 데이터를 담는 구조체.
//  *
//  * GPGGA 문장 형식:
//  *   $GPGGA,HHMMSS.ss,LLLL.ll,a,YYYYY.yy,a,q,NN,D.D,H.H,M,G.G,M,A.A,XXXX*hh
//  *    필드  1=시각   2=위도   3=N/S 4=경도    5=E/W 6=Fix 7=위성수 ...
//  * ===================================================================== */
// typedef struct {
//     char    time[15];       /* UTC 시각  예) "123519.00" → 12시 35분 19초  */
//     char    latitude[15];   /* 위도 (도분 형식) 예) "4807.038"              */
//     char    lat_dir;        /* 위도 방향 : 'N' (북위) 또는 'S' (남위)       */
//     char    longitude[15];  /* 경도 (도분 형식) 예) "01131.000"             */
//     char    lon_dir;        /* 경도 방향 : 'E' (동경) 또는 'W' (서경)       */
//     uint8_t is_valid;       /* GPS Fix 여부 : 1=유효, 0=미획득              */
// } GPS_Data;

// /* =======================================================================
//  * 함수 프로토타입
//  * ===================================================================== */

// /**
//  * @brief  GPS 모듈 핀(PD0, PD1)의 DDR 및 PORT를 초기화한다.
//  *         UART0_init() 호출 이후에 불러야 한다.
//  */
// void GPS_init(void);

// /**
//  * @brief  UART0를 통해 들어오는 NMEA 문장 중 GPGGA 문장을 찾아 파싱한다.
//  *         GPGGA 문장이 올 때까지 내부에서 반복 대기한다.
//  * @param  gps_info  파싱된 데이터를 저장할 GPS_Data 구조체 포인터
//  */
// void GPS_read_GPGGA(GPS_Data *gps_info);

// #endif /* NEO6M_GPS_H */




/*
 * neo6m_gps.h
 * GY-NEO6MV2 GPS 모듈 드라이버 헤더
 *
 * [하드웨어 연결]
 * GPS VCC  → Arduino 3.3V 또는 5V 
 * GPS GND  → GND
 * GPS TX   → Arduino 11번 핀 (PB3) !! 0번 핀에서 여기로 옮기세요 !!
 *
 * [해결된 사항]
 * GPS 수신을 PB3로 옮겨 아두이노 우노의 USB 통신 충돌 문제를 해결했습니다.
 */

#ifndef NEO6M_GPS_H
#define NEO6M_GPS_H

#include <avr/io.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    char    time[15];       
    char    latitude[15];   
    char    lat_dir;        
    char    longitude[15];  
    char    lon_dir;        
    uint8_t is_valid;       
} GPS_Data;

void GPS_init(void);
void GPS_read_GPGGA(GPS_Data *gps_info);

#endif /* NEO6M_GPS_H */