#include "display_header.h"
#include "display_configuration.h"
#include "../Adafruit_SHARP_Memory_Display/Adafruit_SharpMem.h"

// Macros used to write binary literals (thanks to http://embeddedgurus.com/barr-code/2009/09/binary-literals-in-c/).
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)
#define B8(d) ((unsigned char)B8__(HEX__(d)))

static const uint8_t disconnectedProximityLevelBitmap[];
static const uint8_t veryFarProximityLevelBitmap[];
static const uint8_t farProximityLevelBitmap[];
static const uint8_t averageProximityLevelBitmap[];
static const uint8_t closeProximityLevelBitmap[];
static const uint8_t veryCloseProximityLevelBitmap[];

void display_header_show_time(char* time) {
	Adafruit_GFX_fillRect(0, 0, 48, HEADER_PANEL_HEIGHT - 1, WHITE);

	Adafruit_GFX_setCursor(0, 0);
	Adafruit_GFX_writeText(time);

	Adafruit_GFX_drawFastHLine(0, HEADER_PANEL_HEIGHT - 1, SCREEN_WIDTH, BLACK);
	Adafruit_SharpMem_refresh();
}

void display_header_show_proximity_level(proximity_level proximity_level) {
    const int16_t originX = SCREEN_WIDTH - 12 - 9; // left
    const int16_t originY = 0; // top

    const uint8_t* proximityLevelBitmap;
    switch (proximity_level) {
        case VERY_FAR:
            proximityLevelBitmap = veryFarProximityLevelBitmap;
            break;
        case FAR:
            proximityLevelBitmap = farProximityLevelBitmap;
            break;
        case AVERAGE:
            proximityLevelBitmap = averageProximityLevelBitmap;
            break;
        case CLOSE:
            proximityLevelBitmap = closeProximityLevelBitmap;
            break;
        case VERY_CLOSE:
            proximityLevelBitmap = veryCloseProximityLevelBitmap;
            break;
        default:
            proximityLevelBitmap = disconnectedProximityLevelBitmap;
            break;
    }

    Adafruit_GFX_drawBitmap(originX, originY, proximityLevelBitmap, 9, 7, BLACK);
	Adafruit_SharpMem_refresh();
}

void display_header_show_gsm_signal_percentage(uint8_t gsm_signal_percentage) {
	Adafruit_GFX_drawFastVLine(SCREEN_WIDTH - 2 , 0, 6, gsm_signal_percentage >= 80 ? BLACK : WHITE);
	Adafruit_GFX_drawFastVLine(SCREEN_WIDTH - 4 , 1, 5, gsm_signal_percentage >= 60 ? BLACK : WHITE);
	Adafruit_GFX_drawFastVLine(SCREEN_WIDTH - 6 , 2, 4, gsm_signal_percentage >= 40 ? BLACK : WHITE);
	Adafruit_GFX_drawFastVLine(SCREEN_WIDTH - 8 , 3, 3, gsm_signal_percentage >= 20 ? BLACK : WHITE);
	Adafruit_GFX_drawFastVLine(SCREEN_WIDTH - 10, 4, 2, gsm_signal_percentage >=  0 ? BLACK : WHITE);

	Adafruit_GFX_drawFastHLine(0, HEADER_PANEL_HEIGHT - 1, SCREEN_WIDTH, BLACK);
	Adafruit_SharpMem_refresh();
}

void display_header_show_battery_percentage(uint8_t battery_percentage) {
	const int16_t originX = SCREEN_WIDTH - 13 - 9 - 14; // left
	const int16_t originY = 0; // top

	Adafruit_GFX_drawFastVLine(originX + 11, originY + 2, 3, BLACK);
	Adafruit_GFX_drawFastVLine(originX + 10, originY, 2, BLACK);
	Adafruit_GFX_drawFastVLine(originX + 10, originY + 5, 2, BLACK);
	Adafruit_GFX_drawFastVLine(originX, originY, 7, BLACK);
	Adafruit_GFX_drawFastHLine(originX, originY, 11, BLACK);
	Adafruit_GFX_drawFastHLine(originX, originY + 6, 11, BLACK);

	for (uint8_t i = 1; i <= 10; i++)
	{
		Adafruit_GFX_drawFastVLine(
				originX + i,
				i < 10 ? originY + 1 : originY + 2,
				i < 10 ? 5 : 3,
				battery_percentage > (i - 1) * 10 ? BLACK : WHITE);
	}

	Adafruit_SharpMem_refresh();
}

static const uint8_t disconnectedProximityLevelBitmap[] = {
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000),
    B8(000000000)
};

static const uint8_t veryFarProximityLevelBitmap[] = {
    B8(000000100),
    B8(000001010),
    B8(000010010),
    B8(000100001),
    B8(001000001),
    B8(000100001),
    B8(000010010),
    B8(000001010),
    B8(000000100)
};

static const uint8_t farProximityLevelBitmap[] = {
    B8(000000100),
    B8(000001010),
    B8(000010010),
    B8(000100001),
    B8(001100001),
    B8(000100001),
    B8(000010010),
    B8(000001010),
    B8(000000100)
};

static const uint8_t averageProximityLevelBitmap[] = {
    B8(000000100),
    B8(000001010),
    B8(000010010),
    B8(000111001),
    B8(001111001),
    B8(000111001),
    B8(000010010),
    B8(000001010),
    B8(000000100)
};

static const uint8_t closeProximityLevelBitmap[] = {
    B8(000000100),
    B8(000001010),
    B8(000011010),
    B8(000111101),
    B8(001111101),
    B8(000111101),
    B8(000011010),
    B8(000001010),
    B8(000000100)
};

static const uint8_t veryCloseProximityLevelBitmap[] = {
    B8(000000100),
    B8(000001110),
    B8(000011110),
    B8(000111111),
    B8(001111111),
    B8(000111111),
    B8(000011110),
    B8(000001110),
    B8(000000100)
};
