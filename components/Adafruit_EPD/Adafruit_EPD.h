#ifndef _Adafruit_EPD_H_
#define _Adafruit_EPD_H_

//#define EPD_DEBUG

#define RAMBUFSIZE 64 ///< size of the ram buffer

//typedef volatile uint32_t PortReg; ///< a port register for fast access
//typedef uint32_t PortMask;         ///< a port register mask for your pin

#include "Adafruit_GFX.h"
#include "digital_epd.h"
#include <stdlib.h>
#include <stdint.h>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_err.h"
/**************************************************************************/
/*!
    @brief available EPD colors
*/
/**************************************************************************/
enum
{
  EPD_BLACK,   ///< black color
  EPD_WHITE,   ///< white color
  EPD_INVERSE, ///< invert color
  EPD_RED,     ///< red color
  EPD_GRAY,    ///< gray color ('red' on grayscale)
  EPD_DARK,    ///< darker color
  EPD_LIGHT,   ///< lighter color
};

#define EPD_swap(a, b) \
  {                    \
    int16_t t = a;     \
    a = b;             \
    b = t;             \
  } ///< simple swap function

/**************************************************************************/
/*!
    @brief  Class for interfacing with Adafruit EPD display breakouts.
*/
/**************************************************************************/

class Adafruit_EPD : public Adafruit_GFX
{
public:
  Adafruit_EPD(int width, int height, gpio_num_t DC, gpio_num_t RST, gpio_num_t CS,
               gpio_num_t BUSY,
               spi_host_device_t SPI_CHANNEL, uint32_t SPI_CLOCK, uint8_t SPI_MODE);
  ~Adafruit_EPD();

  void begin(bool reset = true);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clearBuffer();
  void clearDisplay();
  void setBlackBuffer(int8_t index, bool inverted);
  void setColorBuffer(int8_t index, bool inverted);
  
  void display(void);

  //partial update

protected:
  /**************************************************************************/
  /*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
    displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
    command
  */
  /**************************************************************************/
  virtual uint8_t writeRAMCommand(uint8_t index) = 0;

  /**************************************************************************/
  /*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
  */
  /**************************************************************************/
  virtual void setRAMAddress(uint16_t x, uint16_t y) = 0;

  /**************************************************************************/

  virtual void busy_wait(void) = 0;
  /*!
    @brief start up the display
  */
  /**************************************************************************/
  virtual void powerUp(void) = 0;

  /**************************************************************************/
  /*!
    @brief signal the display to update
  */
  /**************************************************************************/
  virtual void update(void) = 0;

  /**************************************************************************/
  /*!
    @brief wind down the display
  */
  /**************************************************************************/
  virtual void powerDown(void) = 0;
  void hardwareReset(void);

  spi_device_handle_t device_handle;
  uint32_t SPI_CLOCK;
  uint8_t SPI_MODE;
  spi_host_device_t SPI_CHANNEL;

  gpio_num_t _sid_pin,          ///< sid pin
      _sclk_pin,                ///< serial clock pin
      _dc_pin,                  ///< data/command pin
      _reset_pin,               ///< reset pin
      _cs_pin,                  ///< chip select pin
      _busy_pin;                ///< busy pin
  static bool _isInTransaction; ///< true if SPI bus is in trasnfer state
  bool singleByteTxns;          ///< if true CS will go high after every data byte
                                ///< transferred

  const uint8_t *_epd_init_code = NULL;
  const uint8_t *_epd_lut_code = NULL;
  const uint8_t *_epd_partial_init_code = NULL;
  const uint8_t *_epd_partial_lut_code = NULL;

  bool blackInverted;    ///< is black channel inverted
  bool colorInverted;    ///< is red channel inverted
  uint16_t buffer1_size; ///< size of the primary buffer
  uint16_t buffer2_size; ///< size of the secondary buffer
  uint8_t *buffer1;      ///< the pointer to the primary buffer if using on-chip ram
  uint8_t
      *buffer2; ///< the pointer to the secondary buffer if using on-chip ram
  uint8_t
      *color_buffer; ///< the pointer to the color buffer if using on-chip ram
  uint8_t
      *black_buffer;         ///< the pointer to the black buffer if using on-chip ram
  uint16_t buffer1_addr;     ///< The SRAM address offsets for the primary buffer
  uint16_t buffer2_addr;     ///< The SRAM address offsets for the secondary buffer
  uint16_t colorbuffer_addr; ///< The SRAM address offsets for the color buffer
  uint16_t blackbuffer_addr; ///< The SRAM address offsets for the black buffer

  void EPD_commandList(const uint8_t *init_code);
  void EPD_command(uint8_t c, const uint8_t *buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end = true);
  void EPD_data(const uint8_t *buf, uint16_t len);
  void EPD_data(uint8_t data);

  uint8_t SPItransfer(uint8_t c);

  void csLow();
  void csHigh();
  void dcHigh();
  void dcLow();
};

#include "Adafruit_IL0373.h"

#endif /* _Adafruit_EPD_H_ */
