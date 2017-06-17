#include "service_proximity.h"

// Private variables
static proximity_level m_current_level = DISCONNECTED;


void service_proximity_set_level(proximity_level level) {
	m_current_level = level;
}

proximity_level service_proximity_get_level() {
	return m_current_level;
}
