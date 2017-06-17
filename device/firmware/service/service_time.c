#include "service_time.h"
#include <stdbool.h>
#include <stdio.h>
#include "nrf51.h"

#define LFCLK_FREQUENCY  0x8000
#define RTC_COUNTER_MASK 0xFFFFFF

// Private variables
static bool m_initialized = false;
static uint32_t m_ticks_per_second = 0;
static uint32_t m_previous_rtc_value = 0;
static uint32_t m_time_since_startup = 0; // Number of elapsed seconds since startup.
static uint32_t m_startup_time_since_midnight = 0; // Number of elapsed seconds between 12am (midnight) and startup time.
static char m_last_formatted_time[9];

void service_time_init() {
	m_previous_rtc_value = 0;
	m_time_since_startup = 0;
	m_startup_time_since_midnight = 0;
	m_ticks_per_second = LFCLK_FREQUENCY / (NRF_RTC1->PRESCALER + 1);
	m_last_formatted_time[0] = '\0';

	m_initialized = true;
}

void service_time_update() {
	if (m_initialized)
	{
		uint32_t current_rtc_value = NRF_RTC1->COUNTER;
		uint32_t delta_ticks = (current_rtc_value - m_previous_rtc_value) & RTC_COUNTER_MASK;
		uint32_t delta_seconds = delta_ticks / m_ticks_per_second;
		m_time_since_startup += delta_seconds;
		uint32_t missingTicks = delta_ticks - delta_seconds * m_ticks_per_second;
		m_previous_rtc_value = current_rtc_value - missingTicks;
	}
}

void service_time_set_current_time(uint8_t hour, uint8_t minute, uint8_t second) {
	m_startup_time_since_midnight = hour * 60 * 60 + minute * 60 + second - m_time_since_startup;
}

char* service_time_get_current_formatted_time() {
	uint32_t current_time = (m_time_since_startup + m_startup_time_since_midnight) % (24 * 60 * 60);
	uint8_t current_hour = current_time / (60 * 60);
	uint8_t current_minute = current_time / 60 - current_hour * 60;
	uint8_t current_second = current_time - current_hour * 60 * 60 - current_minute * 60;
	sprintf(m_last_formatted_time, "%02d:%02d:%02d", current_hour, current_minute, current_second);
	return m_last_formatted_time;
}

uint32_t service_time_get_time_since_startup() {
	return m_time_since_startup;
}
