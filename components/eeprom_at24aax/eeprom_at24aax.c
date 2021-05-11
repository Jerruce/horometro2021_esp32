
#include "eeprom_at24aax.h"
#define TAG "eeprom at24aax"

sistema_eeprom_t sys;

const char *at24aax_err_to_name(at24aax_err_t err)
{
	switch (err)
	{
	case AT24AAX_OK:
		return "AT24AAX_OK";
	case AT24AAX_FALLA:
		return "AT24AAX_FALLA";
	case AT24AAX_MEMORIA_CON_SISTEMA:
		return "AT24AAX_MEMORIA_CON_SISTEMA";
	case AT24AAX_MEMORIA_SIN_SISTEMA:
		return "AT24AAX_MEMORIA_SIN_SISTEMA";
	case AT24AAX_TOKEN_OK:
		return "AT24AAX_TOKEN_OK";
	case AT24AAX_NO_HAY_ALARMAS:
		return "AT24AAX_NO_HAY_ALARMAS";
	default:
		return "AT24AAX_DESCONOCIDA";
	}
}
at24aax_err_t at24aax_init(index_t index, pagesize_t pageSize)
{
	sys.alarmas_detectadas = 0;
	sys.alarmas_permitidas = 10;
	sys.alarma_len_max = 100;
	sys.alarma_pos = 1;
	sys.token_pos = 3200;
	sys.token = (void *)"HorometroEEPROM";

	(sys.mem).dev_addrr = AT24CX_BASE_ID | (index & 0x7);
	sys.mem.pageSize = pageSize;

	switch (at24aax_escribir_token(&sys))
	{
	case AT24AAX_MEMORIA_SIN_SISTEMA:
		return at24aax_instalar_sistema(&sys);
	case AT24AAX_MEMORIA_CON_SISTEMA:
		return at24aax_copiar_sistema(&sys);
	default:
		return AT24AAX_FALLA;
	}
}
void at24aax_imprimir_datos_de_sistema(sistema_eeprom_t *sys)
{
	sistema_eeprom_t s2 = {
		.mem = {
			.dev_addrr = sys->mem.dev_addrr,
			.pageSize = sys->mem.pageSize},
		.token = sys->token};

	int dev_addrr = s2.mem.dev_addrr;

	at24aax_read_16bit(dev_addrr, 0, &(s2.alarmas_detectadas));
	at24aax_read_16bit(dev_addrr, 2, &(s2.alarmas_permitidas));
	at24aax_read(dev_addrr, 4, &(s2.alarma_len_max));
	at24aax_read(dev_addrr, 5, &(s2.alarma_pos));
	at24aax_read_16bit(dev_addrr, 6, &(s2.token_pos));

	char bufer[30];
	at24aax_leer_token(&s2, bufer);
	ESP_LOGE(TAG, "alarmas_detectadas:%u", s2.alarmas_detectadas);
	ESP_LOGE(TAG, "alarmas_permitidas:%u", s2.alarmas_permitidas);
	ESP_LOGE(TAG, "alarma_len_max:%u", s2.alarma_len_max);
	ESP_LOGE(TAG, "alarma_pos:%u", s2.alarma_pos);
	ESP_LOGE(TAG, "token_pos:%u", s2.token_pos);
	ESP_LOGE(TAG, "dev_addrr:%u", s2.mem.dev_addrr);
	ESP_LOGE(TAG, "pagesize:%u", s2.mem.pageSize);
	ESP_LOGE(TAG, "token:%s_", bufer);
}
at24aax_err_t at24aax_borrar_alarmas()
{
	sys.alarmas_detectadas = 0;
	sys.alarma_pos = 1;
	return at24aax_actualizar_sistema(&sys);
}
// byte 0-7 ocupados por el sistema
at24aax_err_t at24aax_escribir_alarma(char *alarma)
{
	//Extraer variables del sistema eeprom
	uint8_t pos = sys.alarma_pos;
	uint16_t alarma_addrr = 8 + (sys.alarma_len_max) * (pos - 1);
	int dev_addrr = sys.mem.dev_addrr;
	uint16_t detectadas = sys.alarmas_detectadas;
	uint16_t permitidas = sys.alarmas_permitidas;

	//Escribir el string ALARMA
	if (at24aax_write_string(dev_addrr, alarma_addrr, alarma) != ESP_OK)
	{
		ESP_LOGE(TAG, "Falla al escribir alarma.");
		return AT24AAX_FALLA;
	}

	//Actualizar variables del sistema eeprom
	detectadas++;
	pos++;
	sys.alarmas_detectadas = detectadas > permitidas ? permitidas : detectadas;
	sys.alarma_pos = pos > permitidas ? 1 : pos;

	if (at24aax_actualizar_sistema(&sys) != AT24AAX_OK)
	{
		ESP_LOGE(TAG, "Falla al actualizar sistema");
		return AT24AAX_FALLA;
	}

	ESP_LOGE(TAG, "Alarma escrita OK");
	return AT24AAX_OK;
}

//lee la alarma mas antigua y la borra del sistema
at24aax_err_t at24aax_leer_alarmas(char *bufer)
{
	/*
	if (sys.alarmas_detectadas == 0)
	{
		return AT24AAX_NO_HAY_ALARMAS;
	}
	*/

	//Extraer variables del sistema eeprom
	uint16_t detectadas = sys.alarmas_detectadas;
	uint16_t permitidas = sys.alarmas_permitidas;
	uint8_t pos = sys.alarma_pos;
	uint8_t pos_to_read = permitidas + pos - detectadas;
	if (pos_to_read > permitidas)
		pos_to_read -= permitidas;
	uint16_t alarma_addrr;
	int dev_addrr = sys.mem.dev_addrr;
//ESP_LOGE(TAG, "POINT 6");
	//Realizar lectura de alarmas
	esp_err_t err;
	char *_bufer = bufer;
	sprintf(_bufer, "{\"alarmas\":[");
	_bufer = _bufer + strlen(_bufer);
//ESP_LOGE(TAG, "POINT 7");
	do
	{
		if (detectadas == 0) //parche
		{
			_bufer++;
			break;
		}

		//Calcular addrr de alarma
		alarma_addrr = 8 + (sys.alarma_len_max) * (pos_to_read - 1);

		//Leer longitud de alarma(1er byte)
		uint8_t alarma_len;		
		at24aax_read(dev_addrr, alarma_addrr, &alarma_len);

		//Leer alarma
		err = at24aax_read_string(dev_addrr, alarma_addrr + 1, _bufer, alarma_len);
		if (err != ESP_OK)
			return AT24AAX_FALLA;

		_bufer += alarma_len;
		//*_bufer = '\n'; // cambiar por ','
		//_bufer++;
		*_bufer = ','; // cambiar por ','
		_bufer++;

		//Actualizar nueva posicion a leer
		pos_to_read++;
		if (pos_to_read > permitidas)
			pos_to_read -= permitidas;
	} while (pos_to_read != pos);

	_bufer--;
	*_bufer = ']';
	_bufer++;
	*_bufer = '}';
	_bufer++;
	*_bufer = 0;

	ESP_LOGE(TAG, "Leer alarmas OK:\n%s", bufer);
	return AT24AAX_OK;
}

at24aax_err_t at24aax_actualizar_sistema(sistema_eeprom_t *sys)
{
	uint16_t alarmas_detectadas = sys->alarmas_detectadas; //byte 0, 1 --> alarmas guardadas
	uint8_t alarma_pos = sys->alarma_pos;				   // byte 5 --> posicion actual //   alarma1, alarma2, ... , alarma 10

	at24aax_write(sys->mem.dev_addrr, 0, alarmas_detectadas >> 8);
	at24aax_write(sys->mem.dev_addrr, 1, alarmas_detectadas & 0xFF);

	esp_err_t err = at24aax_write(sys->mem.dev_addrr, 5, alarma_pos);

	if (err != ESP_OK)
		return AT24AAX_FALLA;
	ESP_LOGE(TAG, "sistema actualizado ok");
	return AT24AAX_OK;
}
at24aax_err_t at24aax_instalar_sistema(sistema_eeprom_t *sys)
{
	uint16_t alarmas_detectadas = sys->alarmas_detectadas; //byte 0, 1 --> alarmas guardadas
	uint16_t alarmas_permitidas = sys->alarmas_permitidas; //byte 2, 3 --> alarmas maxima
	uint8_t alarma_len_max = sys->alarma_len_max;		   //byte 4 --> longitud de alarma
	uint8_t alarma_pos = sys->alarma_pos;				   // byte 5 --> posicion actual //   alarma1, alarma2, ... , alarma 10
	uint16_t token_pos = sys->token_pos;				   // bytes 6 , 7 ---> posicion del token

	at24aax_write(sys->mem.dev_addrr, 0, alarmas_detectadas >> 8);
	at24aax_write(sys->mem.dev_addrr, 1, alarmas_detectadas & 0xFF);

	at24aax_write(sys->mem.dev_addrr, 2, alarmas_permitidas >> 8);
	at24aax_write(sys->mem.dev_addrr, 3, alarmas_permitidas & 0xFF);

	at24aax_write(sys->mem.dev_addrr, 4, alarma_len_max);

	at24aax_write(sys->mem.dev_addrr, 5, alarma_pos);

	at24aax_write(sys->mem.dev_addrr, 6, token_pos >> 8);
	esp_err_t err = at24aax_write(sys->mem.dev_addrr, 7, token_pos & 0xFF);
	if (err != ESP_OK)
		return AT24AAX_FALLA;

	ESP_LOGE(TAG, "sistema instalado ok");
	at24aax_imprimir_datos_de_sistema(sys);

	return AT24AAX_OK;
}
at24aax_err_t at24aax_copiar_sistema(sistema_eeprom_t *sys)
{
	int dev_addrr = sys->mem.dev_addrr;

	uint16_t alarmas_detectadas; //byte 0, 1 --> alarmas guardadas
	uint16_t alarmas_permitidas; //byte 2, 3 --> alarmas maxima
	uint8_t alarma_len_max;		 //byte 4 --> longitud de alarma
	uint8_t alarma_pos;			 // byte 5 --> posicion actual //   alarma1, alarma2, ... , alarma 10
	uint16_t token_pos;			 // bytes 6 , 7 ---> posicion del token

	at24aax_read_16bit(dev_addrr, 0, &(alarmas_detectadas));
	at24aax_read_16bit(dev_addrr, 2, &(alarmas_permitidas));
	at24aax_read(dev_addrr, 4, &(alarma_len_max));
	at24aax_read(dev_addrr, 5, &(alarma_pos));
	esp_err_t err = at24aax_read_16bit(dev_addrr, 6, &(token_pos));

	sys->alarmas_detectadas = alarmas_detectadas;
	sys->alarmas_permitidas = alarmas_permitidas;
	sys->alarma_len_max = alarma_len_max;
	sys->alarma_pos = alarma_pos;
	sys->token_pos = token_pos;

	if (err != ESP_OK)
		return AT24AAX_FALLA;

	ESP_LOGE(TAG, "sistema copiado ok");
	at24aax_imprimir_datos_de_sistema(sys);

	return AT24AAX_OK;
}
esp_err_t at24aax_leer_token(sistema_eeprom_t *sys, char *data)
{
	return at24aax_read_string((sys->mem).dev_addrr, sys->token_pos, data, strlen((sys->token))); //strlen no incluye a null
}

//si no hay token, entonces escribe token nuevo.
at24aax_err_t at24aax_escribir_token(sistema_eeprom_t *sys)
{
	char bufer[30];
	esp_err_t err;
	err = at24aax_leer_token(sys, bufer);
	if (err != ESP_OK)
		return AT24AAX_FALLA;

	ESP_LOGE(TAG, "token _ escribir token:%s__", bufer);
	if (strcmp(sys->token, bufer) != 0)
	{
		ESP_LOGE(TAG, "No se encontro token, escribiendo nuevo token...");
		err = at24aax_write_string((sys->mem).dev_addrr, sys->token_pos, sys->token);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Falla escribiendo nuevo token.");
			//TODO: memoria eeprom en falla
			return AT24AAX_FALLA;
		}
		if (at24aax_verificar_token(sys) != AT24AAX_TOKEN_OK)
		{
			return AT24AAX_FALLA;
		}
		return AT24AAX_MEMORIA_SIN_SISTEMA;
	}
	else
	{
		ESP_LOGE(TAG, "Se encontro token.");
		return AT24AAX_MEMORIA_CON_SISTEMA;
	}
}

at24aax_err_t at24aax_verificar_token(sistema_eeprom_t *sys)
{
	char bufer[30];
	esp_err_t err;

	ESP_LOGE(TAG, "Verificando token...");
	err = at24aax_leer_token(sys, bufer);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Paso 1 de verificacion fallida.");
		return AT24AAX_FALLA;
	}
	ESP_LOGE(TAG, "token:%s__", bufer);

	if (strcmp(sys->token, bufer) != 0)
	{
		ESP_LOGE(TAG, "Paso 2 de verificacion fallida.");
		return AT24AAX_FALLA;
	}
	ESP_LOGE(TAG, "Verificacion OK.");

	return AT24AAX_TOKEN_OK;
}

esp_err_t at24aax_read_string(int dev_addrr, unsigned int address, char *data, int len)
{
	esp_err_t err;
	uint8_t *_data = (uint8_t *)data;
	for (int i = 0; i < len; i++)
	{
		err = at24aax_read(dev_addrr, address + i, _data + i);
		if (err != ESP_OK)
			return ESP_FAIL;
	}
	_data[len] = 0;
	return ESP_OK;
}
esp_err_t at24aax_write_string(int dev_addrr, unsigned int address, char *data)
{
	uint8_t len = strlen(data);
	uint8_t *_data = (uint8_t *)data;
	esp_err_t err;
	for (int i = 0; i < len; i++)
	{
		err = at24aax_write(dev_addrr, address + i, *(_data + i));
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "falla en: %c , len:%u", *(_data + i), len);
			return ESP_FAIL;
		}
	}
	return ESP_OK;
}

esp_err_t at24aax_write(int dev_addrr, unsigned int address, uint8_t data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addrr << 1), ACK_CHECK_EN);

	i2c_master_write_byte(cmd, address >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, address & 0xFF, ACK_CHECK_EN);

	i2c_master_write(cmd, &data, 1, ACK_CHECK_EN);

	i2c_master_stop(cmd);

	//esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	esp_err_t ret = i2c_master_cmd_begin(EEPROM_I2C_BUS, cmd, 1000 / portTICK_RATE_MS);
	vTaskDelay(pdMS_TO_TICKS(20)); // be careful with this time
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t at24aax_read(int dev_addrr, unsigned int address, uint8_t *data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addrr << 1), ACK_CHECK_EN);

	i2c_master_write_byte(cmd, address >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, address & 0xFF, ACK_CHECK_EN);

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addrr << 1) + 1, ACK_CHECK_EN);

	i2c_master_read_byte(cmd, data, NACK_VAL);
	i2c_master_stop(cmd);

	//esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	esp_err_t ret = i2c_master_cmd_begin(EEPROM_I2C_BUS, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}
esp_err_t at24aax_read_16bit(int dev_addrr, unsigned int address, uint16_t *data)
{
	uint8_t low_byte, high_byte;
	esp_err_t err;
	at24aax_read(dev_addrr, address, &high_byte);
	err = at24aax_read(dev_addrr, address + 1, &low_byte);
	*data = (high_byte << 8) + low_byte;
	return err;
}