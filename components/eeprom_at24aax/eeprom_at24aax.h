#ifndef _AT24C256_h
#define _AT24C256_h

#include "stdint.h"
#include <stdio.h>
#include "string.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "MT_i2c_master.h"
// AT24Cx I2C adress
// 80
// 0x50
//#define AT24CX_ID B1010000
#define AT24CX_BASE_ID 80
#define EEPROM_I2C_BUS I2C_NUM_1

typedef enum
{
    AT24aa256_pagesize = 64
} pagesize_t;

typedef enum
{
    A2A1A0_000 = 0,
    A2A1A0_001,
    A2A1A0_010,
    A2A1A0_011,
    A2A1A0_100,
    A2A1A0_101,
    A2A1A0_110,
    A2A1A0_111,
} index_t;

typedef struct
{
    int dev_addrr;
    pagesize_t pageSize;
} at24aax_t;

typedef struct
{
    at24aax_t mem;
    uint16_t alarmas_detectadas; //byte 0, 1 --> alarmas guardadas
    uint16_t alarmas_permitidas; //byte 2, 3 --> alarmas maxima
    uint8_t alarma_len_max;      //byte 4 --> longitud de alarma
    uint8_t alarma_pos;          // byte 5 --> posicion actual //   alarma1, alarma2, ... , alarma 10
    uint16_t token_pos;          // bytes 6 , 7 ---> posicion del token
    void *token;                 //on mcu
} sistema_eeprom_t;

typedef enum
{
    AT24AAX_OK = 0,
    AT24AAX_FALLA,
    AT24AAX_MEMORIA_CON_SISTEMA,
    AT24AAX_MEMORIA_SIN_SISTEMA,
    AT24AAX_TOKEN_OK,
    AT24AAX_NO_HAY_ALARMAS
} at24aax_err_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t at24aax_write(int dev_addrr, unsigned int address, uint8_t data);
esp_err_t at24aax_read(int dev_addrr, unsigned int address, uint8_t *data);
esp_err_t at24aax_read_16bit(int dev_addrr, unsigned int address, uint16_t *data);
esp_err_t at24aax_write_string(int dev_addrr, unsigned int address, char *data);
esp_err_t at24aax_read_string(int dev_addrr, unsigned int address, char *data, int len);

at24aax_err_t at24aax_init( index_t index, pagesize_t pageSize);

//TOKEN
esp_err_t at24aax_leer_token(sistema_eeprom_t *sys, char *data);
at24aax_err_t at24aax_escribir_token(sistema_eeprom_t *sys);
at24aax_err_t at24aax_verificar_token(sistema_eeprom_t *sys);

//SISTEMA
at24aax_err_t at24aax_instalar_sistema(sistema_eeprom_t *sys);
at24aax_err_t at24aax_actualizar_sistema(sistema_eeprom_t *sys);
at24aax_err_t at24aax_copiar_sistema(sistema_eeprom_t *sys);
void at24aax_imprimir_datos_de_sistema(sistema_eeprom_t *sys);

//ALARMA API
at24aax_err_t at24aax_leer_alarmas( char *bufer);
at24aax_err_t at24aax_escribir_alarma( char *alarma);
at24aax_err_t at24aax_borrar_alarmas();
const char *at24aax_err_to_name(at24aax_err_t err);
//typedef void* mvh3004d_handle_t;

#ifdef __cplusplus
}
#endif

#endif