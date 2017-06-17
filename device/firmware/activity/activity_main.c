#include "activity_main.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "activity_call_for_help.h"
#include "../display/display_menu.h"
#include "../display/display_message_panel.h"
#include "../service/service_gathering.h"

// Private functions declaration
static void activity_main_start();
static void activity_main_stop();
static void call_for_help_menu_item_selection_handler();
static void display_home_message();
static void display_gathering_information();
static void handle_gathering_schedule_event(bool gathering_scheduled);

// Private variables
static activity_context_t m_context = {activity_main_start, activity_main_stop};
static display_menu_item_t m_call_for_help_menu_item = {"HELP", call_for_help_menu_item_selection_handler};
static bool m_activity_active = false;
static char m_gathering_description[SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH + 32];


void activity_main_init() {
	service_gathering_register_schedule_event_handler(handle_gathering_schedule_event);
}

activity_context_t* activity_main_get_context() {
	return &m_context;
}

static void activity_main_start() {
	m_activity_active = true;

	display_menu_set_menu_items(&m_call_for_help_menu_item, NULL);

	if (service_gathering_is_scheduled())
	{
		display_gathering_information();
	}
	else
	{
		display_home_message();
	}
}

static void activity_main_stop() {
	m_activity_active = false;
}

static void call_for_help_menu_item_selection_handler() {
	activity_manager_start_activity(activity_call_for_help_get_context());
}

static void display_home_message() {
	display_message_panel_show_text("Enjoy your trip!");
}

static void display_gathering_information() {
	sprintf(m_gathering_description, "Gathering scheduled at %02d:%02d. %s",
			service_gathering_get_hour(),
			service_gathering_get_minute(),
			service_gathering_get_description());
	display_message_panel_show_text(m_gathering_description);
}

static void handle_gathering_schedule_event(bool gathering_scheduled) {
	if (m_activity_active)
	{
		if (gathering_scheduled)
		{
			display_gathering_information();
		}
		else
		{
			display_home_message();
		}
	}
}
