#ifndef _DISPLAY_HEADER_H_
#define _DISPLAY_HEADER_H_

#include <stdint.h>
#include "../enums/proximity_level.h"

void display_header_show_time(char* time);

void display_header_show_proximity_level(proximity_level proximity_level);

void display_header_show_gsm_signal_percentage(uint8_t gsm_signal_percentage);

void display_header_show_battery_percentage(uint8_t battery_percentage);

#endif
