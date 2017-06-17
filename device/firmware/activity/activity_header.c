#include "activity_header.h"
#include <stdbool.h>
#include "../main_loop_call.h"
#include "../display/display_header.h"
#include "../service/service_time.h"
#include "../service/service_proximity.h"
#include "../service/service_fona.h"

// private functions declaration
static void activity_header_refresh_in_main_loop();

// Private variables
static bool m_started = false;


void activity_header_start() {
	m_started = true;

	display_header_show_proximity_level(DISCONNECTED);
}

void activity_header_refresh() {
	if (m_started)
	{
		call_in_main_loop(activity_header_refresh_in_main_loop);
	}
}

static void activity_header_refresh_in_main_loop() {
	display_header_show_time(service_time_get_current_formatted_time());

	display_header_show_proximity_level(service_proximity_get_level());

	display_header_show_battery_percentage(service_fona_get_battery_percentage());

	display_header_show_gsm_signal_percentage(service_fona_get_gsm_signal_percentage());
}
