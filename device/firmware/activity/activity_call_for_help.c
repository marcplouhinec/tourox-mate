#include "activity_call_for_help.h"
#include <stddef.h>
#include "../main_loop_call.h"
#include "activity_main.h"
#include "../display/display_menu.h"
#include "../display/display_message_panel.h"
#include "../service/service_fona.h"

// Private functions declaration
static void activity_call_for_help_start();
static void activity_call_for_help_stop();
static void confirm_call_for_help_menu_item_selection_handler();
static void call_guide_in_main_loop();
static void cancel_call_for_help_menu_item_selection_handler();
static void cancel_call_for_help_menu_item_selection_handler_in_main_loop();
static void fona_event_handler(service_fona_event_type event_type);

// Private variables
static activity_context_t m_context = {activity_call_for_help_start, activity_call_for_help_stop};
static display_menu_item_t m_confirm_call_for_help_menu_item = {"OK", confirm_call_for_help_menu_item_selection_handler};
static display_menu_item_t m_cancel_call_for_help_menu_item = {"CANCEL", cancel_call_for_help_menu_item_selection_handler};
static bool m_activity_active = false;

void activity_call_for_help_init() {
	service_fona_register_event_handler(fona_event_handler);
}

activity_context_t* activity_call_for_help_get_context() {
	return &m_context;
}

static void activity_call_for_help_start() {
	m_activity_active = true;

	display_message_panel_show_text("Call for help?");
	display_menu_set_menu_items(&m_confirm_call_for_help_menu_item, &m_cancel_call_for_help_menu_item);
}

static void activity_call_for_help_stop() {
	m_activity_active = false;
}

static void confirm_call_for_help_menu_item_selection_handler() {
	display_message_panel_show_text("Calling for help...");
	display_menu_set_menu_items(NULL, &m_cancel_call_for_help_menu_item);
	call_in_main_loop(call_guide_in_main_loop);
}
static void call_guide_in_main_loop() {
	service_fona_call_guide();
}

static void cancel_call_for_help_menu_item_selection_handler() {
	call_in_main_loop(cancel_call_for_help_menu_item_selection_handler_in_main_loop);
}
static void cancel_call_for_help_menu_item_selection_handler_in_main_loop() {
	service_fona_hang_up();
	activity_manager_start_activity(activity_main_get_context());
}

static void fona_event_handler(service_fona_event_type event_type) {
	if ((event_type == SERVICE_FONA_NO_CARRIER || event_type == SERVICE_FONA_BUSY) && m_activity_active)
	{
		cancel_call_for_help_menu_item_selection_handler();
	}
}
