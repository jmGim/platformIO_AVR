

#include "neo6m_gps.h"
#include "UART0.h"

void GPS_init(void) {    
    
    // Set PB3 (RXD) as INPUT. We will receive data from the GPS TX pin here.
    DDRB &= ~(1 << DDB3); // PB3 입력 (RX)
    // Enable the internal pull-up resistor on PB3 to keep the line stable
    PORTB |= (1 << PORTB3);
    // Set PB4 (TXD) as OUTPUT. We will transmit data to the PC from here.
    DDRB |= (1 << DDB4);  // PB4 출력 (TX) [추가됨]
    
    // Note: The UART peripheral will override normal port operations once enabled,
    // but explicitly setting DDR is excellent practice and prevents floating pins.
}

void GPS_read_GPGGA(GPS_Data *gps_info) {
    char buffer[100];
    char c;
    uint8_t index = 0;
    
    // Reset validity to 0 before reading
    gps_info->is_valid = 0;

    // 1. Wait for the '$' character, which marks the start of an NMEA sentence
    while (UART0_receive() != '$');

    // 2. Read the rest of the sentence until the carriage return/newline
    while (1) {
        c = UART0_receive();
        if (c == '\r' || c == '\n') {
            buffer[index] = '\0'; // Null-terminate the string
            break;
        }
        if (index < 99) {
            buffer[index++] = c;
        }
    }

    // 3. Check if the received sentence is a GPGGA sentence
    if (strncmp(buffer, "GPGGA", 5) == 0) {
        uint8_t comma_count = 0;
        uint8_t j = 0;
        
        // Clear old string data
        memset(gps_info->time, 0, sizeof(gps_info->time));
        memset(gps_info->latitude, 0, sizeof(gps_info->latitude));
        memset(gps_info->longitude, 0, sizeof(gps_info->longitude));

        // 4. Manually parse the comma-separated values
        for (uint8_t i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] == ',') {
                comma_count++;
                j = 0; // Reset index for the next data field
                continue;
            }

            // Route characters into the struct based on comma position
            if (comma_count == 1) {
                gps_info->time[j++] = buffer[i];
            } 
            else if (comma_count == 2) {
                gps_info->latitude[j++] = buffer[i];
            } 
            else if (comma_count == 3) {
                gps_info->lat_dir = buffer[i];
            } 
            else if (comma_count == 4) {
                gps_info->longitude[j++] = buffer[i];
            } 
            else if (comma_count == 5) {
                gps_info->lon_dir = buffer[i];
            } 
            else if (comma_count == 6) {
                // Fix Quality: '1' or '2' means a valid fix. '0' means invalid.
                if (buffer[i] == '1' || buffer[i] == '2') {
                    gps_info->is_valid = 1;
                }
            }
        }
    }
}