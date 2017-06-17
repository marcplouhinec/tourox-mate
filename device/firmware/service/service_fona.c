#include "service_fona.h"
#include <stddef.h>
#include <string.h>
#include "../Adafruit_FONA_Library/Adafruit_FONA.h"
#include "nrf51_bitfields.h"
#include "service_time.h"
#include "../util/logging.h"

#define BATTERY_PERCENTAGE_CACHE_DURATION_IN_SECONDS 60
#define GSM_SIGNAL_PERCENTAGE_CACHE_DURATION_IN_SECONDS 20

// Private functions declaration
static void fona_event_handler(Adafruit_FONA_event_type event_type);

// Private variables
static bool m_fona_available = false;
static bool m_sim_unlocked = false;
static bool m_gprs_enabled = false;
static uint32_t m_last_battery_measurement_time_since_startup = 0;
static uint32_t m_last_gsm_signal_measurement_time_since_startup = 0;
static uint16_t m_battery_pecentage = 0;
static uint16_t m_gsm_signal_pecentage = 0;
static char m_imei[IMEI_SIZE] = {'\0'};
static char m_guide_phone_number[PHONE_NUMBER_SIZE] = {'\0'};
static service_fona_event_handler_t m_event_handlers[MAX_FONA_EVENT_LISTENERS] = {NULL};
static uint8_t m_nb_event_handlers = 0;


void service_fona_init() {
	if (Adafruit_FONA_begin(UART_BAUDRATE_BAUDRATE_Baud9600))
	{
		m_fona_available = true;
		Adafruit_FONA_setAudio(FONA_EXTAUDIO);
		LOG("FONA initialized\r\n");
	}
}

void service_fona_unlock_sim(char* pin_number, char* apn, char* apn_username, char* apn_password) {
	if (!m_fona_available)
	{
		return ;
	}
	LOG("FONA Unlock SIM...\r\n");
	LOGF("  pin_number = '%s'\r\n", pin_number);
	LOGF("  apn = '%s'\r\n", apn);
	LOGF("  apn_username = '%s'\r\n", apn_username);
	LOGF("  apn_password = '%s'\r\n", apn_password);

	// Check if the SIM is not already unlocked
	uint8_t network_status =  Adafruit_FONA_getNetworkStatus();
	LOGF("  network_status = '%u'\r\n", network_status);
	if (network_status == 0)
	{
		// SIM locked, un-lock it...
		if (Adafruit_FONA_unlockSIM(pin_number))
		{
			LOG("FONA SIM unlocked\r\n");
		}
		else
		{
			LOG("FONA error: SIM cannot be unlocked\r\n");
			return;
		}
	}
	else
	{
		LOG("FONA SIM already unlocked\r\n");
	}

	Adafruit_FONA_setGPRSNetworkSettings(apn, apn_username, apn_password);
	m_sim_unlocked = true;
}

void service_fona_set_guide_phone_number(char* phone_number) {
	strncpy(m_guide_phone_number, phone_number, PHONE_NUMBER_SIZE);
	m_guide_phone_number[PHONE_NUMBER_SIZE - 1] = '\0';
	LOGF("FONA set guide phone number: %s\r\n", m_guide_phone_number);
}

char* service_fona_get_imei() {
	if (m_fona_available && m_imei[0] == '\0')
	{
		Adafruit_FONA_getIMEI(m_imei);
		m_imei[IMEI_SIZE - 1] = '\0';
	}
	return m_imei;
}

bool service_fona_get_geolocation(char* p_lng_lat_date_time, uint8_t max_length) {
    if (!m_fona_available || !m_sim_unlocked)
    {
    	return false;
    }
    LOGF("FONA get_geolocation (max_length = %u)...\r\n", max_length);

    if(!m_gprs_enabled)
    {
		LOG("FONA Enable GPRS...\r\n");
		if (Adafruit_FONA_enableGPRS(true))
		{
			LOG("FONA GPRS enabled\r\n");
		}
		else
		{
			LOG("FONA ERROR Unable to enable GPRS - try disabling and enabling it again\r\n");
			Adafruit_FONA_enableGPRS(false);
			if (Adafruit_FONA_enableGPRS(true))
			{
				LOG("FONA GPRS finally enabled\r\n");
			}
			else
			{
				LOG("FONA ERROR Definitely unable to enable GPRS\r\n");
				return false;
			}
		}
    	m_gprs_enabled = true;
    }

    uint16_t returncode;
    if (!Adafruit_FONA_getGSMLoc_raw(&returncode, p_lng_lat_date_time, max_length))
    {
    	LOG("FONA Unable to get the geolocation\r\n");
    	return false;
    }

    if (returncode != 0)
    {
    	LOGF("FONA Unable to get the geo location. return code: %u\r\n", returncode);
    	return false;
    }

    LOGF("FONA Geo location: %s\r\n", p_lng_lat_date_time);
    return true;
}

uint8_t service_fona_get_battery_percentage() {
	if (!m_fona_available)
	{
		return 0;
	}

	// Cache the battery level in order to avoid reading it too often
	uint32_t time_since_startup = service_time_get_time_since_startup();
	if (m_last_battery_measurement_time_since_startup == 0 || time_since_startup - m_last_battery_measurement_time_since_startup > BATTERY_PERCENTAGE_CACHE_DURATION_IN_SECONDS)
	{
		Adafruit_FONA_getBattPercent(&m_battery_pecentage);
		m_last_battery_measurement_time_since_startup = time_since_startup;
	}

	return (uint8_t) m_battery_pecentage;
}

uint8_t service_fona_get_gsm_signal_percentage() {
	if (!m_fona_available)
	{
		return 0;
	}

	// Cache the gsm signal level in order to avoid reading it too often
	uint32_t time_since_startup = service_time_get_time_since_startup();
	if (m_last_gsm_signal_measurement_time_since_startup == 0 || time_since_startup - m_last_gsm_signal_measurement_time_since_startup > GSM_SIGNAL_PERCENTAGE_CACHE_DURATION_IN_SECONDS)
	{
		// Get the RSSI and convert it into dBm (thanks to the FONAtest)
		uint8_t raw_rssi = Adafruit_FONA_getRSSI();
		int dbm_rssi = 0;
		if (raw_rssi == 0)
		{
			dbm_rssi = -115;
		}
		else if (raw_rssi == 1)
		{
			dbm_rssi = -111;
		}
		else if (raw_rssi == 31)
		{
			dbm_rssi = -52;
		}
		else if ((raw_rssi >= 2) && (raw_rssi <= 30))
		{
			dbm_rssi = (raw_rssi - 2) * (-54 - -110) / (30 - 2) + -110;
		}

		// Convert the RSSI into percentage (thanks to http://stackoverflow.com/a/15798024)
		m_gsm_signal_pecentage = 0;
		if (dbm_rssi <= -100)
		{
			m_gsm_signal_pecentage = 0;
		}
		else if (dbm_rssi >= -50)
		{
			m_gsm_signal_pecentage = 100;
		}
		else
		{
			m_gsm_signal_pecentage = 2 * (dbm_rssi + 100);
		}

		// Update the cache time
		m_last_gsm_signal_measurement_time_since_startup = time_since_startup;
	}

    return m_gsm_signal_pecentage;
}

bool service_fona_call_guide() {
	if (!m_fona_available || m_guide_phone_number[0] == 0)
	{
		return false;
	}

	LOGF("  FONA_callPhone '%s'\r\n", m_guide_phone_number);
	return Adafruit_FONA_callPhone(m_guide_phone_number);
}

void service_fona_pick_up() {
	if (!m_fona_available)
	{
		return;
	}

	LOG("FONA pick up\r\n");
	Adafruit_FONA_pickUp();
}

void service_fona_hang_up() {
	if (!m_fona_available)
	{
		return;
	}

	LOG("FONA hang up\r\n");
	Adafruit_FONA_hangUp();
}

uint16_t service_fona_send_http_get_request_and_expect_response_without_body(char* url) {
	if (!m_fona_available)
	{
		return 500;
	}

	LOGF("FONA Send GET request: %s...\r\n", url);

    uint16_t statuscode = 500;
    int16_t length;

    if (!Adafruit_FONA_HTTP_GET_start(url, &statuscode, (uint16_t*) &length))
    {
    	LOG("FONA ERROR unable to send the GET request\r\n");
    	return 500;
    }

    Adafruit_FONA_HTTP_GET_end();

    LOGF("FONA GET request sent. status: %u...\r\n", statuscode);

    return statuscode;
}

bool service_fona_register_event_handler(service_fona_event_handler_t event_handler) {
	if (event_handler == NULL || m_nb_event_handlers >= MAX_FONA_EVENT_LISTENERS)
	{
		return false;
	}
	m_event_handlers[m_nb_event_handlers] = event_handler;
	m_nb_event_handlers++;

	// Register an Adafruit_FONA_event_handler if it is the first registered service_fona_event_handler
	if (m_nb_event_handlers == 1)
	{
		Adafruit_FONA_register_event_handler(fona_event_handler);
	}
	return true;
}

static void fona_event_handler(Adafruit_FONA_event_type event_type)
{
	if (m_nb_event_handlers == 0)
	{
		return;
	}

	if (event_type == RING)
	{
		LOG("FONA RING event\r\n");
		for (uint8_t i = 0; i < m_nb_event_handlers; i++)
		{
			m_event_handlers[i](SERVICE_FONA_RING);
		}
	}
	else if (event_type == NO_CARRIER)
	{
		LOG("FONA NO_CARRIER event\r\n");
		for (uint8_t i = 0; i < m_nb_event_handlers; i++)
		{
			m_event_handlers[i](SERVICE_FONA_NO_CARRIER);
		}
	}
	else if (event_type == BUSY)
	{
		LOG("FONA BUSY event\r\n");
		for (uint8_t i = 0; i < m_nb_event_handlers; i++)
		{
			m_event_handlers[i](SERVICE_FONA_BUSY);
		}
	}
}
