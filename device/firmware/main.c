/**
 * @brief Group Guided Tour Assistant main file.
 *
 * @author Marc Plouhinec
 */

#include <stdbool.h>
#include <stdint.h>
#include "main_loop_call.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "ble_facade/ble_facade.h"
#include "Adafruit_SHARP_Memory_Display/Adafruit_SharpMem.h"
#include "Adafruit_FONA_Library/Adafruit_FONA.h"
#include "display/display_header.h"
#include "display/display_menu.h"
#include "display/display_message_panel.h"
#include "service/service_time.h"
#include "service/service_fona.h"
#include "service/service_geolocation.h"
#include "activity/activity_manager.h"
#include "activity/activity_header.h"
#include "activity/activity_tour_configuration.h"
#include "activity/activity_main.h"
#include "activity/activity_call_for_help.h"
#include "activity/activity_guide_call.h"
#include "util/logging.h"

//
// Hard-coded constants
//

#define APP_TIMER_PRESCALER      0    /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS     10   /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE  8    /**< Size of timer operation queues. */

#define STATUS_LED_INTERVAL                   APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)  /**< STATUS_LED blinking interval (ticks). */
#define DISPLAY_MESSAGE_PANEL_INTERVAL        APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER)  /**< Display message panel page change interval (ticks). */
#define SERVICE_TIME_UPDATE_INTERVAL          APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)  /**< Service time update interval (ticks). */
#define ACTIVITY_HEADER_REFRESH_INTERVAL      APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)  /**< Activity header refresh interval (ticks). */
#define SERVICE_GEOLOCATION_UPLOAD_INTERVAL   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Activity header refresh interval (ticks). */

#define BUTTON_DETECTION_DELAY   APP_TIMER_TICKS(50, APP_TIMER_PRESCALER) /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define MAX_MAIN_LOOP_CALLABLES 20

//
// I/O pins definitions
//

#ifdef TARGET_BOARD_IS_NRF51_DK
	#define STATUS_LED_PIN_NUMBER     21
	#define DISPLAY_ENABLE_PIN_NUMBER 1 // Unused
	#define DISPLAY_CS_PIN_NUMBER     15
	#define DISPLAY_MOSI_PIN_NUMBER   14
	#define DISPLAY_MISO_PIN_NUMBER   2 // Unused
	#define DISPLAY_SCLK_PIN_NUMBER   13
	#define LEFT_BUTTON_PIN_NUMBER    19
	#define RIGHT_BUTTON_PIN_NUMBER   20
	#define FONA_TX_PIN_NUMBER        5
	#define FONA_RX_PIN_NUMBER        6
	#define FONA_RST_PIN_NUMBER       12
#endif

#ifdef TARGET_BOARD_IS_BLE_NANO
	#define STATUS_LED_PIN_NUMBER     19
	#define DISPLAY_ENABLE_PIN_NUMBER 1 // Unused
	#define DISPLAY_CS_PIN_NUMBER     28
	#define DISPLAY_MOSI_PIN_NUMBER   29
	#define DISPLAY_MISO_PIN_NUMBER   2 // Unused
	#define DISPLAY_SCLK_PIN_NUMBER   15
	#define LEFT_BUTTON_PIN_NUMBER    4
	#define RIGHT_BUTTON_PIN_NUMBER   5
	#define FONA_TX_PIN_NUMBER        9
	#define FONA_RX_PIN_NUMBER        11
	#define FONA_RST_PIN_NUMBER       3
#endif


// Global functions declaration
static void status_led_timeout_handler(void* p_context);
static void display_message_panel_timeout_handler(void* p_context);
static void service_time_update_timeout_handler(void* p_context);
static void activity_header_refresh_timeout_handler(void* p_context);
static void service_geolocation_upload_timeout_handler(void* p_context);

static void app_button_handler(uint8_t pin_no, uint8_t button_action);


// Private variables
static app_timer_id_t m_status_led_timer_id;
static app_timer_id_t m_display_message_panel_timer_id;
static app_timer_id_t m_service_time_update_timer_id;
static app_timer_id_t m_activity_header_refresh_timer_id;
static app_timer_id_t m_service_geolocation_upload_timer_id;
static main_loop_callable_t m_main_loop_callables[MAX_MAIN_LOOP_CALLABLES] = {0};
static uint8_t m_nb_main_loop_callables = 0;


/**
 * @brief Application entry point.
 */
int main(void)
{
	LOG("Starting...\r\n");
	uint32_t err_code;

	// Initialize the STATUS_LED pin
	nrf_gpio_cfg_output(STATUS_LED_PIN_NUMBER);

	// Initialize the display
	Adafruit_SharpMem_init(
			DISPLAY_ENABLE_PIN_NUMBER,
			DISPLAY_CS_PIN_NUMBER,
			DISPLAY_MOSI_PIN_NUMBER,
			DISPLAY_MISO_PIN_NUMBER,
			DISPLAY_SCLK_PIN_NUMBER);
	Adafruit_SharpMem_begin();
	Adafruit_GFX_setRotation(0);
	Adafruit_SharpMem_enableDisplay();
	Adafruit_SharpMem_clearDisplay();
	Adafruit_GFX_setTextSize(1);
	Adafruit_GFX_setTextColor(BLACK);

	// Initialize the FONA
	Adafruit_FONA_init(FONA_TX_PIN_NUMBER, FONA_RX_PIN_NUMBER, FONA_RST_PIN_NUMBER);

	// Initialize timers
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
	err_code = app_timer_create(&m_status_led_timer_id, APP_TIMER_MODE_REPEATED, status_led_timeout_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&m_display_message_panel_timer_id, APP_TIMER_MODE_REPEATED, display_message_panel_timeout_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&m_service_time_update_timer_id, APP_TIMER_MODE_REPEATED, service_time_update_timeout_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&m_activity_header_refresh_timer_id, APP_TIMER_MODE_REPEATED, activity_header_refresh_timeout_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&m_service_geolocation_upload_timer_id, APP_TIMER_MODE_REPEATED, service_geolocation_upload_timeout_handler);
	APP_ERROR_CHECK(err_code);

	// Initialize buttons (note: it MUST be done after initializing the timer!)
	app_button_cfg_t app_button_cfg[] =
	{
			{LEFT_BUTTON_PIN_NUMBER, false, NRF_GPIO_PIN_PULLUP, app_button_handler},
			{RIGHT_BUTTON_PIN_NUMBER, false, NRF_GPIO_PIN_PULLUP, app_button_handler}
	};
	app_button_init(app_button_cfg, sizeof(app_button_cfg) / sizeof(app_button_cfg[0]), BUTTON_DETECTION_DELAY);
	err_code = app_button_enable();
	APP_ERROR_CHECK(err_code);

	// Initialize services
	service_time_init();
	service_fona_init();
	service_geolocation_init();

	// Initialize the BLE facade
	ble_facade_init(service_fona_get_imei());

	// Initialize the activities
	activity_tour_configuration_init();
	activity_main_init();
	activity_call_for_help_init();
	activity_guide_call_init();

	activity_header_start();
	activity_manager_start_activity(activity_tour_configuration_get_context());

	// Start timers
	err_code = app_timer_start(m_status_led_timer_id, STATUS_LED_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_display_message_panel_timer_id, DISPLAY_MESSAGE_PANEL_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_service_time_update_timer_id, SERVICE_TIME_UPDATE_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_activity_header_refresh_timer_id, ACTIVITY_HEADER_REFRESH_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_service_geolocation_upload_timer_id, SERVICE_GEOLOCATION_UPLOAD_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);

	LOG("Started\r\n");

	while (true) {
		ble_facade_wait_for_event();

		// Call the main_loop_callables
		for (uint8_t i = 0; i < m_nb_main_loop_callables; i++)
		{
			m_main_loop_callables[i]();
		}
		m_nb_main_loop_callables = 0;
	}
}

bool call_in_main_loop(main_loop_callable_t callable) {
	if (m_nb_main_loop_callables >= MAX_MAIN_LOOP_CALLABLES)
	{
		return false;
	}

	m_main_loop_callables[m_nb_main_loop_callables] = callable;
	m_nb_main_loop_callables++;

	return true;
}

/**
 * Function called when the STATUS_LED timer tick.
 */
static void status_led_timeout_handler(void* p_context) {
	UNUSED_PARAMETER(p_context);
	nrf_gpio_pin_toggle(STATUS_LED_PIN_NUMBER);
}

/**
 * Function called when the display_message_panel timer tick.
 */
static void display_message_panel_timeout_handler(void* p_context) {
	UNUSED_PARAMETER(p_context);
	display_message_panel_change_page();
}

/**
 * Function called when the service_time_update timer tick.
 */
static void service_time_update_timeout_handler(void* p_context) {
	UNUSED_PARAMETER(p_context);
	service_time_update();
}

/**
 * Function called when the activity_header_refresh timer tick.
 */
static void activity_header_refresh_timeout_handler(void* p_context) {
	UNUSED_PARAMETER(p_context);
	activity_header_refresh();
}

/**
 * Function called when the service_geolocation_upload timer tick.
 */
static void service_geolocation_upload_timeout_handler(void* p_context) {
	UNUSED_PARAMETER(p_context);
	service_geolocation_send_current_geolocation();
}

/**
 * Function called when a button is pressed.
 */
static void app_button_handler(uint8_t pin_no, uint8_t button_action) {
	if (button_action == APP_BUTTON_PUSH)
	{
		if (LEFT_BUTTON_PIN_NUMBER == pin_no)
			display_menu_handle_left_button_pushed();
		else if (RIGHT_BUTTON_PIN_NUMBER == pin_no)
			display_menu_handle_right_button_pushed();
	}
}
