#ifndef _SERVICE_GEOLOCATION_H_
#define _SERVICE_GEOLOCATION_H_

/**
 * @brief Initialize the service.
 */
void service_geolocation_init();

/**
 * @brief Function to be called by a timer at regular interval in order to let the device to send its location to the server.
 */
void service_geolocation_send_current_geolocation();

#endif
