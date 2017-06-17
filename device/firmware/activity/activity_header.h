/**
 * @brief Header activity
 *
 * This activity is responsible from displaying time and status information on the header.
 *
 * Note: this activity is a bit special because it must be started at the beginning of the application and running forever;
 * this is why it cannot be managed by the activity_manager.
 */

#ifndef _ACTIVITY_HEADER_H_
#define _ACTIVITY_HEADER_H_

/**
 * @brief Start the activity.
 */
void activity_header_start();

/**
 * @brief Refresh the header.
 *
 * Note: this function should be called from a timer.
 */
void activity_header_refresh();

#endif
