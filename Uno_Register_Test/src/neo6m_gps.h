// #ifndef NEO6M_GPS_H_
// #define NEO6M_GPS_H_

// #define F_CPU 16000000UL // Set your clock speed
// #include <avr/io.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <avr/interrupt.h>

// // UART Configuration
// #define BAUD 9600
// #define MYUBRR F_CPU/16/BAUD-1

// // GPS Functions
// void GPS_Init(void);
// char GPS_Read(void);
// void GPS_Parse(char* nmeaSentence, char* lat, char* lon);

// #endif /* NEO6M_GPS_H_ */



#ifndef NEO6M_GPS_H
#define NEO6M_GPS_H

#include <avr/io.h>
#include <string.h>

// Structure to hold parsed GPS data
typedef struct {
    char time[15];
    char latitude[15];
    char lat_dir;       // N or S
    char longitude[15];
    char lon_dir;       // E or W
    uint8_t is_valid;   // 1 if GPS has a fix, 0 if not
} GPS_Data;

// Function prototypes
void GPS_init(void);
void GPS_read_GPGGA(GPS_Data *gps_info);

#endif /* NEO6M_GPS_H */