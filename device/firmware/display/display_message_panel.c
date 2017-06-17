#include "display_message_panel.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "nordic_common.h"
#include "display_configuration.h"
#include "../Adafruit_SHARP_Memory_Display/Adafruit_SharpMem.h"

#define CEILING(x,y) (((x) + (y) - 1) / (y))

#define MESSAGE_PANEL_HEIGHT (SCREEN_HEIGHT - HEADER_PANEL_HEIGHT - MENU_PANEL_HEIGHT)
#define MAX_LINES_PER_PAGE   ((MESSAGE_PANEL_HEIGHT - PADDING_TOP - PADDING_BOTTOM - PAGE_INDICATOR_SIZE) / FONT_HEIGHT_WITH_MARGIN)
#define MESSAGE_PANEL_ORIGIN_Y (HEADER_PANEL_HEIGHT)
#define PADDING_TOP 1
#define PADDING_BOTTOM 1
#define PAGE_INDICATOR_SIZE 4
#define PAGE_INDICATOR_MARGIN_RIGHT 1

/**
 * Information about a line displayed on the screen.
 */
typedef struct {
	uint16_t character_index;
	uint16_t nb_characters;
} message_line_t;


// Private functions declaration
static void display_current_page();


// Private variables
static char* m_message = NULL;
static uint8_t m_current_page_index = 0;
static uint8_t m_nb_message_lines = 0;
static message_line_t m_message_lines[16];


void display_message_panel_show_text(char* message) {
	m_message = message;
	m_current_page_index = 0;

	// Split the message in lines
	m_nb_message_lines = 0;
	uint16_t character_index = 0;
	uint16_t last_space_character_index = -1;
	message_line_t* current_line = NULL;
	uint16_t message_length = strlen(message);
	while (character_index < message_length)
	{
		// Extract character data
		char current_character = message[character_index];
		bool has_next_character = character_index + 1 < message_length;
		char next_character = has_next_character ? message[character_index + 1] : '\0';
		if (current_character == ' ')
		{
			last_space_character_index = character_index;
		}

		// Create a new line if necessary
		if (current_line == NULL)
		{
			if (current_character != ' ') // Ignore space characters at the beginning of a line
			{
				current_line = m_message_lines + m_nb_message_lines;
				current_line->character_index = character_index;
				current_line->nb_characters = 1;
				m_nb_message_lines++;
			}
		}
		else
		{
			current_line->nb_characters++;

			// Check if we are at the end of a line
			if (current_line->nb_characters == MAX_CHARACTERS_PER_LINE)
			{
				// Check if we are not cutting a word
				if (has_next_character && current_character != ' ' && next_character != ' ')
				{
					// Check if the current word is shorter than a line
					uint16_t wordSize = strcspn(message + character_index, " ") + character_index - (last_space_character_index + 1);
					if (wordSize <= MAX_CHARACTERS_PER_LINE)
					{
						// Shorten the current line in order to put the current word on the next line
						current_line->nb_characters -= character_index - last_space_character_index;
						character_index = last_space_character_index; // Note that the character_index is incremented at the end of the loop
					}
				}

				// Remove trailing spaces
				while (message[character_index] == ' ')
				{
					character_index--;
					current_line->nb_characters--;
				}

				current_line = NULL;
			}
		}

		character_index++;
	}

	// Show the current page
	display_current_page();
}

void display_message_panel_change_page() {
	uint8_t nb_pages = CEILING(((double) m_nb_message_lines), ((double) MAX_LINES_PER_PAGE));
	if (nb_pages > 1)
	{
		m_current_page_index++;
		if (m_current_page_index >= nb_pages)
		{
			m_current_page_index = 0;
		}

		display_current_page();
	}
}

/**
 * Print the current page.
 */
static void display_current_page() {
    // Erase the panel
	Adafruit_GFX_fillRect(0, MESSAGE_PANEL_ORIGIN_Y, SCREEN_WIDTH, MESSAGE_PANEL_HEIGHT, WHITE);

    // Center the text vertically if applicable
	uint16_t cursor_y = MESSAGE_PANEL_ORIGIN_Y + PADDING_TOP;
	uint16_t line_start_index = m_current_page_index * MAX_LINES_PER_PAGE;
	uint16_t line_end_index = MIN((m_current_page_index + 1) * MAX_LINES_PER_PAGE, m_nb_message_lines) - 1;
	uint16_t nb_displayed_lines = line_end_index - line_start_index + 1;
    if (nb_displayed_lines < MAX_LINES_PER_PAGE)
    {
        cursor_y += ((MAX_LINES_PER_PAGE - nb_displayed_lines) * FONT_HEIGHT_WITH_MARGIN) / 2;
    }

    // Print the lines
    char text[MAX_CHARACTERS_PER_LINE + 1];
    memset(&text, 0, sizeof(text));
    for (uint16_t line_index = line_start_index; line_index <= line_end_index; line_index++)
    {
        // Extract the characters to print
    	message_line_t line = m_message_lines[line_index];
        strncpy(text, m_message + line.character_index, line.nb_characters);
        text[line.nb_characters] = '\0';

        // Draw the text in the line center
        uint16_t cursor_x = ((MAX_CHARACTERS_PER_LINE - line.nb_characters) * FONT_WIDTH_WITH_MARGIN) / 2;
        Adafruit_GFX_setCursor(cursor_x, cursor_y);
        Adafruit_GFX_writeText(text);
        cursor_y += FONT_HEIGHT_WITH_MARGIN;
    }

    // Show the page indicator if applicable
    bool show_page_indicator = m_nb_message_lines > MAX_LINES_PER_PAGE;
    if (show_page_indicator)
    {
    	uint8_t nb_pages = CEILING(((double) m_nb_message_lines), ((double) MAX_LINES_PER_PAGE));

    	uint16_t indicator_cursor_x = SCREEN_WIDTH - nb_pages * (PAGE_INDICATOR_SIZE + PAGE_INDICATOR_MARGIN_RIGHT);
        for (uint8_t i = 0; i < nb_pages; i++)
        {
            if (m_current_page_index == i)
            {
            	Adafruit_GFX_fillRect(
                    indicator_cursor_x,
                    MESSAGE_PANEL_ORIGIN_Y + MESSAGE_PANEL_HEIGHT - PADDING_BOTTOM - PAGE_INDICATOR_SIZE,
                    PAGE_INDICATOR_SIZE,
                    PAGE_INDICATOR_SIZE,
                    BLACK);
            }
            else
            {
            	Adafruit_GFX_drawRect(
                    indicator_cursor_x,
                    MESSAGE_PANEL_ORIGIN_Y + MESSAGE_PANEL_HEIGHT - PADDING_BOTTOM - PAGE_INDICATOR_SIZE,
                    PAGE_INDICATOR_SIZE,
                    PAGE_INDICATOR_SIZE,
                    BLACK);
            }
            indicator_cursor_x += PAGE_INDICATOR_SIZE + PAGE_INDICATOR_MARGIN_RIGHT;
        }
    }

    Adafruit_SharpMem_refresh();
}
