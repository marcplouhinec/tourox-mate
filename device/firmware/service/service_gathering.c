#include "service_gathering.h"

#include <stddef.h>
#include <string.h>
#include "../util/logging.h"

// Private variables
static bool m_gathering_scheduled = false;
static uint8_t m_hour;
static uint8_t m_minute;
static char m_description[SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH] = {'\0'};
static service_gathering_schedule_event_handler_t m_schedule_event_handler = NULL;


void service_gathering_schedule(uint8_t hour, uint8_t minute, char* description) {
	m_hour = hour;
	m_minute = minute;
	strncpy(m_description, description, SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH);
	m_description[SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH - 1] = '\0';
	m_gathering_scheduled = true;
	LOG("Gathering scheduled at ");
	LOGF("%u:", hour);
	LOGF("%u :\r\n", minute);
	LOGF("%s\r\n", m_description);

	if (m_schedule_event_handler != NULL)
	{
		LOG("Call the schedule_event_handler\r\n");
		m_schedule_event_handler(m_gathering_scheduled);
	}
	else
	{
		LOG("No schedule_event_handler to call\r\n");
	}
}

void service_gathering_unschedule() {
	m_gathering_scheduled = false;

	if (m_schedule_event_handler != NULL)
	{
		m_schedule_event_handler(m_gathering_scheduled);
	}
}

uint8_t service_gathering_get_hour() {
	return m_hour;
}

uint8_t service_gathering_get_minute() {
	return m_minute;
}

char* service_gathering_get_description() {
	return m_description;
}

bool service_gathering_is_scheduled() {
	return m_gathering_scheduled;
}

void service_gathering_register_schedule_event_handler(service_gathering_schedule_event_handler_t schedule_event_handler) {
	m_schedule_event_handler = schedule_event_handler;
}
