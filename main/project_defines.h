
#ifndef PROJECT_DEFINES_H_
#define PROJECT_DEFINES_H_

/*****************************************************
                Pin definition 
******************************************************/


// Pins for SPI
#define MCU_TO_MCU_SPI_HANDSHAKE_GPIO   GPIO_NUM_27
#define MCU_TO_MCU_SPI_CS_GPIO          GPIO_NUM_15
#define MCU_TO_EPAPER_SPI_CS_GPIO       GPIO_NUM_5

// Pins for Epaper display
#define EPD_DC_GPIO                     GPIO_NUM_4
#define EPD_RESET_GPIO                  GPIO_NUM_17
#define EPD_CS_GPIO                     MCU_TO_EPAPER_SPI_CS_GPIO
#define EPD_BUSY_GPIO                   GPIO_NUM_16

// Pins for Epaper display
#define SD_CARD_CS_GPIO                 GPIO_NUM_26
#define SD_CARD_CD_GPIO                 GPIO_NUM_25


/*****************************************************
                 Contansts and macros 
******************************************************/

// Defines for SPI communication

#define MCU_TO_MCU_SPI_CHANNEL          HSPI_HOST   
#define MCU_TO_MCU_SPI_MODE             SPI_MODE_0
#define MCU_TO_MCU_SPI_CLK_FREQ_HZ      10000
#define MCU_TO_MCU_SPI_BIT_ORDER        SPI_MSB_FIRST

#define MCU_TO_EPAPER_SPI_CHANNEL       VSPI_HOST
#define MCU_TO_EPAPER_SPI_MODE          SPI_MODE_0
#define MCU_TO_EPAPER_SPI_CLK_FREQ_HZ   4000000UL//100000
#define MCU_TO_EPAPER_SPI_BIT_ORDER     SPI_MSB_FIRST

// System flags

#endif