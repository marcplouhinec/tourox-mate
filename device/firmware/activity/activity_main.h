#ifndef _ACTIVITY_MAIN_H_
#define _ACTIVITY_MAIN_H_

#include "activity_manager.h"

/**
 * @brief Initialize the activity.
 */
void activity_main_init();

/**
 * @brief Get the activity context.
 */
activity_context_t* activity_main_get_context();

#endif
