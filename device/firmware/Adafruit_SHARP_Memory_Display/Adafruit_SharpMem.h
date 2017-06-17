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

#ifndef ADAFRUIT_SHARP_MEM_H
#define ADAFRUIT_SHARP_MEM_H

#include "Adafruit_GFX/Adafruit_GFX.h"

// LCD Dimensions
#define SHARPMEM_LCDWIDTH       (96)
#define SHARPMEM_LCDHEIGHT      (96)

/**
 * @brief Initialize the Adafruit_SharpMem library.
 *
 * Note that this function was originally a class constructor.
 */
void Adafruit_SharpMem_init(
		uint8_t enable_pin_number,
		uint8_t cs_pin_number,
		uint8_t mosi_pin_number,
		uint8_t miso_unused_pin_number,
		uint8_t sclk_pin_number);

void Adafruit_SharpMem_begin();

void Adafruit_SharpMem_drawPixel(int16_t x, int16_t y, uint16_t color);

uint8_t Adafruit_SharpMem_getPixel(uint16_t x, uint16_t y);

void Adafruit_SharpMem_clearDisplay();

void Adafruit_SharpMem_refresh();

void Adafruit_SharpMem_enableDisplay();

void Adafruit_SharpMem_disableDisplay();

#endif // ADAFRUIT_SHARP_MEM_H
