/*
 * MT_rtc_pcf8523.h
 *
 *  Created on: Jan 21, 2020
 *      Author: mtorres
 */

#ifndef MT_RTC_PCF8523_H_
#define MT_RTC_PCF8523_H_

/* Inclusión de archivos */
#include <stdint.h>
#include "esp_err.h"
#include "MT_i2c_master.h"

/* Dirección del dispositivo */
#define PCF8523_SLAW 0xD0
#define PCF8523_SLAR 0xD1

/* Direcciones de los registros */
#define CONTROL_1_REG 0x00
#define CONTROL_2_REG 0x01
#define CONTROL_3_REG 0x02
#define SECONDS_REG 0x03
#define MINUTES_REG 0x04
#define HOURS_REG 0x05
#define DAYS_REG 0x06
#define WEEKDAYS_REG 0x07
#define MONTHS_REG 0x08
#define YEARS_REG 0x09
#define MINUTE_ALARM_REG 0x0A
#define HOUR_ALARM_REG 0x0B
#define DAY_ALARM_REG 0x0C
#define WEEKDAY_ALARM_REG 0x0D
#define OFFSET_REG 0x0E
#define TMR_CLKOUT_CTRL_REG 0x0F
#define TMR_A_FREQ_CTRL_REG 0x10
#define TMR_A_REG 0x11
#define TMR_B_FREQ_CTRL_REG 0x12
#define TMR_B_REG 0x13

/* Bits importantes */
#define BIT_CAP_SEL 7
#define BIT_T 6
#define BIT_STOP 5
#define BIT_SR 4
#define BIT_12_24 3
#define BIT_SIE 2
#define BIT_AIE 1
#define BIT_CIE 0

#define BIT_WTAF 7
#define BIT_CTAF 6
#define BIT_CTBF 5
#define BIT_SF 4
#define BIT_AF 3
#define BIT_WTAIE 2
#define BIT_CTAIE 1
#define BIT_CTBIE 0

#define BIT_PM2 7
#define BIT_PM1 6
#define BIT_PM0 5
#define BIT_BSF 3
#define BIT_BLF 2
#define BIT_BSIE 1
#define BIT_BLIE 0

#define BIT_COF2 5
#define BIT_COF1 4
#define BIT_COF0 3

/* Comando reset */
#define RESET_CMD

/* Posiciones en el buffer de fecha */
#define DAY 0
#define MONTH 1
#define YEAR 2

/* Posiciones en el buffer de hora */
#define SECOND 0
#define MINUTE 1
#define HOUR 2
#define MERIDIEM 3

/* Definición de tipos */
typedef enum
{
	enero = 1,
	febrero,
	marzo,
	abril,
	mayo,
	junio,
	julio,
	agosto,
	septiembre,
	octubre,
	noviembre,
	diciembre
} mes_t;

typedef enum
{
	AM = 0,
	PM
} meridiem_t;

typedef struct
{
	uint8_t dd;
	mes_t mm;
	uint16_t yy;
} rtc_date_t;

typedef struct
{
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	meridiem_t mer;
} rtc_time_t;

#ifdef __cplusplus
extern "C"
{
#endif

	/* Declaración de funciones (Prototipos) */
	const char *mes_string(mes_t mes);
	esp_err_t RTC_Init(void);
	esp_err_t RTC_Time_Set_12h(rtc_time_t time);
	esp_err_t RTC_Time_Set_24h(rtc_time_t time);
	esp_err_t RTC_Time_Read_12h(rtc_time_t *time);
	esp_err_t RTC_Time_Read_24h(rtc_time_t *time);
	esp_err_t RTC_Date_Set(rtc_date_t date);
	esp_err_t RTC_Date_Read(rtc_date_t *date);
	esp_err_t RTC_Set_RegCtrl3(uint8_t PM, uint8_t BSF, uint8_t BSIE, uint8_t BLIE);
	esp_err_t RTC_Clear_Bit(uint8_t reg, uint8_t bit);
	//hour 24h por defecto

	extern rtc_date_t rtc_date;
	extern rtc_time_t rtc_time;

#ifdef __cplusplus
}
#endif

#endif /* MT_RTC_PCF8523_H_ */
