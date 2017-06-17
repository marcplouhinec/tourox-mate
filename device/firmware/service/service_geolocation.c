#include "service_geolocation.h"
#include <stddef.h>
#include <string.h>
#include "../main_loop_call.h"
#include "service_fona.h"

#define SEND_GEOLOCATION_URL               "http://www.tourox.io/sl/imeiimeiimeiime/-xx.dddddd/-xx.dddddd"
#define SEND_GEOLOCATION_URL_LENGTH        (sizeof(SEND_GEOLOCATION_URL))
#define SEND_GEOLOCATION_URL_IMEI_POS      (sizeof("http://nwww.tourox.io/sl"))
#define SEND_GEOLOCATION_URL_LNGLAT_POS    (sizeof("http://www.tourox.io/sl/imeiimeiimeiime"))
#define SEND_GEOLOCATION_LNGLAT_MAX_LENGTH (sizeof("-xx.dddddd/-xx.dddddd"))

// Private functions declaration
static void service_geolocation_send_current_geolocation_in_main_loop();

// Private variables
static char m_send_geolocation_url[SEND_GEOLOCATION_URL_LENGTH];


void service_geolocation_init() {
	// Initialize the URL
	strcpy(m_send_geolocation_url, SEND_GEOLOCATION_URL);

	// Copy the IMEI in the URL
	char* imei = service_fona_get_imei();
	strncpy(m_send_geolocation_url + SEND_GEOLOCATION_URL_IMEI_POS - 1, imei, IMEI_SIZE - 1);
}

void service_geolocation_send_current_geolocation() {
	call_in_main_loop(service_geolocation_send_current_geolocation_in_main_loop);
}

static void service_geolocation_send_current_geolocation_in_main_loop() {
	if (service_fona_get_geolocation(m_send_geolocation_url + SEND_GEOLOCATION_URL_LNGLAT_POS, SEND_GEOLOCATION_LNGLAT_MAX_LENGTH))
	{
		// The URL is in the form http://<url>/lng,lat,yyy, we need to replace the first ',' by a '/' and then the second ',' by a '\0'.
		char* p_comma = strchr(m_send_geolocation_url + SEND_GEOLOCATION_URL_LNGLAT_POS, ',');
        if (p_comma != NULL)
        	p_comma[0] = '/';
        p_comma = strchr(m_send_geolocation_url + SEND_GEOLOCATION_URL_LNGLAT_POS, ',');
        if (p_comma != NULL)
        	p_comma[0] = '\0';

        service_fona_send_http_get_request_and_expect_response_without_body(m_send_geolocation_url);
	}
}
