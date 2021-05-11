#include "custom_nvs.h"

#define TAG "CUSTOM_NVS"

static RTC_DATA_ATTR struct timeval sleep_enter_time;

int motor_inicia_trabajo()
{
    return gettimeofday(&sleep_enter_time, NULL);
}
esp_err_t motor_termina_trabajo()
{
    struct timeval sleep_exit_time;
    gettimeofday(&sleep_exit_time, NULL);

    uint32_t segundos_detectados = (uint32_t)(sleep_exit_time.tv_sec - sleep_enter_time.tv_sec) ;
    ESP_LOGI(TAG, "tiempo detectado Segs: %u", segundos_detectados);

    horometro_actualizar_segundos_NVS(segundos_nameSpace, segundos_detectados);

    alarma_t alarmas[4];
    if (horometro_extraer_alarmas_NVS(alarmas_nameSpace, alarmas) == ESP_OK)
    {
        for (int i = 0; i < 4; i++)
        {
            if (alarmas[i].alarma_id == i)
            {
                alarmas[i].segundos_acumulados += segundos_detectados;
            }
        }
        horometro_actualizar_alarmas_NVS(alarmas_nameSpace, alarmas);
    }

    uint32_t segundos = 0;
    esp_err_t err = horometro_extraer_segundos_NVS(segundos_nameSpace, &segundos);
    ESP_LOGI(TAG, "segundos:%u", segundos);
    return err;
}

void horometro_init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    /*
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_LOGE("NVS", "No free pages.");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

esp_err_t horometro_extraer_credenciales_wifi_from_nvs(const char *nameSpace, char *wifi_ssid, char *wifi_pswd)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    size_t ssid_size = 0;
    size_t pswd_size = 0;
    esp_err_t result_ssid = nvs_get_blob(handle, "wifi_ssid", NULL, &ssid_size);
    esp_err_t result_pswd = nvs_get_blob(handle, "wifi_pswd", NULL, &pswd_size);

    if (result_ssid != ESP_OK || result_pswd != ESP_OK)
    {
        return ESP_FAIL;
    }
    if (ssid_size <= 0 || pswd_size <= 0)
    {
        return ESP_FAIL;
    }

    result_ssid = nvs_get_blob(handle, "wifi_ssid", (void *)wifi_ssid, &ssid_size);
    result_pswd = nvs_get_blob(handle, "wifi_pswd", (void *)wifi_pswd, &pswd_size);

    if (result_ssid != ESP_OK || result_pswd != ESP_OK)
    {
        return ESP_FAIL;
    }

    nvs_close(handle);
    return ESP_OK;
}
esp_err_t horometro_actualizar_credenciales_wifi_to_nvs(const char *nameSpace, char *wifi_ssid, char *wifi_pswd)
{

    nvs_handle handle;
    esp_err_t err1, err2;
    err1 = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    err1 = nvs_set_blob(handle, "wifi_ssid", (void *)wifi_ssid, strlen(wifi_ssid) + 1);
    //err2 = nvs_set_blob(handle, "wifi_pswd", (void *)wifi_pswd, strlen(wifi_pswd) + 1);
    err2 = nvs_set_blob(handle, "wifi_pswd", (void *)"123456789", 9 + 1);
    if (err1 != ESP_OK || err2 != ESP_OK)
        return ESP_FAIL;

    err1 = nvs_commit(handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    nvs_close(handle);

    return ESP_OK;
}

esp_err_t horometro_extraer_motorID_NVS(const char *nameSpace, char *motorID)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    size_t motorID_size = 0;
    esp_err_t result_motorID = nvs_get_blob(handle, "motorID", NULL, &motorID_size);

    if (result_motorID != ESP_OK)
    {
        return ESP_FAIL;
    }
    if (motorID_size <= 0)
    {
        return ESP_FAIL;
    }

    result_motorID = nvs_get_blob(handle, "motorID", (void *)motorID, &motorID_size);

    if (result_motorID != ESP_OK)
    {
        return ESP_FAIL;
    }

    nvs_close(handle);
    return ESP_OK;
}
esp_err_t horometro_actualizar_motorID_NVS(const char *nameSpace, char *motorID)
{

    nvs_handle handle;
    esp_err_t err1;
    err1 = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    err1 = nvs_set_blob(handle, "motorID", (void *)motorID, strlen(motorID) + 1);

    if (err1 != ESP_OK)
        return ESP_FAIL;

    err1 = nvs_commit(handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    nvs_close(handle);

    return ESP_OK;
}

esp_err_t horometro_extraer_interfaces_NVS(const char *nameSpace, uint8_t *interfaces)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    esp_err_t result_interfaces = nvs_get_u8(handle, "interfaces", interfaces);

    if (result_interfaces != ESP_OK)
    {
        return ESP_FAIL;
    }

    nvs_close(handle);
    return ESP_OK;
}
esp_err_t horometro_actualizar_interfaces_NVS(const char *nameSpace, uint8_t interface, bool estado)
{

    nvs_handle handle;
    esp_err_t err1;
    err1 = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    uint8_t actual = 0;
    horometro_extraer_interfaces_NVS(nameSpace, &actual);

    actual = (actual & ~(1 << interface)) + ((estado ? 1 : 0) << interface);

    err1 = nvs_set_u8(handle, "interfaces", actual);

    if (err1 != ESP_OK)
        return ESP_FAIL;

    err1 = nvs_commit(handle);
    if (err1 != ESP_OK)
        return ESP_FAIL;

    nvs_close(handle);

    return ESP_OK;
}
int sensores_habilitados()
{
    uint8_t interfaces = 0;
    horometro_extraer_interfaces_NVS(interfaces_nameSpace, &interfaces);
    return (interfaces & (1 << interface_sensores));
}

esp_err_t horometro_extraer_alarmas_NVS(const char *nameSpace, alarma_t *alarmas)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    size_t size = 0;
    err = nvs_get_blob(handle, "alarmas_key", NULL, &size);

    if (err != ESP_OK)
    {
        return ESP_FAIL;
    }
    if (size <= 0)
    {
        return ESP_FAIL;
    }

    err = nvs_get_blob(handle, "alarmas_key", (void *)alarmas, &size);

    if (err != ESP_OK)
    {
        return ESP_FAIL;
    }

    nvs_close(handle);
    return ESP_OK;
}
esp_err_t horometro_actualizar_alarmas_NVS(const char *nameSpace, alarma_t *alarmas)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    err = nvs_set_blob(handle, "alarmas_key", (void *)alarmas, 4 * sizeof(alarma_t)); // 4 : #alarmas

    if (err != ESP_OK)
        return ESP_FAIL;

    err = nvs_commit(handle);
    if (err != ESP_OK)
        return ESP_FAIL;

    nvs_close(handle);

    return ESP_OK;
}

esp_err_t horometro_extraer_segundos_NVS(const char *nameSpace, uint32_t *segundos)
{
    nvs_handle handle;
    esp_err_t err;
    err = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "1111");
        return ESP_FAIL;
    }

    err = nvs_get_u32(handle, "segundos_key", segundos);

    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "2222");
        return ESP_FAIL;
    }

    nvs_close(handle);
    return ESP_OK;
}
esp_err_t horometro_actualizar_segundos_NVS(const char *nameSpace, uint32_t add)
{
    nvs_handle handle;
    esp_err_t err1;
    err1 = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err1 != ESP_OK)
    {
        ESP_LOGI(TAG, "3333");
        return ESP_FAIL;
    }

    uint32_t segundos_acumulados = 0;
    horometro_extraer_segundos_NVS(nameSpace, &segundos_acumulados);

    err1 = nvs_set_u32(handle, "segundos_key", segundos_acumulados + add);

    if (err1 != ESP_OK)
    {
        ESP_LOGI(TAG, "5555");
        return ESP_FAIL;
    }

    err1 = nvs_commit(handle);
    if (err1 != ESP_OK)
    {
        ESP_LOGI(TAG, "6666");
        return ESP_FAIL;
    }

    nvs_close(handle);

    return ESP_OK;
}
esp_err_t horometro_limpiar_segundos_NVS(const char *nameSpace)
{
    nvs_handle handle;
    esp_err_t err1;
    err1 = nvs_open(nameSpace, NVS_READWRITE, &handle);
    if (err1 != ESP_OK)
    {
        return ESP_FAIL;
    }

    err1 = nvs_set_u32(handle, "segundos_key", (uint32_t)0);

    if (err1 != ESP_OK)
    {
        return ESP_FAIL;
    }

    err1 = nvs_commit(handle);
    if (err1 != ESP_OK)
    {
        return ESP_FAIL;
    }

    nvs_close(handle);

    return ESP_OK;
}