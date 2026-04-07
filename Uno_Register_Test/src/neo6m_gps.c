// /*
//  * neo6m_gps.c
//  * GY-NEO6MV2 GPS 모듈 드라이버 구현 (ATmega328P)
//  *
//  * 파싱 대상 NMEA 문장 : $GPGGA (Global Positioning System Fix Data)
//  */

// #include "neo6m_gps.h"
// #include "UART0.h"

// /* =======================================================================
//  * GPS_init
//  * =======================================================================
//  * 역할 : PD0(RXD), PD1(TXD) 핀의 DDR 및 PORT 초기 상태를 설정한다.
//  *
//  * [배경 지식]
//  *   ATmega328P에서 UART0가 활성화되면 PD0/PD1은 UART 주변장치가 직접
//  *   제어한다. 하지만 명시적으로 DDR을 설정해두면 UART 비활성화 상태에서
//  *   핀이 부유(floating)되는 것을 방지하고, 코드 가독성도 높아진다.
//  *
//  * [DDR 설정]
//  *   DDD0 = 0 : PD0을 입력(INPUT)으로 설정 → GPS TX 신호 수신
//  *   DDD1 = 1 : PD1을 출력(OUTPUT)으로 설정 → PC로 데이터 송신
//  *
//  * [PORT 설정]
//  *   PORTD0 = 1 : PD0 내부 풀업 저항 활성화 → 신호선 안정화
//  *                (UART 수신 핀에 풀업은 UART 동작에 영향을 주지 않음)
//  * ===================================================================== */
// void GPS_init(void)
// {
//     /* PD0(RXD) : 입력 방향 설정 */
//     DDRD &= ~(1 << DDD0);

//     /* PD0 내부 풀업 활성화 (선 안정화, UART 동작에 무해) */
//     PORTD |= (1 << PORTD0);

//     /* PD1(TXD) : 출력 방향 설정 */
//     DDRD |= (1 << DDD1);
// }

// /* =======================================================================
//  * GPS_read_GPGGA
//  * =======================================================================
//  * 역할 : UART0 수신 스트림에서 $GPGGA 문장을 찾아 파싱하고,
//  *        결과를 gps_info 구조체에 저장한다.
//  *
//  * [GPGGA 문장 예시]
//  *   $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
//  *   필드번호:  1      2       3    4      5  6  7  ...
//  *   (쉼표 기준)
//  *     comma_count=1 → 시각(time)
//  *     comma_count=2 → 위도(latitude)
//  *     comma_count=3 → 위도방향(lat_dir)
//  *     comma_count=4 → 경도(longitude)
//  *     comma_count=5 → 경도방향(lon_dir)
//  *     comma_count=6 → Fix 품질 (0=없음, 1=GPS Fix, 2=DGPS Fix)
//  *
//  * [수정 사항]
//  *   기존 구조 : GPGGA가 아니면 함수를 그냥 리턴 → main의 while(1)이
//  *               다음 호출에서 다시 '$'부터 탐색하므로 동작은 하지만
//  *               "내부에서 GPGGA를 찾을 때까지 반복"하도록 개선하여
//  *               호출 측 코드를 단순화했다.
//  * ===================================================================== */
// void GPS_read_GPGGA(GPS_Data *gps_info)
// {
//     char    buffer[100];
//     char    c;
//     uint8_t index;

//     /*
//      * GPGGA 문장을 찾을 때까지 내부에서 반복한다.
//      * GPS 모듈은 보통 1초마다 여러 종류의 NMEA 문장을 연속으로 전송한다.
//      * (GPGGA, GPRMC, GPGSV, GPGSA 등)
//      * '$'를 기준으로 한 문장씩 읽고, GPGGA인지 확인한다.
//      */
//     while (1)
//     {
//         /* ── Step 1 : '$' 문자가 올 때까지 대기 ── */
//         /*
//          * GPS 데이터가 들어오지 않으면 여기서 무한 블로킹된다.
//          * 하드웨어 연결(GPS TX → PD0)을 반드시 확인할 것.
//          * 디버깅 시 : gps_main.c 의 RAW BYPASS 모드를 먼저 실행해볼 것.
//          */
//         while (UART0_receive() != '$');

//         /* ── Step 2 : '\r' 또는 '\n' 전까지 나머지 문자 수신 ── */
//         index = 0;
//         while (1) {
//             c = UART0_receive();

//             /* 문장 끝(CR 또는 LF)에 도달하면 버퍼를 닫고 루프 탈출 */
//             if (c == '\r' || c == '\n') {
//                 buffer[index] = '\0';
//                 break;
//             }

//             /* 버퍼 오버플로우 방지 : 최대 99문자까지만 저장 */
//             if (index < 99) {
//                 buffer[index++] = c;
//             }
//             /* 99자 초과 시 해당 문장은 무시하고 다음 '$'를 기다림 */
//         }

//         /* ── Step 3 : 수신된 문장이 GPGGA인지 확인 ── */
//         /*
//          * '$' 를 이미 소비했으므로 buffer[0]부터는 'G','P','G','G','A'가 온다.
//          * strncmp(buffer, "GPGGA", 5) == 0 이면 GPGGA 문장이다.
//          */
//         if (strncmp(buffer, "GPGGA", 5) != 0) {
//             /* GPGGA가 아니면 다음 문장을 읽으러 루프 처음으로 돌아감 */
//             continue;
//         }

//         /* ── Step 4 : GPGGA 문장 파싱 ── */

//         /* 이전 데이터 초기화 */
//         gps_info->is_valid = 0;
//         memset(gps_info->time,      0, sizeof(gps_info->time));
//         memset(gps_info->latitude,  0, sizeof(gps_info->latitude));
//         memset(gps_info->longitude, 0, sizeof(gps_info->longitude));
//         gps_info->lat_dir = '\0';
//         gps_info->lon_dir = '\0';

//         /*
//          * buffer 안의 내용 (예):
//          *   "GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,..."
//          *
//          * 쉼표(,)를 만날 때마다 comma_count를 증가시키고,
//          * 해당 필드의 문자를 구조체에 저장한다.
//          * 필드 인덱스 j는 쉼표를 만날 때마다 0으로 초기화된다.
//          */
//         uint8_t comma_count = 0;
//         uint8_t j           = 0;

//         for (uint8_t i = 0; buffer[i] != '\0'; i++)
//         {
//             if (buffer[i] == ',') {
//                 /* 쉼표 → 다음 필드로 이동 */
//                 comma_count++;
//                 j = 0;
//                 continue;
//             }

//             /*
//              * comma_count 값에 따라 적절한 필드에 문자를 저장한다.
//              * 배열 경계(버퍼 오버플로우)를 반드시 확인한다.
//              */
//             switch (comma_count)
//             {
//                 case 1: /* 시각 (HHMMSS.ss) */
//                     if (j < sizeof(gps_info->time) - 1)
//                         gps_info->time[j++] = buffer[i];
//                     break;

//                 case 2: /* 위도 (DDMM.mmmm) */
//                     if (j < sizeof(gps_info->latitude) - 1)
//                         gps_info->latitude[j++] = buffer[i];
//                     break;

//                 case 3: /* 위도 방향 (N/S) */
//                     gps_info->lat_dir = buffer[i];
//                     break;

//                 case 4: /* 경도 (DDDMM.mmmm) */
//                     if (j < sizeof(gps_info->longitude) - 1)
//                         gps_info->longitude[j++] = buffer[i];
//                     break;

//                 case 5: /* 경도 방향 (E/W) */
//                     gps_info->lon_dir = buffer[i];
//                     break;

//                 case 6: /* Fix 품질
//                          *   '0' = Fix 없음 (is_valid = 0)
//                          *   '1' = GPS Fix  (is_valid = 1)
//                          *   '2' = DGPS Fix (is_valid = 1)
//                          */
//                     if (buffer[i] == '1' || buffer[i] == '2')
//                         gps_info->is_valid = 1;
//                     break;

//                 default:
//                     /* 필드 7 이후(위성 수, HDOP, 고도 등)는 무시 */
//                     break;
//             }
//         }

//         /* GPGGA 파싱 완료 → 함수 반환 */
//         return;

//     } /* while (1) 끝 */
// }



#include "neo6m_gps.h"
#include "UART0.h"

void GPS_init(void)
{
    /* PB3(RX) : 입력 방향 및 풀업 설정 (UART0.c에도 있지만 명시적으로 한 번 더 세팅) */
    DDRB &= ~(1 << DDB3);
    PORTB |= (1 << PORTB3);
}

void GPS_read_GPGGA(GPS_Data *gps_info)
{
    char    buffer[100];
    char    c;
    uint8_t index;

    while (1)
    {
        while (UART0_receive() != '$');

        index = 0;
        while (1) {
            c = UART0_receive();
            if (c == '\r' || c == '\n') {
                buffer[index] = '\0';
                break;
            }
            if (index < 99) {
                buffer[index++] = c;
            }
        }

        if (strncmp(buffer, "GPGGA", 5) != 0) {
            continue;
        }

        gps_info->is_valid = 0;
        memset(gps_info->time,      0, sizeof(gps_info->time));
        memset(gps_info->latitude,  0, sizeof(gps_info->latitude));
        memset(gps_info->longitude, 0, sizeof(gps_info->longitude));
        gps_info->lat_dir = '\0';
        gps_info->lon_dir = '\0';

        uint8_t comma_count = 0;
        uint8_t j           = 0;

        for (uint8_t i = 0; buffer[i] != '\0'; i++)
        {
            if (buffer[i] == ',') {
                comma_count++;
                j = 0;
                continue;
            }

            switch (comma_count)
            {
                case 1: 
                    if (j < sizeof(gps_info->time) - 1)
                        gps_info->time[j++] = buffer[i];
                    break;
                case 2: 
                    if (j < sizeof(gps_info->latitude) - 1)
                        gps_info->latitude[j++] = buffer[i];
                    break;
                case 3: 
                    gps_info->lat_dir = buffer[i];
                    break;
                case 4: 
                    if (j < sizeof(gps_info->longitude) - 1)
                        gps_info->longitude[j++] = buffer[i];
                    break;
                case 5: 
                    gps_info->lon_dir = buffer[i];
                    break;
                case 6: 
                    if (buffer[i] == '1' || buffer[i] == '2')
                        gps_info->is_valid = 1;
                    break;
                default:
                    break;
            }
        }
        return;
    }
}