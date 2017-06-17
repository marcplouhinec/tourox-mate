#ifndef _ACTIVITY_MANAGER_H_
#define _ACTIVITY_MANAGER_H_

/**
 * @brief Function that all activity must implement in order to handle the start event.
 *
 * Note: this function should be called by the activity_manager.
 */
typedef void (*activity_on_start_event_handler_t) ();

/**
 * @brief Function that all activity must implement in order to handle the stop event.
 *
 * Note: this function should be called by the activity_manager.
 */
typedef void (*activity_on_stop_event_handler_t) ();

/**
 * @brief Information required by the activity_manager to start an activity.
 */
typedef struct {
	activity_on_start_event_handler_t activity_on_start_event_handler;
	activity_on_stop_event_handler_t activity_on_stop_event_handler;
} activity_context_t;

/**
 * @brief Start the given activity.
 *
 * Note: if an activity is currently started, its activity_on_stop_event_handler() function is called before starting the new one.
 */
void activity_manager_start_activity(activity_context_t* p_activity_context);

#endif
