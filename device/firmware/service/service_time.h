#ifndef _SERVICE_TIME_H_
#define _SERVICE_TIME_H_

#include <stdint.h>

/**
 * @brief Initialize the service.
 */
void service_time_init();

/**
 * @brief Function that must be called by a timer at least one time per second.
 *
 * Note that the RTC1 clock is used to maintain a good precision.
 */
void service_time_update();

/**
 * @brief Set the current time.
 */
void service_time_set_current_time(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * @brief Get the current time formatted like this: 'hh:mm:ss'.
 *
 * Note: the returned value is a static array, it means that two calls to this function would return the same pointer.
 */
char* service_time_get_current_formatted_time();

/**
 * @return Number of elapsed seconds since startup.
 */
uint32_t service_time_get_time_since_startup();

#endif
