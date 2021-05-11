idf_component_register(SRCS "custom_adc.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_adc_cal freertos
                    )