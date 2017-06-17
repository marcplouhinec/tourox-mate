#include "service_tour.h"
#include <stddef.h>

// Private variables
static bool m_tour_configured = false;
static service_tour_configuration_change_event_handler_t m_configuration_change_event_handler = NULL;


void service_tour_set_tour_configured(bool tour_configured) {
	m_tour_configured = tour_configured;

	if (m_configuration_change_event_handler != NULL)
		m_configuration_change_event_handler(tour_configured);
}

bool service_tour_is_tour_configured() {
	return m_tour_configured;
}

void service_tour_set_configuration_change_event_handler(service_tour_configuration_change_event_handler_t configuration_change_event_handler) {
	m_configuration_change_event_handler = configuration_change_event_handler;
}
