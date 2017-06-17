#include "activity_manager.h"
#include <stddef.h>

// Private variables
static activity_context_t* m_p_running_activity_context = NULL;


void activity_manager_start_activity(activity_context_t* p_activity_context) {
	if (m_p_running_activity_context != NULL)
	{
		m_p_running_activity_context->activity_on_stop_event_handler();
	}

	m_p_running_activity_context = p_activity_context;
	m_p_running_activity_context->activity_on_start_event_handler();
}
