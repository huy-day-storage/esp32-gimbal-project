# ICM20602 ESP32 Library

## Features
- I2C & SPI support
- Accelerometer / Gyroscope / Temperature
- Calibration offset
- Simple low-pass filter
- ESP32 optimized

## I2C wiring
VCC  -> 3.3V  
GND  -> GND  
SCL  -> GPIO22  
SDA  -> GPIO21  
CS   -> 3.3V  

## SPI wiring
VCC  -> 3.3V  
GND  -> GND  
SCK  -> GPIO18  
MISO -> GPIO19  
MOSI -> GPIO23  
CS   -> GPIO5  

## Default I2C Address
0x68 (SA0 = GND)
