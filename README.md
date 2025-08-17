# AVR Home Automation System

## Overview

This project is an AVR-based home automation system using ATmega32, FreeRTOS, and custom MCAL/HAL drivers. It supports control and monitoring of lights, fan, security alarm, sensors, and more via physical buttons, DIP switches, and Bluetooth (USART).

## Features

- Modular MCAL (Microcontroller Abstraction Layer) and HAL (Hardware Abstraction Layer) drivers
- FreeRTOS-based multitasking
- LCD display for status and sensor readings
- EEPROM logging of events and system snapshots
- Sensor integration: LM35 temperature, LDR light
- Control via buttons, DIP switches, and Bluetooth

## Folder Structure

```
APP/         # Main application code
HAL/         # Hardware Abstraction Layer drivers (LCD, SSD, STEPPER, KPD)
MCAL/        # Microcontroller Abstraction Layer drivers (ADC, DIO, USART, etc.)
RTOS/        # FreeRTOS configuration and source
LIB/         # Common type and bit manipulation macros
```

## Getting Started

1. **Hardware Setup:**
   - ATmega32 microcontroller (8MHz internal oscillator)
   - Connect sensors, LCD, LEDs, fan, and buttons as per pin definitions in `main.c`
   - See comments in `main.c` for pin assignments
2. **Build:**
   - Use provided `makefile` in Debug/Release folders
   - Compile with AVR-GCC toolchain
3. **Flash:**
   - Use AVR programmer to upload the `.hex` file to the microcontroller
4. **Run:**
   - System starts with LCD status display
   - Control devices via buttons, DIP switches, or Bluetooth

## Drivers

### MCAL Drivers

- **DIO:** Digital Input/Output pin and port control
- **ADC:** Analog-to-Digital Converter for sensor readings
- **USART:** Serial communication for Bluetooth
- **Timers, SPI, TWI, WDT, EXTI, GIE:** Standard AVR peripherals

### HAL Drivers

- **LCD:** 16x2 character display
- **SSD:** Seven-segment display
- **STEPPER:** Stepper motor control
- **KPD:** Keypad interface

## How to Add/Use Drivers

- Include the relevant header in your application file
- Call the initialization function (e.g., `MDIO_vInit()`, `HLCD_vInit()`)
- Use provided APIs for pin, port, sensor, or display operations
- See each driver's `.h` file for available functions

## Example: Reading a Sensor

```c
MDIO_vInit();
MADC_vInit();
u16 temp = MADC_u16AnalogRead(4); // Read LM35 on ADC4
```

## License
MIT License 