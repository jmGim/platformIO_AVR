




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