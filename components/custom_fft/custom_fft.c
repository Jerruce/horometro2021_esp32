#include "custom_fft.h"
#include "esp_log.h"
#include "esp_err.h"
#include <math.h>
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_dsp.h"

#define N_SAMPLES 1024 //1024
#define TAG "custom fft"

float *x1;
float *x2;
float *wind;
float *y_cf;
float *y1_cf;
float *y2_cf;

static int find_maximum(float a[], int n)
{
    int c, index = 0;

    for (c = 1; c < n; c++)
        if (a[c] > a[index])
            index = c;

    return index;
}

void horometro_iniciarlizar_fft()
{
    x1 = calloc(N_SAMPLES, sizeof(float));
    x2 = calloc(N_SAMPLES, sizeof(float));
    wind = calloc(N_SAMPLES, sizeof(float));
    y_cf = calloc(N_SAMPLES * 2, sizeof(float));
    y1_cf = &y_cf[0];
    y2_cf = &y_cf[N_SAMPLES];

    int ret = dsps_fft2r_init_fc32(NULL, 4096);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        return;
    }
}
float horometro_realizar_sCorriente_fft(uint32_t (*f_input)(void))
{
    // Generate hann window
    dsps_wind_hann_f32(wind, N_SAMPLES);
    // Generate input signal for x1 A=1 , F=0.1
    for (int i = 0; i < N_SAMPLES; i++)
    {
        x1[i] = f_input();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    // Generate input signal for x2 A=0.1,F=0.2
    for (int i = 0; i < N_SAMPLES; i++)
    {
        x2[i] = 0.0;
    }

    // Convert two input vectors to one complex vector
    for (int i = 0; i < N_SAMPLES; i++)
    {
        y_cf[i * 2 + 0] = x1[i] * wind[i];
        //y_cf[i * 2 + 0] = x1[i] ;
        y_cf[i * 2 + 1] = 0;
    }
    // FFT
    unsigned int start_b = xthal_get_ccount();
    dsps_fft2r_fc32(y_cf, N_SAMPLES);
    unsigned int end_b = xthal_get_ccount();
    // Bit reverse
    dsps_bit_rev_fc32(y_cf, N_SAMPLES);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(y_cf, N_SAMPLES);

    for (int i = 0; i < N_SAMPLES / 2; i++)
    {
        y1_cf[i] = 2.0 * sqrtf(y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1]) / N_SAMPLES;
    }

    // Show power spectrum in 64x10 window from -100 to 0 dB from 0..N/4 samples
                //ESP_LOGW(TAG, "Signal x1");
                //dsps_view(y1_cf, N_SAMPLES / 2, 100, 30, 0.0, 3300.0, '*');

    //ESP_LOGI(TAG, "FFT for %i complex points take %i cycles", N_SAMPLES, end_b - start_b);

    //ESP_LOGI(TAG, "End Example.");

    for (int i = 0; i < N_SAMPLES / 2; i++)
    {
        if (y1_cf[i] > 300.0)
        {
            ESP_LOGI(TAG, "___y1_cf[%i]=%f___", i, y1_cf[i]);
        }
    }
    y1_cf[0] = 0.0;
    y1_cf[1] = 0.0;
    y1_cf[2] = 0.0;
    y1_cf[3] = 0.0;
    y1_cf[4] = 0.0;
    y1_cf[5] = 0.0;

    int index = find_maximum(y1_cf, N_SAMPLES / 2);
    ESP_LOGI(TAG, "y1_cf[%i]=%f", index, y1_cf[index]);

    if (!(index > 100 && index < 140))
    {
        ESP_LOGE(TAG, "Falla en red electrica");
        return -1.0; // numero imposible porq fc=500Hz
    }
    //float y_x_hz = y1_cf[index] / 1000.0; // pasar a voltios
    float corriente = y1_cf[index] * 0.0053 - 0.0456;
    return corriente;
    //TODO: AUMENTAR numero de muestras por onda
}
void horometro_realizar_acelerometro_fft()
{
}