/* Inclusión de archivos */
#include "MT_cog_lcd.h"

#define TAG "MT LCD"

/* Variables globales */
uint8_t spi_rx_buffer[SPI_MAX_DMA_LEN] = {};
uint8_t spi_tx_buffer[SPI_MAX_DMA_LEN] = {};
spi_device_handle_t lcd_spi_handle;

/* Definición de funciones */

// Full buffer DMA transfer
static int32_t spi_dma_transfer_bytes(uint8_t *data, uint16_t size)
{
	esp_err_t trans_result = ESP_OK;
	spi_transaction_t trans_t;

	// Prepare transaction parameters
	if (data == spi_rx_buffer)
	{
		trans_t.rx_buffer = spi_rx_buffer;
		trans_t.tx_buffer = NULL;
	}
	else
	{
		trans_t.rx_buffer = NULL;
		trans_t.tx_buffer = spi_tx_buffer;
	}

	trans_t.rxlength = 0;
	trans_t.length = 8 * size;
	trans_t.flags = 0;
	trans_t.cmd = 0;
	trans_t.addr = 0;
	trans_t.user = NULL;

	// Perform transaction
	trans_result = spi_device_transmit(lcd_spi_handle, &trans_t);
	if (ESP_OK != trans_result)
	{
		return SPI_TRANSFER_CANCELLED;
	}

	return size;
}

void COG_LCD_Lcd_Led_Power(lcd_led_power_t lcd_led_power)
{
	gpio_set_level(COG_LCD_LED, lcd_led_power % 2);
}

esp_err_t COG_LCD_Init(void)
{
	/* Habilitar los GPIOs para los pines de control */
	gpio_pad_select_gpio(COG_LCD_RST);
	gpio_pad_select_gpio(COG_LCD_RS);
	gpio_pad_select_gpio(COG_LCD_CSB);
	gpio_pad_select_gpio(COG_LCD_LED);

	/* Configurar los pines de control como salida */
	gpio_set_direction(COG_LCD_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(COG_LCD_RS, GPIO_MODE_OUTPUT);
	gpio_set_direction(COG_LCD_CSB, GPIO_MODE_OUTPUT);
	gpio_set_direction(COG_LCD_LED, GPIO_MODE_OUTPUT);

	/* Inicializar los pines de salida */
	gpio_set_level(COG_LCD_RST, 1);
	gpio_set_level(COG_LCD_RS, 0);
	gpio_set_level(COG_LCD_CSB, 1);
	COG_LCD_Lcd_Led_Power(lcd_led_off);

	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.duty_cycle_pos = 128,
		.cs_ena_pretrans = 0,
		.cs_ena_posttrans = 0,
		.clock_speed_hz = SPI_CLOCK,
		.mode = SPI_MODE,
		.spics_io_num = -1, // El CS se manipulará manualmente
		.queue_size = 1,
		.flags = 0,
		.pre_cb = NULL,
		.post_cb = NULL,
	};

	// add device to spi bus
	esp_err_t err = spi_bus_add_device(SPI_CHANNEL, &devcfg, &lcd_spi_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Falla en add device");
		return ESP_FAIL;
	}

	/* Resetear la pantalla */
	gpio_set_level(COG_LCD_RST, 0);
	vTaskDelay(2 / portTICK_PERIOD_MS);
	gpio_set_level(COG_LCD_RST, 1);

	/* Esperar un tiempo ... */
	vTaskDelay(20 / portTICK_PERIOD_MS);

	/* Secuencia de inicialización */
	COG_LCD_Tx_Command(0x30); // Despertar la pantalla
	vTaskDelay(2 / portTICK_PERIOD_MS);
	COG_LCD_Tx_Command(0x30);		   // Despertar la pantalla
	COG_LCD_Tx_Command(0x30);		   // Despertar la pantalla
	COG_LCD_Tx_Command(0x39);		   // Función de configuración
	COG_LCD_Tx_Command(0x14);		   // Frecuencia del oscilador interno
	COG_LCD_Tx_Command(0x56);		   // Control de energía
	COG_LCD_Tx_Command(0x6D);		   // Control del seguidor
	COG_LCD_Tx_Command(0x70);		   // Contraste
	COG_LCD_Tx_Command(LCD_DISP_ON);   // Encender display
	COG_LCD_Tx_Command(LCD_ENTRY_INC); // Modo entrada
	COG_LCD_Tx_Command(LCD_CLEAR);	   // Limpiar pantalla
	vTaskDelay(10 / portTICK_PERIOD_MS);

	return ESP_OK;
}

void COG_LCD_Tx_Command(uint8_t command)
{

	gpio_set_level(COG_LCD_CSB, 0);
	gpio_set_level(COG_LCD_RS, 0);

	spi_tx_buffer[0] = command;
	spi_dma_transfer_bytes(spi_tx_buffer, 1);

	gpio_set_level(COG_LCD_RS, 1);
	gpio_set_level(COG_LCD_CSB, 1);
}

void COG_LCD_Put_Char(char byte)
{

	gpio_set_level(COG_LCD_CSB, 0);
	gpio_set_level(COG_LCD_RS, 1);

	spi_tx_buffer[0] = byte;
	spi_dma_transfer_bytes(spi_tx_buffer, 1);

	gpio_set_level(COG_LCD_CSB, 1);
}

void COG_LCD_Put_String(char *str)
{

	uint8_t indice = 0;

	while ((indice < LCD_DISP_LENGTH) && (*str != '\0'))
	{
		COG_LCD_Put_Char(*str);
		str++;
	}
}

void COG_LCD_Goto_XY(uint8_t pos_x, uint8_t pos_y)
{

	uint8_t comando, offset, col;

	/* Ubicar la dirección de la línea correspondiente */
	switch (pos_y)
	{
	case 1:
		offset = LCD_START_LINE1;
		break;
	case 2:
		offset = LCD_START_LINE2;
		break;
	case 3:
		offset = LCD_START_LINE3;
		break;
	case 4:
		offset = LCD_START_LINE4;
		break;
	default:
		offset = LCD_START_LINE1;
		break;
	}

	/* Ubicar la dirección de la columna correspondiente*/
	if ((pos_x >= 1) && (pos_x <= LCD_DISP_LENGTH))
	{
		col = pos_x - 1;
	}
	else
	{
		col = 0;
	}

	comando = (offset + col) | 0b10000000;
	COG_LCD_Tx_Command(comando);
}
