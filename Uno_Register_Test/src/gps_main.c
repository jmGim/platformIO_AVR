

// /*
//  * gps_main.c
//  * GY-NEO6MV2 GPS 모듈 메인 프로그램 (ATmega328P)
//  *
//  * =========================================================================
//  * [하드웨어 연결 요약]
//  *
//  *   GY-NEO6MV2      ATmega328P (Arduino Uno/Nano 기준 핀 번호)
//  *   ──────────      ────────────────────────────────────────────
//  *   VCC         →  3.3V 또는 5V
//  *   GND         →  GND
//  *   TX          →  PD0 / RXD0 (Digital Pin 0)
//  *   RX          →  PD1 / TXD0 (Digital Pin 1) ← 선택 사항
//  *
//  *   PC (시리얼 모니터)
//  *   ──────────────────
//  *   USB-to-Serial TX  →  PD0 (Digital Pin 0)   ← !! 충돌 주의 !!
//  *   USB-to-Serial RX  ←  PD1 (Digital Pin 1)
//  *
//  * =========================================================================
//  * [!! 가장 중요한 하드웨어 주의사항 !!]
//  *
//  *   Arduino Uno / Nano 보드에서는 USB-to-Serial 칩이 PD0/PD1에 항상
//  *   연결되어 있습니다. GPS TX도 PD0에 연결하면 두 TX 신호가 같은 선을
//  *   동시에 구동하여 버스 충돌(bus conflict)이 발생합니다.
//  *
//  *   → 시리얼 모니터에 아무것도 안 뜨거나 깨진 문자가 나오는 주원인!
//  *
//  *   [권장 해결책]
//  *   방법 A (가장 안전):
//  *     별도의 USB-to-Serial 어댑터를 PD1(TXD)에만 연결하고,
//  *     Arduino 보드의 기본 USB 케이블은 전원 공급 전용으로만 사용.
//  *
//  *   방법 B (디버깅 시):
//  *     GPS를 연결하기 전에 아래 STEP 0 (RAW BYPASS 테스트)를 먼저 실행해
//  *     UART TX 출력이 PC에서 보이는지 먼저 확인.
//  *
//  * =========================================================================
//  * [디버깅 절차 (값이 안 뜰 때 순서대로 확인)]
//  *
//  *   STEP 0 → UART TX 동작 확인 (GPS 없이)
//  *             : #define TEST_UART_ONLY 를 활성화하고 빌드.
//  *               "UART OK" 메시지가 뜨면 UART는 정상.
//  *
//  *   STEP 1 → RAW GPS 수신 확인
//  *             : #define TEST_RAW_BYPASS 를 활성화하고 빌드.
//  *               시리얼 모니터에 NMEA 문장($GPGGA,...)이 그대로 보여야 함.
//  *               안 보이면 → GPS 배선 또는 보드 버스 충돌 문제.
//  *
//  *   STEP 2 → GPS 파싱 결과 확인
//  *             : 두 define 모두 비활성화 → 정상 동작 모드.
//  * =========================================================================
//  */

// #define F_CPU 16000000UL        /* 16 MHz (Arduino Uno/Nano 기본 클럭) */

// #include <avr/io.h>
// #include <util/delay.h>
// #include "UART0.h"
// #include "neo6m_gps.h"

// /*
//  * ── 디버깅 모드 선택 ──────────────────────────────────────────────────────
//  * 아래 두 #define 중 하나만 활성화(주석 해제)하거나,
//  * 둘 다 주석 처리하면 정상 GPS 파싱 모드로 동작한다.
//  */
// // #define TEST_UART_ONLY      /* STEP 0 : UART TX 만 테스트 (GPS 불필요) */
// // #define TEST_RAW_BYPASS     /* STEP 1 : GPS 원본 NMEA 스트림을 그대로 출력 */


// int main(void)
// {
//     /* ── 초기화 ────────────────────────────────────────────────────────── */

//     /*
//      * [초기화 순서가 중요]
//      * 반드시 UART0_init() 먼저 → 그 다음 GPS_init()
//      * 이유: GPS_init()에서 PORT 레지스터를 건드리는데,
//      *       UART가 먼저 활성화되어 있어야 UART 주변장치가 핀을 올바르게 제어함.
//      */
//     UART0_init();   /* UART0 하드웨어 초기화 (보율, 프레임 형식, 송수신 활성화) */
//     GPS_init();     /* GPS 핀 DDR/PORT 초기화 (PD0=입력+풀업, PD1=출력)        */


// /* ========================================================================
//  * STEP 0 : UART TX 전용 테스트 (GPS 모듈 없이 실행 가능)
//  * ========================================================================
//  * GPS를 연결하지 않고 UART TX가 PC에서 보이는지 먼저 확인한다.
//  * 이 메시지가 시리얼 모니터에 안 뜨면 → 보율 설정 오류 또는 배선 문제.
//  * 이 메시지가 뜨면 → UART TX는 정상. GPS 수신 쪽 문제 가능성.
//  * ====================================================================== */
// #ifdef TEST_UART_ONLY
//     while (1) {
//         UART0_print_string("UART OK - 9600 baud, 8N1\r\n");
//         _delay_ms(1000);
//     }
// #endif /* TEST_UART_ONLY */


// /* ========================================================================
//  * STEP 1 : RAW GPS BYPASS 테스트
//  * ========================================================================
//  * GPS 모듈에서 수신된 문자를 파싱 없이 그대로 PC로 전송한다.
//  * 시리얼 모니터에 "$GPGGA,..." 형태의 NMEA 문장이 보여야 한다.
//  *
//  * [기존 코드의 버그 수정]
//  *   기존 (버그 있음):
//  *     char c = UDR0;              ← 수신 확인 전에 먼저 읽음! 쓰레기값 발생
//  *     if (UCSR0A & (1<<RXC0)) {
//  *         UART0_transmit(c);
//  *     }
//  *
//  *   수정 (올바른 순서):
//  *     if (UCSR0A & (1<<RXC0)) {  ← 먼저 수신 완료 플래그 확인
//  *         char c = UDR0;         ← 그 다음에 버퍼 읽기
//  *         UART0_transmit(c);
//  *     }
//  *   또는 더 간단하게 UART0_receive() 블로킹 함수 사용.
//  * ====================================================================== */
// #ifdef TEST_RAW_BYPASS
//     UART0_print_string("=== RAW GPS BYPASS MODE ===\r\n");
//     UART0_print_string("Waiting for GPS data on PD0...\r\n");

//     while (1) {
//         /*
//          * ※ 올바른 순서 : 수신 완료 확인 → UDR0 읽기 → 송신
//          *   (기존 코드는 이 순서가 뒤집혀 있었음)
//          */
//         if (UCSR0A & (1 << RXC0)) {     /* 수신 버퍼에 데이터가 있는가? */
//             char c = UDR0;              /* 있으면 읽기                  */
//             UART0_transmit(c);          /* PC로 그대로 전송             */
//         }
//         /*
//          * 블로킹 버전으로 대체해도 동일하게 동작:
//          *   char c = UART0_receive();
//          *   UART0_transmit(c);
//          */
//     }
// #endif /* TEST_RAW_BYPASS */


// /* ========================================================================
//  * 정상 동작 모드 : GPS GPGGA 파싱 및 출력
//  * ======================================================================== */

//     GPS_Data myGPS;

//     UART0_print_string("=== GY-NEO6MV2 GPS @ ATmega328P ===\r\n");
//     UART0_print_string("UART : 9600 baud, 8N1\r\n");
//     UART0_print_string("Parsing : GPGGA sentence\r\n");
//     UART0_print_string("Waiting for satellite fix...\r\n");
//     UART0_print_string("------------------------------------\r\n");

//     while (1)
//     {
//         /*
//          * GPS_read_GPGGA() 는 GPGGA 문장이 수신될 때까지 내부에서 블로킹된다.
//          * GPGGA 문장을 찾으면 파싱 결과를 myGPS 에 저장하고 반환한다.
//          */
//         GPS_read_GPGGA(&myGPS);

//         if (myGPS.is_valid)
//         {
//             /* ── GPS Fix 획득 : 위치 정보 출력 ── */

//             UART0_print_string("[FIX] Time (UTC)  : ");
//             /*
//              * time 필드 형식 : "HHMMSS.ss"
//              * 예) "123519.00" → 12:35:19 UTC
//              * 보기 좋게 콜론(:)을 삽입하여 출력한다.
//              */
//             char hh[3] = {myGPS.time[0], myGPS.time[1], '\0'};
//             char mm[3] = {myGPS.time[2], myGPS.time[3], '\0'};
//             char ss[3] = {myGPS.time[4], myGPS.time[5], '\0'};
//             UART0_print_string(hh);
//             UART0_transmit(':');
//             UART0_print_string(mm);
//             UART0_transmit(':');
//             UART0_print_string(ss);
//             UART0_print_string(" UTC\r\n");

//             UART0_print_string("[FIX] Latitude    : ");
//             UART0_print_string(myGPS.latitude);
//             UART0_transmit(' ');
//             UART0_transmit(myGPS.lat_dir);  /* 'N' or 'S' */
//             UART0_print_string("\r\n");

//             UART0_print_string("[FIX] Longitude   : ");
//             UART0_print_string(myGPS.longitude);
//             UART0_transmit(' ');
//             UART0_transmit(myGPS.lon_dir);  /* 'E' or 'W' */
//             UART0_print_string("\r\n");

//             UART0_print_string("------------------------------------\r\n");
//         }
//         else
//         {
//             /* ── Fix 없음 : 위성 탐색 중 ── */
//             /*
//              * GPGGA 문장은 수신되고 있지만 Fix Quality = 0.
//              * → GPS 모듈이 위성 신호를 찾고 있는 중.
//              * → 창문 가까이 또는 실외에서 1~5분 대기.
//              */
//             UART0_print_string("[NO FIX] Searching... Move antenna near a window.\r\n");
//         }

//     } /* while(1) 끝 */

//     return 0;
// }










#define F_CPU 16000000UL        

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "UART0.h"
#include "neo6m_gps.h"

// #define TEST_UART_ONLY      /* STEP 0 : UART TX 만 테스트 */
// #define TEST_RAW_BYPASS     /* STEP 1 : GPS 원본 NMEA 스트림을 그대로 출력 */


// 좌표 환산 : NMEA 포맷(DDMM.MMMMM)을 구글지도 포맷(DD.DDDDDD) 실수형으로 변환합니다.
float convert_nmea_to_dd(char* nmea_str, uint8_t is_longitude) {
    char deg_str[4] = {0};
    char min_str[15] = {0};
    
    // 위도는 앞 2자리, 경도는 앞 3자리가 '도(Degrees)' 입니다.
    int deg_len = is_longitude ? 3 : 2;

    // 도(Degrees) 부분 분리
    strncpy(deg_str, nmea_str, deg_len);
    // 분(Minutes) 부분 분리
    strcpy(min_str, nmea_str + deg_len);

    float degrees = atof(deg_str);
    float minutes = atof(min_str);

    // 환산 공식: 도 + (분 / 60)
    return degrees + (minutes / 60.0);
}



int main(void)
{
    UART0_init();   
    GPS_init();     

#ifdef TEST_UART_ONLY
    while (1) {
        UART0_print_string("UART OK - 9600 baud, 8N1 (Output on PD1)\r\n");
        _delay_ms(1000);
    }
#endif 

#ifdef TEST_RAW_BYPASS
    UART0_print_string("=== RAW GPS BYPASS MODE ===\r\n");
    UART0_print_string("Waiting for GPS data on PB3 (Pin 11)...\r\n");

    while (1) {
        /* PB3에서 소프트웨어로 수신하여, PD1으로 하드웨어 송신 */
        char c = UART0_receive();              
        UART0_transmit(c);          
    }
#endif 

    GPS_Data myGPS;

    UART0_print_string("=== GY-NEO6MV2 GPS @ ATmega328P ===\r\n");
    UART0_print_string("UART : Hybrid (RX=PB3, TX=PD1)\r\n");
    UART0_print_string("Waiting for satellite fix...\r\n");
    UART0_print_string("------------------------------------\r\n");

    while (1)
    {
        GPS_read_GPGGA(&myGPS);

        if (myGPS.is_valid)
        {
            UART0_print_string("[FIX] Time (UTC)  : ");
            char hh[3] = {myGPS.time[0], myGPS.time[1], '\0'};
            char mm[3] = {myGPS.time[2], myGPS.time[3], '\0'};
            char ss[3] = {myGPS.time[4], myGPS.time[5], '\0'};
            UART0_print_string(atoi(hh) + atoi(myGPS.longitude));
            UART0_transmit(':');
            UART0_print_string(mm);
            UART0_transmit(':');
            UART0_print_string(ss);
            UART0_print_string(" UTC+9\r\n");


            // --- 환산 및 출력 부분 수정 ---
            float lat_dd = convert_nmea_to_dd(myGPS.latitude, 0); // 위도 변환
            float lon_dd = convert_nmea_to_dd(myGPS.longitude, 1); // 경도 변환

            char lat_buf[20];
            char long_buf[20];
            // UART0_print_string("[FIX] Google Maps : ");
            
            // 위도 출력 (소수점 6자리까지)
            
            UART0_print_string("[FIX] Latitude    : ");
            dtostrf(lat_dd, 9, 6, lat_buf);
            UART0_transmit(' ');
            UART0_transmit(myGPS.lat_dir);  
            UART0_print_string("\r\n");

            UART0_print_string("[FIX] Longitude   : ");
            // 경도 출력 (소수점 6자리까지)
            dtostrf(lon_dd, 10, 6, long_buf);
            UART0_print_string(long_buf);
            UART0_transmit(' ');
            UART0_transmit(myGPS.lon_dir);  
            UART0_print_string("\r\n");
            
            // https://easymap.info/ko/coordinates?lat=37.54240&lon=126.84124&z=18&mlat=37.5424&mlon=126.84124&c=

            // https://www.google.com/maps/place/37.542178,126.841321
            UART0_print_string("https://www.google.com/maps/place"); UART0_print_string(lat_buf); 
            UART0_print_string(","); UART0_print_string(long_buf);

            // UART0_print_string("https://easymap.info/ko/coordinates?lat="); UART0_print_string(lat_buf); 
            // UART0_print_string("&lon="); UART0_print_string(long_buf);
            // UART0_print_string("&z=18&mlat="); UART0_print_string(lat_buf); UART0_print_string("&mlon="); UART0_print_string(long_buf);
            // UART0_print_string("&c=");
            // UART0_print_string("\r\n");
            // UART0_print_string("------------------------------------\r\n");
        }
        else
        {
            UART0_print_string("[NO FIX] Searching... Move antenna near a window.\r\n");
        }
    } 
    return 0;
}