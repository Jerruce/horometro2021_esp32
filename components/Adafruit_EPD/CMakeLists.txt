idf_component_register(
    SRCS  "Adafruit_EPD.cpp"  "Adafruit_IL0373.cpp"
    INCLUDE_DIRS "."
    REQUIRES "Adafruit_GFX" "custom_spi"
)
