/*
 * MT_cog_lcd.h
 *
 *  Created on: Jan 23, 2020
 *      Author: mtorres
 */

#ifndef MT_COG_LCD_H_
#define MT_COG_LCD_H_

/* Inclusión de archivos */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "MT_cog_lcd.h"
#include "esp_log.h"
#include "esp_err.h"

/* Definición de pines */
#define COG_LCD_RST GPIO_NUM_23
#define COG_LCD_RS GPIO_NUM_18
#define COG_LCD_CSB GPIO_NUM_17
#define COG_LCD_LED GPIO_NUM_16

/* clear display and return home */
#define LCD_CLEAR 0x01 /* DB0: clear display                  */
#define LCD_HOME 0x02  /* DB1: return to home position        */

/* set entry mode: display shift on/off, dec/inc cursor move direction */
#define LCD_ENTRY_DEC 0x04       /* display shift off, dec cursor move dir */
#define LCD_ENTRY_DEC_SHIFT 0x05 /* display shift on,  dec cursor move dir */
#define LCD_ENTRY_INC 0x06       /* display shift off, inc cursor move dir */
#define LCD_ENTRY_INC_SHIFT 0x07 /* display shift on,  inc cursor move dir */

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF 0x08             /* display off                            */
#define LCD_DISP_ON 0x0C              /* display on, cursor off                 */
#define LCD_DISP_ON_BLINK 0x0D        /* display on, cursor off, blink char     */
#define LCD_DISP_ON_CURSOR 0x0E       /* display on, cursor on                  */
#define LCD_DISP_ON_CURSOR_BLINK 0x0F /* display on, cursor on, blink char      */

/* move cursor/shift display */
#define LCD_MOVE_CURSOR_LEFT 0x10  /* move cursor left  (decrement)          */
#define LCD_MOVE_CURSOR_RIGHT 0x14 /* move cursor right (increment)          */
#define LCD_MOVE_DISP_LEFT 0x18    /* shift display left                     */
#define LCD_MOVE_DISP_RIGHT 0x1C   /* shift display right                    */

#define LCD_LINES 2          /* number of visible lines of the display */
#define LCD_DISP_LENGTH 16   /* visibles characters per line of the display */
#define LCD_LINE_LENGTH 40   /* internal line length of the display    */
#define LCD_START_LINE1 0x00 /* DDRAM address of first char of line 1 */
#define LCD_START_LINE2 0x40 /* DDRAM address of first char of line 2 */
#define LCD_START_LINE3 0x14 /* DDRAM address of first char of line 3 */
#define LCD_START_LINE4 0x54 /* DDRAM address of first char of line 4 */

typedef enum
{
    lcd_led_off,
    lcd_led_on
} lcd_led_power_t;

/*
 * SPI master modes for ESP32:
 * - Mode 0: CPOL = 0, CPHA = 0
 * - Mode 1: CPOL = 0, CPHA = 1
 * - Mode 2: CPOL = 1, CPHA = 0
 * - Mode 3: CPOL = 1, CPHA = 1
 */
#define SPI_MODE 3 // Default SPI mode 0

#define SPI_CHANNEL HSPI_HOST
#define SPI_CLOCK 250000
#define SPI_TRANSFER_CANCELLED 0

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t spi_tx_buffer[];
extern uint8_t spi_rx_buffer[];
extern spi_device_handle_t lcd_spi_handle;

/* Declaración de funciones (prototipos) */
void COG_LCD_Lcd_Led_Power(lcd_led_power_t);
esp_err_t COG_LCD_Init(void);
void COG_LCD_Tx_Command(uint8_t command);
void COG_LCD_Put_Char(char byte);
void COG_LCD_Put_String(char *str);
void COG_LCD_Goto_XY(uint8_t pos_x, uint8_t pos_y);

#ifdef __cplusplus
}
#endif

#endif /* MT_COG_LCD_H_ */
