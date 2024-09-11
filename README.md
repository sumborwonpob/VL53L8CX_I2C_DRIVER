# VL53L8CX_I2C_DRIVER
An implementation of VL53L8CX I2C driver based on STM's VL53L8CX Ultra-light Driver (ULD) 2.0.0.
https://www.st.com/en/embedded-software/stsw-img040.html

## Environment tested
Device: Pololu VL53L8CX carrier board
https://www.pololu.com/product/3419

Host Device: Raspberry Pi 4B

OS: Ubuntu 22:04

Protocol: I2C

## Pre-requisite
* Enable RPi's I2C
```
sudo apt install raspi-config # Install config program
sudo raspi-config # Open config

# Select 3 Interface Options > 15 I2C
# Enable it

sudo apt-get install libi2c-dev # Install C library for i2c

i2cdetect -y 1 # Check device address
# Note that the address shown in this command may not be the actual address.
# According to the Datasheet, the sensor's default address is 0x52.
```

## Connections
[Carrier Board] -> [RPi]

Vcc -> 3.3V

GND -> GND

SCL -> SCL

SDA -> SDA

SPI/I2C -> GND

## In code usage
Refer to the example.cpp for the basic usage.

Additional usage can be found in the original ULD.
