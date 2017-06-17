#include "activity_guide_call.h"
#include <stdbool.h>
#include <stddef.h>
#include "../main_loop_call.h"
#include "activity_main.h"
#include "../display/display_menu.h"
#include "../display/display_message_panel.h"
#include "../service/service_fona.h"

// Private functions declaration
static void activity_guide_call_start();
static void activity_guide_call_stop();
static void accept_guide_call_menu_item_selection_handler();
static void accept_guide_call_menu_item_selection_handler_in_main_loop();
static void refuse_guide_call_menu_item_selection_handler();
static void refuse_guide_call_menu_item_selection_handler_in_main_loop();
static void hang_up_menu_item_selection_handler();
static void hang_up_menu_item_selection_handler_in_main_loop();
static void fona_event_handler(service_fona_event_type event_type);
static void start_this_activity();
static void stop_this_activity();

// Private variables
static activity_context_t m_context = {activity_guide_call_start, activity_guide_call_stop};
static display_menu_item_t m_accept_guide_call_menu_item = {"YES", accept_guide_call_menu_item_selection_handler};
static display_menu_item_t m_refuse_guide_call_menu_item = {"NO", refuse_guide_call_menu_item_selection_handler};
static display_menu_item_t m_hang_up_guide_call_menu_item = {"HANG UP", hang_up_menu_item_selection_handler};
static bool m_activity_active = false;


void activity_guide_call_init() {
	service_fona_register_event_handler(fona_event_handler);
}

activity_context_t* activity_guide_call_get_context() {
	return &m_context;
}

static void activity_guide_call_start() {
	m_activity_active = true;

	display_message_panel_show_text("Answer incoming call?");
	display_menu_set_menu_items(&m_accept_guide_call_menu_item, &m_refuse_guide_call_menu_item);
}

static void activity_guide_call_stop() {
	m_activity_active = false;
}

static void accept_guide_call_menu_item_selection_handler() {
	call_in_main_loop(accept_guide_call_menu_item_selection_handler_in_main_loop);
}
static void accept_guide_call_menu_item_selection_handler_in_main_loop() {
	service_fona_pick_up();

	display_message_panel_show_text("Ongoing call...");
	display_menu_set_menu_items(NULL, &m_hang_up_guide_call_menu_item);
}

static void refuse_guide_call_menu_item_selection_handler() {
	call_in_main_loop(refuse_guide_call_menu_item_selection_handler_in_main_loop);
}
static void refuse_guide_call_menu_item_selection_handler_in_main_loop() {
	service_fona_hang_up();
	stop_this_activity();
}

static void hang_up_menu_item_selection_handler() {
	call_in_main_loop(hang_up_menu_item_selection_handler_in_main_loop);
}
static void hang_up_menu_item_selection_handler_in_main_loop() {
	service_fona_hang_up();
	stop_this_activity();
}

static void fona_event_handler(service_fona_event_type event_type) {
	if (event_type == SERVICE_FONA_RING)
	{
		call_in_main_loop(start_this_activity);
	}
	else if (event_type == SERVICE_FONA_NO_CARRIER && m_activity_active)
	{
		call_in_main_loop(stop_this_activity);
	}
}

static void start_this_activity() {
	activity_manager_start_activity(activity_guide_call_get_context());
}

static void stop_this_activity() {
	activity_manager_start_activity(activity_main_get_context());
}
