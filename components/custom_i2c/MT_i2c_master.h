/*
 * MT_i2c_master.h
 *
 *  Created on: Jan 21, 2020
 *      Author: mtorres
 */

#ifndef MT_I2C_MASTER_H_
#define MT_I2C_MASTER_H_

/* Inclusión de archivos*/
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

/* Contantes y macros */

/* Parámetros de configuración para la comunicación I2C */
#define I2C_MASTER_SCL_IO 						GPIO_NUM_22    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 						GPIO_NUM_21    /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 						50000UL        /*!< I2C master clock frequency */

/* Parámetros de configuración propios de los drivers I2C */
#define I2C_MASTER_NUM 							I2C_NUM_1 		/*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE 				0               /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 				0               /*!< I2C master doesn't need buffer */

/* Acciones principales ralizadas por el driver */
#define WRITE_BIT	 					I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT 						I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 					0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 					0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 						0x0                             /*!< I2C ack value */
#define NACK_VAL 						0x1                            /*!< I2C nack value */


#define I2C_RX_BUFFER_SIZE	20
#define I2C_TX_BUFFER_SIZE	20

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t i2c_tx_buffer[I2C_TX_BUFFER_SIZE];
extern uint8_t i2c_rx_buffer[I2C_RX_BUFFER_SIZE];

esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t slar, uint8_t *data_rd, size_t size);
esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t slaw, uint8_t *data_wr, size_t size);
esp_err_t i2c_master_init(void);

#ifdef __cplusplus
}
#endif

#endif /* MT_I2C_MASTER_H_ */
