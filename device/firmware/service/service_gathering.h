#ifndef _SERVICE_GATHERING_H_
#define _SERVICE_GATHERING_H_

#include <stdbool.h>
#include <stdint.h>

#define SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH 250

/**
 * @brief Function called when a gathering is scheduled or unscheduled.
 */
typedef void (*service_gathering_schedule_event_handler_t) (bool gathering_scheduled);

/**
 * @brief Schedule a gathering at the given time.
 */
void service_gathering_schedule(uint8_t hour, uint8_t minute, char* description);

/**
 * @brief Unschedule the last gathering.
 */
void service_gathering_unschedule();

/**
 * @return Scheduled gathering hour.
 */
uint8_t service_gathering_get_hour();

/**
 * @return Scheduled gathering minute.
 */
uint8_t service_gathering_get_minute();

/**
 * @return Scheduled gathering description.
 */
char* service_gathering_get_description();

/**
 * @return true if a gathering has been scheduled, false if not.
 */
bool service_gathering_is_scheduled();

/**
 * @brief Register a function that will be called when a gathering is scheduled or unscheduled.
 */
void service_gathering_register_schedule_event_handler(service_gathering_schedule_event_handler_t schedule_event_handler);

#endif
