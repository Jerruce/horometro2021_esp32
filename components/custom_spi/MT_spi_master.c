/* Inclusion de archivos */
#include "MT_spi_master.h"

#define TAG "MT SPI"

//spi_device_handle_t hspi_handle;
//spi_device_handle_t vspi_handle;

uint8_t spi_slave_rx_buffer[20];



// Initialize the SPI2 device in master mode
void HSPI_Master_Config(uint8_t spi_mode, int sck_hz, int bit_order, spi_device_handle_t *my_dev_handle)
{

	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = HSPI_MOSI_GPIO,
		.miso_io_num = HSPI_MISO_GPIO,
		.sclk_io_num = HSPI_SCK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
		//.max_transfer_sz = SPI_MAX_DMA_LEN,
	};

	ESP_LOGI(TAG, "... Initializing HSPI bus in master mode.");
	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));


	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.duty_cycle_pos = 128,
		.cs_ena_pretrans = 0,
		.cs_ena_posttrans = 0,
		.clock_speed_hz = sck_hz,
		.mode = spi_mode,
		.spics_io_num = -1, // The Chip Select pin will be controlled manually
		.queue_size = 1,
		.flags = bit_order,
		.pre_cb = NULL,
		.post_cb = NULL
	};

	ESP_LOGI(TAG, "... Adding device to HSPI bus.");
	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, my_dev_handle));
}




void VSPI_Master_Config(uint8_t spi_mode, int sck_hz, int bit_order, spi_device_handle_t *my_dev_handle)
{

	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = VSPI_MOSI_GPIO,
		.miso_io_num = VSPI_MISO_GPIO,
		.sclk_io_num = VSPI_SCK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
		//.max_transfer_sz = SPI_MAX_DMA_LEN,
	};

	ESP_LOGI(TAG, "... Initializing VSPI bus in master mode.");
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 1));	

	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.duty_cycle_pos = 128,
		.cs_ena_pretrans = 0,
		.cs_ena_posttrans = 0,
		.clock_speed_hz = sck_hz,
		.mode = spi_mode,
		.spics_io_num = -1, // The Chip Select pin will be controlled manually
		.queue_size = 1,
		.flags = bit_order,
		.pre_cb = NULL,
		.post_cb = NULL
	};

	ESP_LOGI(TAG, "... Adding device to VSPI bus.");
	ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, my_dev_handle));

}


int32_t HSPI_Polling_Transfer_Bytes(uint8_t *spi_tx_buffer, uint8_t *spi_rx_buffer, uint16_t n_bytes, spi_device_handle_t *my_dev_handle) {

	esp_err_t trans_result = ESP_OK;
	spi_transaction_t trans_t;

	// Prepare transaction parameters
	trans_t.rx_buffer = spi_rx_buffer;
	trans_t.tx_buffer = spi_tx_buffer;
	trans_t.rxlength = 0;
	trans_t.length = 8 * n_bytes;
	trans_t.flags = 0;
	trans_t.cmd = 0;
	trans_t.addr = 0;
	trans_t.user = NULL;

	// Perform transaction
	trans_result = spi_device_polling_transmit(*my_dev_handle, &trans_t);
	if (ESP_OK != trans_result) {
		return SPI_TRANSFER_CANCELLED;
	}

	return n_bytes;
}


int32_t VSPI_Polling_Transfer_Bytes(uint8_t *spi_tx_buffer, uint8_t *spi_rx_buffer, uint16_t n_bytes, spi_device_handle_t *my_dev_handle) {

	esp_err_t trans_result = ESP_OK;
	spi_transaction_t trans_t;

	// Prepare transaction parameters
	trans_t.rx_buffer = spi_rx_buffer;
	trans_t.tx_buffer = spi_tx_buffer;
	trans_t.rxlength = 0;
	trans_t.length = 8 * n_bytes;
	trans_t.flags = 0;
	trans_t.cmd = 0;
	trans_t.addr = 0;
	trans_t.user = NULL;

	// Perform transaction
	trans_result = spi_device_polling_transmit(*my_dev_handle, &trans_t);
	if (ESP_OK != trans_result) {
		return SPI_TRANSFER_CANCELLED;
	}

	return n_bytes;
}


void HSPI_Slave_Config(uint8_t spi_mode, int bit_order, slave_transaction_cb_t my_post_setup_cb, slave_transaction_cb_t my_post_trans_cb){

	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = HSPI_MOSI_GPIO,
		.miso_io_num = HSPI_MISO_GPIO,
		.sclk_io_num = HSPI_SCK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
		//.max_transfer_sz = SPI_MAX_DMA_LEN,
	};


	// Configuration for the SPI slave interface
	spi_slave_interface_config_t slvcfg = {
		.spics_io_num = HSPI_CS_GPIO,
		.flags = bit_order,
		.queue_size = 1,
		.mode = spi_mode,
		.post_setup_cb = my_post_setup_cb,
		.post_trans_cb = my_post_trans_cb
	};

	ESP_LOGI(TAG, "... Initializing HSPI bus in slave mode.");
	ESP_ERROR_CHECK(spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 2));

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(HSPI_MOSI_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(HSPI_SCK_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(HSPI_CS_GPIO, GPIO_PULLUP_ONLY);

}



void VSPI_Slave_Config(uint8_t spi_mode, int bit_order, slave_transaction_cb_t my_post_setup_cb, slave_transaction_cb_t my_post_trans_cb){

	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = VSPI_MOSI_GPIO,
		.miso_io_num = VSPI_MISO_GPIO,
		.sclk_io_num = VSPI_SCK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
		//.max_transfer_sz = SPI_MAX_DMA_LEN,
	};

	// Configuration for the SPI slave interface
	spi_slave_interface_config_t slvcfg = {
		.spics_io_num = VSPI_CS_GPIO,
		.flags = bit_order,
		.queue_size = 1,
		.mode = spi_mode,
		.post_setup_cb = my_post_setup_cb,
		.post_trans_cb = my_post_trans_cb
	};	

	ESP_LOGI(TAG, "... Initializing VSPI bus in slave mode.");
	ESP_ERROR_CHECK(spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, 2));

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(VSPI_MOSI_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(VSPI_SCK_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(VSPI_CS_GPIO, GPIO_PULLUP_ONLY);


}