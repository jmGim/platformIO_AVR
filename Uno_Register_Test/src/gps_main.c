// #include "neo6m_gps.h"

// int main(void) {
//     char c;
//     char buffer[100];
//     int i = 0;
//     char latitude[15], longitude[15];

//     GPS_Init();

//     while (1) {
//         c = GPS_Read();
//         if (c == '\n' || c == '\r') {
//             if (i > 0) {
//                 buffer[i] = '\0';
//                 GPS_Parse(buffer, latitude, longitude);
//                 // Print or use latitude/longitude here
//                 i = 0;
//             }
//         } else {
//             buffer[i++] = c;
//         }
//     }
// }



#define F_CPU 16000000UL  // Assuming a 16MHz clock (Arduino Uno/Nano standard)


#include "neo6m_gps.h"
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"


int main(void) {
    GPS_Data myGPS;

    // 1. Initialize Hardware
    UART0_init(); // Uses your provided 9600 baud, 8N1 setup
    GPS_init();   // Sets the DDR and PORT states for PD0/PD1

    UART0_print_string("--- GPS GY-NEO6MV2 Initialization ---\r\n");
    UART0_print_string("Waiting for satellite fix...\r\n\r\n");

    while (1) {
        // 2. Wait for and parse a GPGGA sentence
        GPS_read_GPGGA(&myGPS);

        // 3. Print the results if the GPS has locked onto satellites
        if (myGPS.is_valid) {
            UART0_print_string("Time (UTC): ");
            UART0_print_string(myGPS.time);
            UART0_print_string("\r\n");

            UART0_print_string("Latitude: ");
            UART0_print_string(myGPS.latitude);
            UART0_transmit(' ');
            UART0_transmit(myGPS.lat_dir);
            UART0_print_string("\r\n");

            UART0_print_string("Longitude: ");
            UART0_print_string(myGPS.longitude);
            UART0_transmit(' ');
            UART0_transmit(myGPS.lon_dir);
            UART0_print_string("\r\n");
            
            UART0_print_string("------------------------\r\n");
        } else {
            // Uncomment the line below for debugging if you want to see it actively failing to find a fix
            UART0_print_string("No GPS Fix yet. Make sure the antenna is near a window.\r\n");
        }
    }
    return 0;
}