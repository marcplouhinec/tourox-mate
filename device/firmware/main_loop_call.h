#ifndef _MAINLOOPEXECUTION_H_
#define _MAINLOOPEXECUTION_H_

#include <stdbool.h>

/**
 * @brief Function called as soon as possible in the main loop.
 */
typedef void (*main_loop_callable_t) ();

/**
 * @brief Put the given function in an internal queue in order to be executed in the main loop.
 *
 * Because some code like the one interacting with the FONA can only be executed in the main loop,
 * this utility header give the possibility for other file to execute some code in it.
 *
 * @return true if the callable was successfully put in the internal queue, false if not.
 */
bool call_in_main_loop(main_loop_callable_t callable);

#endif
