/* Inclusión de archivos */

#include "MT_rtc_pcf8523.h"

rtc_date_t rtc_date;
rtc_time_t rtc_time;

/* Definición de funciones */
const char *mes_string(mes_t mes)
{
	switch (mes)
	{
	case enero:
		return "ene";
	case febrero:
		return "feb";
	case marzo:
		return "mar";
	case abril:
		return "abr";
	case mayo:
		return "may";
	case junio:
		return "jun";
	case julio:
		return "jul";
	case agosto:
		return "ago";
	case septiembre:
		return "sep";
	case octubre:
		return "oct";
	case noviembre:
		return "nov";
	case diciembre:
		return "dic";
	default:
		return "error";
	}
}

esp_err_t RTC_Init()
{
	esp_err_t err = RTC_Clear_Bit(CONTROL_3_REG, BIT_BLIE);
	err = RTC_Set_RegCtrl3(0b001, 0, 0, 1);
	return err;
}

esp_err_t RTC_Time_Set_12h(rtc_time_t time)
{

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	esp_err_t salida;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = time.hh % 10;
	hh_d = (time.hh / 10) % 10;

	mm_u = time.mm % 10;
	mm_d = (time.mm / 10) % 10;

	ss_u = time.ss % 10;
	ss_d = (time.ss / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Cargar los dígitos y meridiano al buffer de transmisión */
	i2c_tx_buffer[1] = (ss_d << 4) | ss_u;
	i2c_tx_buffer[2] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[3] = (time.mer << 5) | ((hh_d & 1) << 4) | hh_u;

	/* Transmitir los 3 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 4);

	return salida;
}

esp_err_t RTC_Time_Set_24h(rtc_time_t time)
{

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	esp_err_t salida;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = time.hh % 10;
	hh_d = (time.hh / 10) % 10;

	mm_u = time.mm % 10;
	mm_d = (time.mm / 10) % 10;

	ss_u = time.ss % 10;
	ss_d = (time.ss / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (ss_d << 4) | ss_u;
	i2c_tx_buffer[2] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[3] = (hh_d << 4) | hh_u;

	/* Transmitir los 3 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 4);

	return salida;
}

esp_err_t RTC_Time_Read_12h(rtc_time_t *time)
{

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	esp_err_t salida;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 1);
	/* Leer los registros */
	salida = i2c_master_read_slave(I2C_MASTER_NUM, PCF8523_SLAR, i2c_rx_buffer, 3);

	/* Cargar los valores de hora, minuto, segundo y meridiano */
	ss_u = i2c_rx_buffer[0] & 0x0F;
	ss_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	time->ss = (10 * ss_d) + ss_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 0x0F;
	time->mm = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[2] & 0x0F;
	hh_d = (i2c_rx_buffer[2] >> 4) & 1;
	time->hh = (10 * hh_d) + hh_u;

	time->mer = (i2c_rx_buffer[2] >> 5) & 1;

	return salida;
}

esp_err_t RTC_Time_Read_24h(rtc_time_t *time)
{

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	esp_err_t salida;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 1);
	/* Leer los registros */

	salida = i2c_master_read_slave(I2C_MASTER_NUM, PCF8523_SLAR, i2c_rx_buffer, 3);

	/* Cargar los valores de hora, minuto, segundo y meridiano */
	ss_u = i2c_rx_buffer[0] & 0x0F;
	ss_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	time->ss = (10 * ss_d) + ss_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 0x0F;
	time->mm = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[2] & 0x0F;
	hh_d = (i2c_rx_buffer[2] >> 4) & 3;
	time->hh = (10 * hh_d) + hh_u;

	return salida;
}

esp_err_t RTC_Date_Set(rtc_date_t date)
{

	esp_err_t salida = 0;

	/* Extraer los dígitos del día, mes y año */

	uint8_t yy_d, yy_u, mm_d, mm_u, dd_d, dd_u;

	date.yy -= 2000;
	yy_u = date.yy % 10;
	yy_d = (date.yy / 10) % 10;

	mm_u = date.mm % 10;
	mm_d = ((date.mm / 10) % 10);

	dd_u = date.dd % 10;
	dd_d = ((date.dd / 10) % 10) & 0x03;

	/* Cargar dirección del registro de días en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (dd_d << 4) | dd_u;
	/* Transmitir los 2 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 2);

	/* Cargar dirección del registro de meses en el buffer de transmisión */
	i2c_tx_buffer[0] = MONTHS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[2] = (yy_d << 4) | yy_u;
	/* Transmitir los 2 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 3);

	return salida;
}

esp_err_t RTC_Date_Read(rtc_date_t *date)
{

	esp_err_t salida = 0;
	uint8_t yy_d, yy_u, mm_d, mm_u, dd_d, dd_u;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Escribir la dirección del primer registro a leer (DIAS) */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 1);
	/* Leer el registro de DIAS */
	salida = i2c_master_read_slave(I2C_MASTER_NUM, PCF8523_SLAR, i2c_rx_buffer, 1);

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MONTHS_REG;
	/* Escribir la dirección del siguiente registro a leer (MESES) */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 1);
	/* Leer el registro de MESES y el registro AÑOS */
	salida = i2c_master_read_slave(I2C_MASTER_NUM, PCF8523_SLAR, i2c_rx_buffer + 1, 2);

	/* Cargar los valores de día , mes y año*/
	dd_u = i2c_rx_buffer[0] & 0x0F;
	dd_d = (i2c_rx_buffer[0] >> 4) & 0x03;
	date->dd = (10 * dd_d) + dd_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 1;
	date->mm = (10 * mm_d) + mm_u;

	yy_u = i2c_rx_buffer[2] & 0x0F;
	yy_d = (i2c_rx_buffer[2] >> 4) & 0x0F;
	date->yy = (10 * yy_d) + yy_u + 2000; //HOUR

	return salida;
}

esp_err_t RTC_Set_RegCtrl3(uint8_t PM, uint8_t BSF, uint8_t BSIE, uint8_t BLIE)
{
	esp_err_t salida;

	/* Cargar dirección del registro de días en el buffer de transmisión */
	i2c_tx_buffer[0] = CONTROL_3_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (PM << 5) | (BSF << 3) | (BSIE << 1) | (BLIE << 0);
	/* Transmitir los 2 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 2);
	return salida;
}

esp_err_t RTC_Clear_Bit(uint8_t reg, uint8_t bit)
{
	esp_err_t salida;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = reg;
	/* Escribir la dirección del primer registro a leer (DIAS) */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 1);
	/* Leer el registro de DIAS */
	salida = i2c_master_read_slave(I2C_MASTER_NUM, PCF8523_SLAR, i2c_rx_buffer, 1);

	/* Cargar dirección del registro de días en el buffer de transmisión */
	i2c_tx_buffer[0] = reg;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = i2c_rx_buffer[0] & ~(1 << bit);
	/* Transmitir los 2 bytes al RTC */
	salida = i2c_master_write_slave(I2C_MASTER_NUM, PCF8523_SLAW, i2c_tx_buffer, 2);
	return salida;
}