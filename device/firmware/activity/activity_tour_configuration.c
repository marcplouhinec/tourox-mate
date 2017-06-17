#include "activity_tour_configuration.h"
#include <stddef.h>
#include "../display/display_message_panel.h"
#include "../display/display_menu.h"
#include "../service/service_tour.h"
#include "activity_main.h"

// Private functions declaration
static void activity_tour_configuration_start();
static void activity_tour_configuration_stop();
static void tour_configuration_change_event_handler(bool tour_configured);

// Private variables
static activity_context_t m_context = {activity_tour_configuration_start, activity_tour_configuration_stop};


void activity_tour_configuration_init() {
	service_tour_set_configuration_change_event_handler(tour_configuration_change_event_handler);
}

activity_context_t* activity_tour_configuration_get_context() {
	return &m_context;
}

static void activity_tour_configuration_start() {
	display_message_panel_show_text("Please configure a tour");
	display_menu_set_menu_items(NULL, NULL);
}

static void activity_tour_configuration_stop() {
	// Nothing to do
}

static void tour_configuration_change_event_handler(bool tour_configured) {
	if (tour_configured)
	{
		activity_manager_start_activity(activity_main_get_context());
	}
	else
	{
		activity_manager_start_activity(activity_tour_configuration_get_context());
	}
}
