
/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "MT_spi_master.h"

#include "project_defines.h"
#include "global.h"
#include "Adafruit_IL0373.h"
#include "FreeSansBold18pt7b.h"
#include "FreeSansBold12pt7b.h"
#include "FreeSansBold9pt7b.h"


/* Variable definition */

volatile uint8_t system_flags = 0;

WORD_ALIGNED_ATTR uint8_t tx_buff[20];
WORD_ALIGNED_ATTR uint8_t rx_buff[20];

/* Function declaration */
void MCU_to_MCU_SPI_Initialize(void);
//void MCU_to_Epaper_SPI_Initialize(void);
void My_SPI_Slave_Post_Setup_Callback(spi_slave_transaction_t *trans);
void My_SPI_Slave_Post_Transaction_Callback(spi_slave_transaction_t *trans);


extern "C" void app_main(void)
{

    uint8_t conta = 0;

    spi_slave_transaction_t tr;
    esp_err_t ret;

    //MCU_to_MCU_SPI_Initialize();
    
    Adafruit_IL0373 display((int)152, (int)152, EPD_DC_GPIO, EPD_RESET_GPIO, EPD_CS_GPIO, EPD_BUSY_GPIO, MCU_TO_EPAPER_SPI_CHANNEL, MCU_TO_EPAPER_SPI_CLK_FREQ_HZ, MCU_TO_EPAPER_SPI_MODE);
    display.begin();
    display.clearBuffer();
    display.setRotation(3);
    display.setCursor(1, 1);
    display.setTextWrap(false);
    display.setTextColor(EPD_BLACK);
    display.setFont(&FreeSansBold18pt7b);
    
    display.print("Texto1");
    display.update();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //tx_buff[0] = 0x31;
    //tx_buff[1] = 0x34;


    //Set up a transaction of 128 bytes to send/receive
    //tr.length = 6*8;
    //tr.trans_len = tr.length;
    //tr.tx_buffer = tx_buff;
    //tr.rx_buffer = rx_buff;

    while(1){

        // //if(conta < 2){

        //     memset(rx_buff, 0, 20);
        //     ret=spi_slave_transmit(MCU_TO_MCU_SPI_CHANNEL, &tr, portMAX_DELAY);
        //     if(rx_buff[0] != 0){
        //         printf("Dato: %s\n\r",rx_buff);
        //         conta++;
        //     }
        
        // //}


        //vTaskDelay(1000 / portTICK_PERIOD_MS);        
        
        //VSPI_Polling_Transfer_Bytes(tx_buff, rx_buff, 2);
        //vTaskDelay(10 / portTICK_PERIOD_MS);

        //printf("Hola papu\n");
        display.print("Texto2");
        display.update();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}



void MCU_to_MCU_SPI_Initialize(void){


    /* Configure SPI periheral */
    HSPI_Slave_Config(MCU_TO_MCU_SPI_MODE, MCU_TO_MCU_SPI_BIT_ORDER, My_SPI_Slave_Post_Setup_Callback, My_SPI_Slave_Post_Transaction_Callback);

    /* Configure and initialize the handshake pin */
    gpio_pad_select_gpio(MCU_TO_MCU_SPI_HANDSHAKE_GPIO);
    gpio_set_direction(MCU_TO_MCU_SPI_HANDSHAKE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(MCU_TO_MCU_SPI_HANDSHAKE_GPIO, 1);

}


// void MCU_to_Epaper_SPI_Initialize(void){

//     VSPI_Master_Config(MCU_TO_EPAPER_SPI_MODE, MCU_TO_EPAPER_SPI_CLK_FREQ_HZ, MCU_TO_EPAPER_SPI_BIT_ORDER);

//     /* Configure and initialize the Chip Select pin */
//     gpio_pad_select_gpio(MCU_TO_EPAPER_SPI_CS_GPIO);
//     gpio_set_direction(MCU_TO_EPAPER_SPI_CS_GPIO, GPIO_MODE_OUTPUT);
//     gpio_set_level(MCU_TO_EPAPER_SPI_CS_GPIO, 1);    

// }


//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void My_SPI_Slave_Post_Setup_Callback(spi_slave_transaction_t *trans) {
    //WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
    gpio_set_level(MCU_TO_MCU_SPI_HANDSHAKE_GPIO, 1);
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void My_SPI_Slave_Post_Transaction_Callback(spi_slave_transaction_t *trans) {
    //WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
    gpio_set_level(MCU_TO_MCU_SPI_HANDSHAKE_GPIO, 0);
}
