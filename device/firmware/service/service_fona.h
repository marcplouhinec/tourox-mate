#ifndef _SERVICE_FONA_H_
#define _SERVICE_FONA_H_

#include <stdbool.h>
#include <stdint.h>

#define MAX_FONA_EVENT_LISTENERS 3
#define PIN_NUMBER_SIZE 5
#define PHONE_NUMBER_SIZE 30
#define IMEI_SIZE 16
#define APN_SIZE 65
#define APN_USERNAME_SIZE 33
#define APN_PASSWORD_SIZE 33

/**
 * @brief Type of event the FONA can send.
 */
typedef enum {
	SERVICE_FONA_RING = 0,       // Somebody call the FONA
	SERVICE_FONA_NO_CARRIER = 1, // The calling person stop his call
	SERVICE_FONA_BUSY = 2        // The calling person doesn't want to answer
} service_fona_event_type;

/**
 * @brief Function called when a FONA event occurs.
 */
typedef void (*service_fona_event_handler_t) (service_fona_event_type event_type);

/**
 * @brief Initialize the service.
 *
 * Warning: cannot be called in an interruption context!
 */
void service_fona_init();

/**
 * @brief Set the SIM card PIN number and APN information.
 *
 * Warning: cannot be called in an interruption context!
 */
void service_fona_unlock_sim(char* pin_number, char* apn, char* apn_username, char* apn_password);

/**
 * @brief Get the IMEI number (constant).
 *
 * Warning: cannot be called in an interruption context!
 */
char* service_fona_get_imei();

/**
 * @brief Get the current device location in the format: "longitude,latitude,date,time" ("-xx.dddddd,-xx.dddddd,yyyy/mm/dd,hh:mm:ss").
 *
 * Note that the date time is in UTC.
 *
 * Warning: cannot be called in an interruption context!
 *
 * @param[in] p_lng_lat_date_time  Buffer where the result is copied.
 * @param[in] max_length           Maximum length of latLngDateTime.
 * @return true if the location is available, false if not.
 */
bool service_fona_get_geolocation(char* p_lng_lat_date_time, uint8_t max_length);

/**
 * @brief Get the battery percentage.
 *
 * Warning: cannot be called in an interruption context!
 */
uint8_t service_fona_get_battery_percentage();

/**
 * @brief Get the GSM signal level in percentage.
 *
 * Warning: cannot be called in an interruption context!
 */
uint8_t service_fona_get_gsm_signal_percentage();

/**
 * @brief Set the Guide's phone number.
 */
void service_fona_set_guide_phone_number(char* phone_number);

/**
 * @brief Give a phone call to the guide.
 *
 * Warning: cannot be called in an interruption context!
 *
 * @return true if the guide can be called, false if there is an error.
 */
bool service_fona_call_guide();

/**
 * @brief Pick up an incoming call.
 *
 * Warning: cannot be called in an interruption context!
 */
void service_fona_pick_up();

/**
 * @brief Stop calling the guide.
 *
 * Warning: cannot be called in an interruption context!
 */
void service_fona_hang_up();

/**
 * @brief Send a HTTP GET request but do not expect any response body.
 *
 * Warning: cannot be called in an interruption context!
 *
 * @param[in] url URL of the GET request.
 * @return status code
 */
uint16_t service_fona_send_http_get_request_and_expect_response_without_body(char* url);

/**
 * @brief Add a handler for FONA events.
 *
 * @return false if the event handler couldn't be registered. true if yes.
 */
bool service_fona_register_event_handler(service_fona_event_handler_t event_handler);

#endif
