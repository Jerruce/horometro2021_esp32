#ifndef _CUSTOM_FFT_H_
#define _CUSTOM_FFT_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void horometro_iniciarlizar_fft();
float horometro_realizar_sCorriente_fft(uint32_t (*f_input)(void));
void horometro_realizar_acelerometro_fft();

#ifdef __cplusplus
}
#endif

#endif