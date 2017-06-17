#include "display_menu.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "display_configuration.h"
#include "../Adafruit_SHARP_Memory_Display/Adafruit_SharpMem.h"

// Private variables
static display_menu_item_t* m_p_left_menu_item = NULL;
static display_menu_item_t* m_p_right_menu_item = NULL;


void display_menu_set_menu_items(display_menu_item_t* p_left_menu_item, display_menu_item_t* p_right_menu_item) {
	m_p_left_menu_item = p_left_menu_item;
	m_p_right_menu_item = p_right_menu_item;

	// Erase the menu panel
	Adafruit_GFX_fillRect(0, SCREEN_HEIGHT - (MENU_PANEL_HEIGHT - 1), SCREEN_WIDTH, (MENU_PANEL_HEIGHT - 1), WHITE);

	// Compute the number of menu items
	uint8_t nb_menu_items = (p_left_menu_item == NULL ? 0 : 1) + (p_right_menu_item == NULL ? 0 : 1);

	// Display the menu items differently according to their numbers
	if (nb_menu_items == 1 && p_left_menu_item != NULL) {

		// Truncate the menu item name if it is too long
		uint8_t nb_characters = strlen(p_left_menu_item->name);
		char left_menu_item_name[MAX_CHARACTERS_PER_LINE + 1];
		if (nb_characters > MAX_CHARACTERS_PER_LINE) {
			nb_characters = MAX_CHARACTERS_PER_LINE;
		}
		strncpy(left_menu_item_name, p_left_menu_item->name, nb_characters);
		left_menu_item_name[nb_characters] = '\0';

		// Print the menu item name on the left
		Adafruit_GFX_setCursor(0, SCREEN_HEIGHT - FONT_HEIGHT);
		Adafruit_GFX_writeText(left_menu_item_name);

	} else if (nb_menu_items == 1 && p_right_menu_item != NULL) {

		// Truncate the menu item name if it is too long
		uint8_t nb_characters = strlen(p_right_menu_item->name);
		char right_menu_item_name[MAX_CHARACTERS_PER_LINE + 1];
		if (nb_characters > MAX_CHARACTERS_PER_LINE) {
			nb_characters = MAX_CHARACTERS_PER_LINE;
		}
		strncpy(right_menu_item_name, p_right_menu_item->name, nb_characters);
		right_menu_item_name[nb_characters] = '\0';

		// Print the menu item name on the right
		uint8_t x = SCREEN_WIDTH - (nb_characters * FONT_WIDTH_WITH_MARGIN);
		Adafruit_GFX_setCursor(x, SCREEN_HEIGHT - FONT_HEIGHT);
		Adafruit_GFX_writeText(right_menu_item_name);

	} else if (nb_menu_items == 2) {
		// Truncate the menu item name if it is too long
		uint8_t nb_characters_left  = strlen(p_left_menu_item->name);
		uint8_t nb_characters_right = strlen(p_right_menu_item->name);
		uint8_t nb_characters = nb_characters_left + nb_characters_right;
		char left_menu_item_name[MAX_CHARACTERS_PER_LINE + 1];
		char right_menu_item_name[MAX_CHARACTERS_PER_LINE + 1];
		if (nb_characters > MAX_CHARACTERS_PER_LINE) {
			if (nb_characters_left > MAX_CHARACTERS_PER_LINE / 2) {
				nb_characters_left = MAX_CHARACTERS_PER_LINE / 2;
			}
			if (nb_characters_right > MAX_CHARACTERS_PER_LINE / 2) {
				nb_characters_right = MAX_CHARACTERS_PER_LINE / 2;
			}
		}
		strncpy(left_menu_item_name, p_left_menu_item->name, nb_characters_left);
		left_menu_item_name[nb_characters_left] = '\0';
		strncpy(right_menu_item_name, p_right_menu_item->name, nb_characters_right);
		right_menu_item_name[nb_characters_right] = '\0';

		// Print the menu item name on the left
		Adafruit_GFX_setCursor(0, SCREEN_HEIGHT - FONT_HEIGHT);
		Adafruit_GFX_writeText(left_menu_item_name);

		// Print the menu item name on the right
		uint8_t x = SCREEN_WIDTH - (nb_characters_right * FONT_WIDTH_WITH_MARGIN);
		Adafruit_GFX_setCursor(x, SCREEN_HEIGHT - FONT_HEIGHT);
		Adafruit_GFX_writeText(right_menu_item_name);

		// Print a vertical line between the names
		uint8_t minX = nb_characters_left * FONT_WIDTH_WITH_MARGIN + 1;
		uint8_t maxX = SCREEN_WIDTH - (nb_characters_right * FONT_WIDTH_WITH_MARGIN) - 1;
		Adafruit_GFX_drawFastVLine(minX + (maxX - minX) / 2, SCREEN_HEIGHT - MENU_PANEL_HEIGHT, MENU_PANEL_HEIGHT, BLACK);
	}

	// Draw a horizontal line to show the upper limit of the menu and send the graphic buffer to the screen
	Adafruit_GFX_drawFastHLine(0, SCREEN_HEIGHT - MENU_PANEL_HEIGHT, SCREEN_HEIGHT, BLACK);
	Adafruit_SharpMem_refresh();
}

void display_menu_handle_left_button_pushed() {
	if (m_p_left_menu_item != NULL && m_p_left_menu_item->display_menu_item_selection_handler != NULL)
	{
		m_p_left_menu_item->display_menu_item_selection_handler();
	}
}

void display_menu_handle_right_button_pushed() {
	if (m_p_right_menu_item != NULL && m_p_right_menu_item->display_menu_item_selection_handler != NULL)
	{
		m_p_right_menu_item->display_menu_item_selection_handler();
	}
}
