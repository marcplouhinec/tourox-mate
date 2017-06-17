/*********************************************************************
This is an Arduino library for our Monochrome SHARP Memory Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1393

These displays use SPI to communicate, 3 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

 /*
  *  Modified by Marc PLOUHINEC 04/10/2015 for use in nRF51 SDK
  */

#include <string.h>
#include "Adafruit_SharpMem.h"
#include "nrf_gpio.h"

/**************************************************************************
    Sharp Memory Display Connector
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VIN             3.3-5.0V (into LDO supply)
      2   3V3             3.3V out
      3   GND
      4   SCLK            Serial Clock
      5   MOSI            Serial Data Input
      6   CS              Serial Chip Select
      9   EXTMODE         COM Inversion Select (Low = SW clock/serial)
      7   EXTCOMIN        External COM Inversion Signal
      8   DISP            Display On(High)/Off(Low)

 **************************************************************************/

#define SHARPMEM_BIT_WRITECMD   (0x80)
#define SHARPMEM_BIT_VCOM       (0x40)
#define SHARPMEM_BIT_CLEAR      (0x20)
#define TOGGLE_VCOM             do { _sharpmem_vcom = _sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM; } while(0);
#define swap(a, b) { int16_t t = a; a = b; b = t; }

// Private variables
static uint8_t sharpmem_buffer[(SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8];
static uint8_t _sharpmem_vcom;
static uint8_t m_enable_pin_number;
static uint8_t m_cs_pin_number;
static uint8_t m_mosi_pin_number;
static uint8_t m_sclk_pin_number;

// Private functions declaration
static void sendbyte(uint8_t data);
static void sendbyteLSB(uint8_t data);

/* ************* */
/* CONSTRUCTORS  */
/* ************* */

void Adafruit_SharpMem_init(
		uint8_t enable_pin_number,
		uint8_t cs_pin_number,
		uint8_t mosi_pin_number,
		uint8_t miso_unused_pin_number,
		uint8_t sclk_pin_number) {

	Adafruit_GFX_init(SHARPMEM_LCDWIDTH, SHARPMEM_LCDHEIGHT, &Adafruit_SharpMem_drawPixel);
	m_enable_pin_number = enable_pin_number;
	m_cs_pin_number = cs_pin_number;
	m_mosi_pin_number = mosi_pin_number;
	m_sclk_pin_number = sclk_pin_number;

	nrf_gpio_cfg_output(enable_pin_number);
	nrf_gpio_cfg_output(cs_pin_number);
	nrf_gpio_cfg_output(mosi_pin_number);
	nrf_gpio_cfg_output(sclk_pin_number);

	nrf_gpio_pin_clear(enable_pin_number);
	nrf_gpio_pin_clear(cs_pin_number);
	_sharpmem_vcom = SHARPMEM_BIT_VCOM;
}

void Adafruit_SharpMem_begin() {
	Adafruit_GFX_setRotation(2);
}


/* ************** */
/* PUBLIC METHODS */
/* ************** */

// 1<<n is a costly operation on AVR -- table usu. smaller & faster
static const uint8_t
  set[] = {  1,  2,  4,  8,  16,  32,  64,  128 },
  clr[] = { (uint8_t)~1, (uint8_t)~2, (uint8_t)~4, (uint8_t)~8, (uint8_t)~16, (uint8_t)~32, (uint8_t)~64, (uint8_t)~128 };

/**************************************************************************/
/*!
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
*/
/**************************************************************************/
void Adafruit_SharpMem_drawPixel(int16_t x, int16_t y, uint16_t color) {
	if((x < 0) || (x >= Adafruit_GFX_getWidth()) || (y < 0) || (y >= Adafruit_GFX_getHeight())) return;

    switch(Adafruit_GFX_getRotation()) {
        case 1:
            swap(x, y);
            x = SHARPMEM_LCDWIDTH  - 1 - x;
            break;
        case 2:
            x = SHARPMEM_LCDWIDTH  - 1 - x;
            y = SHARPMEM_LCDHEIGHT - 1 - y;
            break;
        case 3:
            swap(x, y);
            y = SHARPMEM_LCDHEIGHT - 1 - y;
            break;
    }

    if(color) {
        sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 8] |= set[x & 7];
    } else {
        sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 8] &= clr[x & 7];
    }
}

/**************************************************************************/
/*!
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)

    @return     1 if the pixel is enabled, 0 if disabled
*/
/**************************************************************************/
uint8_t Adafruit_SharpMem_getPixel(uint16_t x, uint16_t y)
{
    if((x >= Adafruit_GFX_getWidth()) || (y >= Adafruit_GFX_getHeight())) return 0; // <0 test not needed, unsigned

    switch(Adafruit_GFX_getRotation()) {
        case 1:
            swap(x, y);
            x = SHARPMEM_LCDWIDTH  - 1 - x;
            break;
        case 2:
            x = SHARPMEM_LCDWIDTH  - 1 - x;
            y = SHARPMEM_LCDHEIGHT - 1 - y;
            break;
        case 3:
            swap(x, y);
            y = SHARPMEM_LCDHEIGHT - 1 - y;
            break;
    }

    return sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 8] & set[x & 7] ? 1 : 0;
}


/**************************************************************************/
/*!
    @brief Clears the screen
*/
/**************************************************************************/
void Adafruit_SharpMem_clearDisplay()
{
    memset(sharpmem_buffer, 0xff, (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8);
    // Send the clear screen command rather than doing a HW refresh (quicker)
    nrf_gpio_pin_set(m_cs_pin_number);
    sendbyte(_sharpmem_vcom | SHARPMEM_BIT_CLEAR);
    sendbyteLSB(0x00);
    TOGGLE_VCOM;
    nrf_gpio_pin_clear(m_cs_pin_number);
}


/**************************************************************************/
/*!
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void Adafruit_SharpMem_refresh(void)
{
    uint16_t i, totalbytes, currentline, oldline;
    totalbytes = (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8;

    // Send the write command
    nrf_gpio_pin_set(m_cs_pin_number);
    sendbyte(SHARPMEM_BIT_WRITECMD | _sharpmem_vcom);
    TOGGLE_VCOM;

    // Send the address for line 1
    oldline = currentline = 1;
    sendbyteLSB(currentline);

    // Send image buffer
    for (i=0; i<totalbytes; i++)
    {
    	sendbyteLSB(sharpmem_buffer[i]);
        currentline = ((i+1)/(SHARPMEM_LCDWIDTH/8)) + 1;
        if(currentline != oldline)
        {
            // Send end of line and address bytes
        	sendbyteLSB(0x00);
            if (currentline <= SHARPMEM_LCDHEIGHT)
            {
            	sendbyteLSB(currentline);
            }
            oldline = currentline;
        }
    }

    // Send another trailing 8 bits for the last line
    sendbyteLSB(0x00);
    nrf_gpio_pin_clear(m_cs_pin_number);
}

/**************************************************************************/
/*!
    @brief Turn on the screen
*/
/**************************************************************************/
void Adafruit_SharpMem_enableDisplay() {
    nrf_gpio_pin_set(m_enable_pin_number);
}

static void sendbyte(uint8_t data) {
	uint8_t i = 0;

	// LCD expects LSB first
	for (i=0; i<8; i++)
	{
		// Make sure clock starts low
		nrf_gpio_pin_clear(m_sclk_pin_number);
		if (data & 0x80)
			nrf_gpio_pin_set(m_mosi_pin_number);
		else
			nrf_gpio_pin_clear(m_mosi_pin_number);

		// Clock is active high
		nrf_gpio_pin_set(m_sclk_pin_number);
		data <<= 1;
	}
	// Make sure clock ends low
	nrf_gpio_pin_clear(m_sclk_pin_number);
}

static void sendbyteLSB(uint8_t data) {
	uint8_t i = 0;

	// LCD expects LSB first
	for (i=0; i<8; i++)
	{
		// Make sure clock starts low
		nrf_gpio_pin_clear(m_sclk_pin_number);
		if (data & 0x01)
			nrf_gpio_pin_set(m_mosi_pin_number);
		else
			nrf_gpio_pin_clear(m_mosi_pin_number);
		// Clock is active high
		nrf_gpio_pin_set(m_sclk_pin_number);
		data >>= 1;
	}
	// Make sure clock ends low
	nrf_gpio_pin_clear(m_sclk_pin_number);
}
