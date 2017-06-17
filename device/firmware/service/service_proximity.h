#ifndef _SERVICE_PROXIMITY_H_
#define _SERVICE_PROXIMITY_H_

#include "../enums/proximity_level.h"

/**
 * @brief Set the current proximity level.
 */
void service_proximity_set_level(proximity_level level);

/**
 * @brief Current proximity level.
 */
proximity_level service_proximity_get_level();

#endif
