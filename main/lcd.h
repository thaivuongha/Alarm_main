/*
 * lcd.h
 *
 *  Created on: Oct 13, 2019
 *      Author: TA QUOC ANH
 */

#ifndef MAIN_LCD_HHH_
#define MAIN_LCD_HHH_
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#ifdef __cplusplus
extern "C" {
#endif

void test_lcd(void);

void show_time(void) ;



#ifdef __cplusplus
}
#endif
#endif /* MAIN_LCD_HHH_ */
