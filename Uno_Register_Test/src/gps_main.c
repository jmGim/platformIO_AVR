


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