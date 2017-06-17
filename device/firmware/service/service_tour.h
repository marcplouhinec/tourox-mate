#ifndef _SERVICE_TOUR_H_
#define _SERVICE_TOUR_H_

#include <stdbool.h>

/**
 * @brief Function called when the tour configuration is set or cleared.
 */
typedef void (*service_tour_configuration_change_event_handler_t) (bool tour_configured);

/**
 * @brief Indicate if the tour is configured or not.
 *
 * @param[in] tour_configured true if the tour is configured, false if not.
 */
void service_tour_set_tour_configured(bool tour_configured);

/**
 * @return true true if the tour is configured, false if not.
 */
bool service_tour_is_tour_configured();

/**
 * @brief Register an event handler that will be called when the tour configuration is set or cleared.
 */
void service_tour_set_configuration_change_event_handler(service_tour_configuration_change_event_handler_t configuration_change_event_handler);

#endif
