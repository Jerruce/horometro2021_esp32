/*
 * MT_spi_master.h
 *
 *  Created on: Jan 23, 2020
 *      Author: mtorres
 */

#ifndef MT_SPI_MASTER_H_
#define MT_SPI_MASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_err.h"
#include "esp_log.h"

/* Constants and macros */

/*
 * SPI master modes for ESP32:
 * - Mode 0: CPOL = 0, CPHA = 0
 * - Mode 1: CPOL = 0, CPHA = 1
 * - Mode 2: CPOL = 1, CPHA = 0
 * - Mode 3: CPOL = 1, CPHA = 1
 */


#define SPI_MODE_0      0
#define SPI_MODE_1      1
#define SPI_MODE_2      2
#define SPI_MODE_3      3

#define SPI_LSB_FIRST               (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST)     
#define SPI_MSB_FIRST               0

#define HSPI_CLOCK      			250000
#define VSPI_CLOCK      			250000

#define SPI_TRANSFER_CANCELLED	    0

#define HSPI_MISO_GPIO  GPIO_NUM_12 // Shared with JTAG/MicroSD
#define HSPI_MOSI_GPIO  GPIO_NUM_13 // Shared with JTAG/MicroSD
#define HSPI_SCK_GPIO   GPIO_NUM_14 // Shared with JTAG/MicroSD
#define HSPI_CS_GPIO    GPIO_NUM_15

#define VSPI_MISO_GPIO  GPIO_NUM_19 
#define VSPI_MOSI_GPIO  GPIO_NUM_23 
#define VSPI_SCK_GPIO   GPIO_NUM_18 
#define VSPI_CS_GPIO    GPIO_NUM_5

#ifdef __cplusplus
extern "C" {
#endif

void HSPI_Master_Config(uint8_t spi_mode, int sck_hz, int bit_order, spi_device_handle_t *my_dev_handle);
void VSPI_Master_Config(uint8_t spi_mode, int sck_hz, int bit_order, spi_device_handle_t *my_dev_handle);
int32_t HSPI_Polling_Transfer_Bytes(uint8_t *spi_tx_buffer, uint8_t *spi_rx_buffer, uint16_t n_bytes, spi_device_handle_t *my_dev_handle);
int32_t VSPI_Polling_Transfer_Bytes(uint8_t *spi_tx_buffer, uint8_t *spi_rx_buffer, uint16_t n_bytes, spi_device_handle_t *my_dev_handle); 

void HSPI_Slave_Config(uint8_t spi_mode, int bit_order, slave_transaction_cb_t my_post_setup_cb, slave_transaction_cb_t my_post_trans_cb);
void VSPI_Slave_Config(uint8_t spi_mode, int bit_order, slave_transaction_cb_t my_post_setup_cb, slave_transaction_cb_t my_post_trans_cb);


#ifdef __cplusplus
}
#endif

#endif /* MT_SPI_MASTER_H_ */




