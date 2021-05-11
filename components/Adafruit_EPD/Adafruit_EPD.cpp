
#include "Adafruit_EPD.h"
#include "MT_spi_master.h"
#define TAG "EPD"

static int32_t spi_dma_transfer_byte(uint8_t *data, spi_device_handle_t *device_handle)
{
  esp_err_t trans_result = ESP_OK;
  spi_transaction_t trans_t;

  trans_t.rx_buffer = NULL;
  trans_t.tx_buffer = data;

  trans_t.rxlength = 0;
  trans_t.length = 8;
  trans_t.flags = 0;
  trans_t.cmd = 0;
  trans_t.addr = 0;
  trans_t.user = NULL;

  // Perform transaction
  trans_result = spi_device_transmit(*device_handle, &trans_t);
  if (ESP_OK != trans_result)
  {
    return 0;
  }

  return 1;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
    @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, gpio_num_t DC, gpio_num_t RST,
                           gpio_num_t CS, gpio_num_t BUSY,
                           spi_host_device_t SPI_CHANNEL, uint32_t SPI_CLOCK, uint8_t SPI_MODE)
    : Adafruit_GFX(width, height)
{
  this->SPI_CHANNEL = SPI_CHANNEL;
  this->SPI_CLOCK = SPI_CLOCK;
  this->SPI_MODE = SPI_MODE;

  _cs_pin = CS;
  _reset_pin = RST;
  _dc_pin = DC;
  _busy_pin = BUSY;

  singleByteTxns = false;
  buffer1_size = buffer2_size = 0;
  buffer1_addr = buffer2_addr = 0;
  colorbuffer_addr = blackbuffer_addr = 0;
  buffer1 = buffer2 = color_buffer = black_buffer = NULL;
}

/**************************************************************************/
/*!
    @brief default destructor
*/
/**************************************************************************/
Adafruit_EPD::~Adafruit_EPD()
{
  if (buffer1 != NULL)
  {
    free(buffer1);
    buffer1 = NULL;
  }
  if (buffer2 != NULL)
  {
    free(buffer2);
    buffer2 = NULL;
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_EPD::begin(bool reset)
{
  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, false); // red defaults to not inverted

  // set pin directions
  pinMode(_dc_pin, OUTPUT);
  pinMode(_cs_pin, OUTPUT);

  csHigh();

  // // Configuration for the SPI master interface
  // spi_device_interface_config_t devcfg;
  // devcfg.command_bits = 0;
  // devcfg.address_bits = 0;
  // devcfg.dummy_bits = 0;
  // devcfg.duty_cycle_pos = 128;
  // devcfg.cs_ena_pretrans = 0;
  // devcfg.cs_ena_posttrans = 0;
  // devcfg.clock_speed_hz = SPI_CLOCK;
  // devcfg.mode = SPI_MODE;
  // devcfg.spics_io_num = -1; // El CS se manipulará manualmente
  // devcfg.queue_size = 1;
  // devcfg.flags = 0;
  // devcfg.pre_cb = NULL;
  // devcfg.post_cb = NULL;

  // // add device to spi bus
  // esp_err_t err = spi_bus_add_device(SPI_CHANNEL, &devcfg, &device_handle);
  // if (err != ESP_OK)
  // {
  //   ESP_LOGE(TAG, "Falla en add device");
  //   return;
  // }

  if(SPI_CHANNEL == HSPI_HOST)
  {
    HSPI_Master_Config(SPI_MODE, SPI_CLOCK, SPI_MSB_FIRST, &device_handle);
  }
  else if(SPI_CHANNEL == VSPI_HOST)
  {
    VSPI_Master_Config(SPI_MODE, SPI_CLOCK, SPI_MSB_FIRST, &device_handle);
  }
  else
  {
    ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
    return;
  }


  if (reset)
  {
    hardwareReset();
  }

  if (_busy_pin >= 0)
  {
    pinMode(_busy_pin, INPUT);
  }
}

/**************************************************************************/
/*!
    @brief reset Perform a hardware reset
*/
/**************************************************************************/
void Adafruit_EPD::hardwareReset(void)
{
  if (_reset_pin >= 0)
  {
    // Setup reset pin direction
    pinMode(_reset_pin, OUTPUT);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    digitalWrite(_reset_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    //delay(10);
    // bring reset low
    digitalWrite(_reset_pin, LOW);
    // wait 10ms
    vTaskDelay(pdMS_TO_TICKS(10));
    //delay(10);
    // bring out of reset
    digitalWrite(_reset_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    //delay(10);
  }
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
        @param x the x axis position
        @param y the y axis position
        @param color the color of the pixel
*/
/**************************************************************************/
void Adafruit_EPD::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  uint8_t *pBuf;

  // deal with non-8-bit heights
  uint16_t _HEIGHT = HEIGHT;
  if (_HEIGHT % 8 != 0)
  {
    _HEIGHT += 8 - (_HEIGHT % 8);
  }

  // check rotation, move pixel around if necessary
  switch (getRotation())
  {
  case 1:
    EPD_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = _HEIGHT - y - 1;
    break;
  case 3:
    EPD_swap(x, y);
    y = _HEIGHT - y - 1;
    break;
  }
  uint16_t addr = ((uint32_t)(WIDTH - 1 - x) * (uint32_t)_HEIGHT + y) / 8;
  uint8_t c;

  if ((color == EPD_RED) || (color == EPD_GRAY))
  {
    pBuf = color_buffer + addr;
  }
  else
  {
    pBuf = black_buffer + addr;
  }

  if (((color == EPD_RED || color == EPD_GRAY) && colorInverted) ||
      ((color == EPD_BLACK) && blackInverted))
  {
    *pBuf &= ~(1 << (7 - y % 8));
  }
  else if (((color == EPD_RED || color == EPD_GRAY) && !colorInverted) ||
           ((color == EPD_BLACK) && !blackInverted))
  {
    *pBuf |= (1 << (7 - y % 8));
  }
  else if (color == EPD_INVERSE)
  {
    *pBuf ^= (1 << (7 - y % 8));
  }
}

/**************************************************************************/
/*!
    @brief Transfer the data stored in the buffer(s) to the display
*/
/**************************************************************************/
/*
void Adafruit_EPD::partial_display(const unsigned char *imagedata, int x, int y, int width, int height)
{
  powerUp();

  EPD_command(0x91); //partial in
  EPD_command(0x90); // partial window
  EPD_data(1 << 3);
  EPD_data(5 << 3);
  //EPD_data(x & 0xf8); // x should be the multiple of 8, the last 3 bit will always be ignored
  //EPD_data(((x & 0xf8) + width - 1) | 0x07);
  EPD_data(0);
  EPD_data(1);
  EPD_data(0);
  EPD_data(3);
  //EPD_data(y >> 8);
  //EPD_data(y & 0xff);
  //EPD_data((y + height - 1) >> 8);
  //EPD_data((y + height - 1) & 0xff);
  EPD_data(0x01); // Gates scan both inside and outside of the partial window. (default)
  vTaskDelay(pdMS_TO_TICKS(2));
  EPD_command(EPD_RAM_BW);
  //EPD_command(EPD_RAM_BW, false);

  if (imagedata != NULL)
  {
    for (int i = 0; i < width / 8 * height; i++)
    {
      EPD_data(imagedata[i]);
    }
  }
  else
  {
    for (int i = 0; i < width / 8 * height; i++)
    {
      EPD_data(0x00);
    }
  }

  vTaskDelay(pdMS_TO_TICKS(2));
  EPD_command(0x92); //partial out

  update();
  powerDown();
}
*/
void Adafruit_EPD::display(void)
{
  uint8_t c;
  uint8_t temp_rx_buffer[152];

  powerUp();

  // Set X & Y ram counters
  setRAMAddress(0, 0);

  // write image
  writeRAMCommand(0);
  dcHigh();

  //for (uint16_t i = 0; i < buffer1_size; i++)
  //{
  //  SPItransfer(buffer1[i]);
  //}
  if(SPI_CHANNEL == HSPI_HOST)
  {
    csLow();
    HSPI_Polling_Transfer_Bytes(buffer1, temp_rx_buffer, buffer1_size, &device_handle);
    csHigh();
  }
  else if(SPI_CHANNEL == VSPI_HOST)
  {
    csLow();
    VSPI_Polling_Transfer_Bytes(buffer1, temp_rx_buffer, buffer1_size, &device_handle);
    csHigh();  
  }
  else
  {
    ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
  }


  csHigh();

  if (buffer2_size == 0)
  {
    update();
    return;
  }

  // oh there's another buffer eh?
  vTaskDelay(pdMS_TO_TICKS(2));
  //delay(2);

  // Set X & Y ram counters
  setRAMAddress(0, 0);

  writeRAMCommand(1);
  dcHigh();

  // for (uint16_t i = 0; i < buffer2_size; i++)
  // {
  //   SPItransfer(buffer2[i]);
  // }

  if(SPI_CHANNEL == HSPI_HOST)
  {
    csLow();
    HSPI_Polling_Transfer_Bytes(buffer2, temp_rx_buffer, buffer2_size, &device_handle);
    csHigh();
  }
  else if(SPI_CHANNEL == VSPI_HOST)
  {
    csLow();
    VSPI_Polling_Transfer_Bytes(buffer2, temp_rx_buffer, buffer2_size, &device_handle);
    csHigh();  
  }
  else
  {
    ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
  }


  csHigh();

  update();

  powerDown(); 
}

/**************************************************************************/
/*!
    @brief Determine whether the black pixel data is the first or second buffer
    @param index 0 or 1, for primary or secondary value
    @param inverted Whether to invert the logical value
*/
/**************************************************************************/
void Adafruit_EPD::setBlackBuffer(int8_t index, bool inverted)
{
  if (index == 0)
  {
    black_buffer = buffer1;
  }
  if (index == 1)
  {
    black_buffer = buffer2;
  }
  blackInverted = inverted;
}

/**************************************************************************/
/*!
    @brief Determine whether the color pixel data is the first or second buffer
    @param index 0 or 1, for primary or secondary value
    @param inverted Whether to invert the logical value
*/
/**************************************************************************/
void Adafruit_EPD::setColorBuffer(int8_t index, bool inverted)
{
  if (index == 0)
  {
    color_buffer = buffer1;
  }
  if (index == 1)
  {
    color_buffer = buffer2;
  }
  colorInverted = inverted;
}

/**************************************************************************/
/*!
    @brief clear all data buffers
*/
/**************************************************************************/
void Adafruit_EPD::clearBuffer()
{
  if (buffer1)
  {
    if (blackInverted)
    {
      memset(buffer1, 0xFF, buffer1_size);
    }
    else
    {
      memset(buffer1, 0x00, buffer1_size);
    }
  }
  if (buffer2)
  {
    if (colorInverted)
    {
      memset(buffer2, 0xFF, buffer2_size);
      //memset(buffer2, 0x00, buffer2_size);
    }
    else
    {
      memset(buffer2, 0x00, buffer2_size);
      //memset(buffer2, 0xff, buffer2_size);
    }
  }
}

/**************************************************************************/
/*!
    @brief clear the display twice to remove any spooky ghost images
*/
/**************************************************************************/
void Adafruit_EPD::clearDisplay()
{
  clearBuffer();
  display();
  vTaskDelay(pdMS_TO_TICKS(100));
  //delay(100);
  display();
}

void Adafruit_EPD::EPD_commandList(const uint8_t *init_code)
{
  uint8_t buf[64];

  while (init_code[0] != 0xFE)
  {
    char cmd = init_code[0];
    init_code++;
    int num_args = init_code[0];
    init_code++;
    if (cmd == 0xFF)
    {
      busy_wait();
      vTaskDelay(pdMS_TO_TICKS(num_args));
      continue;
    }
    if (num_args > sizeof(buf))
    {
      //Serial.println("ERROR - buf not large enough!");
      while (1)
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    for (int i = 0; i < num_args; i++)
    {
      buf[i] = init_code[0];
      init_code++;
    }
    EPD_command(cmd, buf, num_args);
  }
}

/**************************************************************************/
/*!
    @brief send an EPD command followed by data
    @param c the command to send
    @param buf the buffer of data to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_command(uint8_t c, const uint8_t *buf, uint16_t len)
{
  EPD_command(c, false);
  EPD_data(buf, len);
}

/**************************************************************************/
/*!
    @brief send an EPD command with no data
    @param c the command to send
    @param end if true the cs pin will be pulled high following the transaction.
   If false the cs pin will remain low.
    @returns the data byte read over the SPI bus
*/
/**************************************************************************/
uint8_t Adafruit_EPD::EPD_command(uint8_t c, bool end)
{

  uint8_t temp_tx_buffer;
  uint8_t temp_rx_buffer;
  uint8_t data = 0;

  // SPI
  csHigh();
  dcLow();
  csLow();

  //data = SPItransfer(c);
  temp_tx_buffer = c;

  if(SPI_CHANNEL == HSPI_HOST)
  {
    HSPI_Polling_Transfer_Bytes(&temp_tx_buffer, &temp_rx_buffer, 1, &device_handle);
    data = temp_rx_buffer;
  }
  else if(SPI_CHANNEL == VSPI_HOST)
  {
    VSPI_Polling_Transfer_Bytes(&temp_tx_buffer, &temp_rx_buffer, 1, &device_handle);
    data = temp_rx_buffer;  
  }
  else
  {
    ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
  }

#ifdef EPD_DEBUG
  Serial.print("\nCommand: 0x");
  Serial.print(c, HEX);
  Serial.print(" - ");
#endif

  if (end)
  {
    csHigh();
  }

  return data;
}

/**************************************************************************/
/*!
    @brief send data to the display
    @param buf the data buffer to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_data(const uint8_t *buf, uint16_t len)
{

  uint8_t temp_rx_buffer;

  // SPI
  dcHigh();

#ifdef EPD_DEBUG
  Serial.print("Data: ");
#endif

  csLow();

  for (uint16_t i = 0; i < len; i++)
  {
    //SPItransfer(buf[i]);

    if(SPI_CHANNEL == HSPI_HOST)
    {
      HSPI_Polling_Transfer_Bytes((uint8_t *)&buf[i], &temp_rx_buffer, 1, &device_handle);
    }
    else if(SPI_CHANNEL == VSPI_HOST)
    {
      VSPI_Polling_Transfer_Bytes((uint8_t *)&buf[i], &temp_rx_buffer, 1, &device_handle); 
    }
    else
    {
      ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
    }

#ifdef EPD_DEBUG
    Serial.print("0x");
    Serial.print(buf[i], HEX);
    Serial.print(", ");
#endif
  }
#ifdef EPD_DEBUG
  Serial.println();
#endif
  csHigh();
}

/**************************************************************************/
/*!
    @brief send data to the display
    @param data the data byte to send
*/
/**************************************************************************/
void Adafruit_EPD::EPD_data(uint8_t data)
{

  uint8_t temp_rx_buffer;

  // SPI
  csHigh();
  dcHigh();
  csLow();

#ifdef DEBUG
  Serial.print("Data: ");
  Serial.print("0x");
  Serial.println(data, HEX);
#endif
  //SPItransfer(data);

  if(SPI_CHANNEL == HSPI_HOST)
  {
    HSPI_Polling_Transfer_Bytes(&data, &temp_rx_buffer, 1, &device_handle);
  }
  else if(SPI_CHANNEL == VSPI_HOST)
  {
    VSPI_Polling_Transfer_Bytes(&data, &temp_rx_buffer, 1, &device_handle); 
  }
  else
  {
    ESP_LOGE(TAG, "The chosen SPI channel is not allowed!");
  }  

  csHigh();
}

/**************************************************************************/
/*!
    @brief transfer a single byte over SPI.
    @param d the data to send
    @returns the data byte read
*/
/**************************************************************************/
uint8_t Adafruit_EPD::SPItransfer(uint8_t d)
{
  // Serial.print("-> 0x"); Serial.println((byte)d, HEX);

  if (singleByteTxns)
  {
    uint8_t b;
    csLow();
    b = spi_dma_transfer_byte(&d, &device_handle);
    //b = _spi->transfer(d);
    csHigh();
    return b;
  }
  else
    return spi_dma_transfer_byte(&d, &device_handle);
  //return _spi->transfer(d);
}

/**************************************************************************/
/*!
    @brief set chip select pin high
*/
/**************************************************************************/
void Adafruit_EPD::csHigh()
{
  digitalWrite(_cs_pin, HIGH);
}

/**************************************************************************/
/*!
    @brief set chip select pin low
*/
/**************************************************************************/
void Adafruit_EPD::csLow()
{

  digitalWrite(_cs_pin, LOW);
}

/**************************************************************************/
/*!
    @brief set data/command pin high
*/
/**************************************************************************/
void Adafruit_EPD::dcHigh()
{
  digitalWrite(_dc_pin, HIGH);
}

/**************************************************************************/
/*!
    @brief set data/command pin low
*/
/**************************************************************************/
void Adafruit_EPD::dcLow()
{
  digitalWrite(_dc_pin, LOW);
}
