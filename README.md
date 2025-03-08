# TempLog Firmware
An arduino firmware for temperature logging software.
The hardware is based on an ATmega2560 board with PNP constant current generators and PT1000 sensors connected to analog inputs.
The firmware relies on a simple serial communication protocol to send the temperatures, adjust calibration, and set the number of connected PT1000, with a maximum of 16 channels.
