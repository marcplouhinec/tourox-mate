#ifndef _DISPLAY_MENU_H_
#define _DISPLAY_MENU_H_

/**
 * @brief Function called when a menu item is selected.
 */
typedef void (*display_menu_item_selection_handler_t) ();

/**
 * @brief Menu item definition.
 */
typedef struct {
	char* name;
	display_menu_item_selection_handler_t display_menu_item_selection_handler;
} display_menu_item_t;

/**
 * @brief Display the given menu items.
 *
 * Note that one or both menu items can be NULL. In this case the corresponding menu item is simply not displayed.
 */
void display_menu_set_menu_items(display_menu_item_t* p_left_menu_item, display_menu_item_t* p_right_menu_item);

/**
 * Function that must be called in the file 'main.c' when the left button is pushed.
 */
void display_menu_handle_left_button_pushed();

/**
 * Function that must be called in the file 'main.c' when the right button is pushed.
 */
void display_menu_handle_right_button_pushed();

#endif
