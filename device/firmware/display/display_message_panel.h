#ifndef _DISPLAY_MESSAGE_PANEL_H_
#define _DISPLAY_MESSAGE_PANEL_H_

/**
 * @brief Show the given message.
 *
 * Note 1: the message can be bigger than the screen, in this case it will be split in pages that will automatically change.
 * Note 2: the message is not copied locally so it cannot be a short-lived variable in the stack.
 */
void display_message_panel_show_text(char* message);

/**
 * @brief Function to be called by a timer in order to to change the page when the displayed text is too long.
 *
 * Note that this method has no effect when the text is short enough to fit in one page.
 */
void display_message_panel_change_page();

#endif
