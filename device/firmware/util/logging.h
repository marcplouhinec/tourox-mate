/**
 * @brief Simple set of MACROS used for abstracting logging.
 *
 * By using theses MACROS it is possible to easily compile the application with SEGGER logging enabled or not.
 *
 * @author Marc Plouhinec
 */

#ifndef _UTIL_LOGGING_H_
#define _UTIL_LOGGING_H_

#if defined(INCLUDESEGGERRTT)

	#include "SEGGER_RTT.h"

	#define LOG(x)      (SEGGER_RTT_WriteString(0, x))

	#define LOGF(x, y)  (SEGGER_RTT_printf(0, x, y))

#else

	#define LOG(x)

	#define LOGF(x, y)

#endif

#endif
