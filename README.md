# MPU6500 ESP-IDF Driver

A lightweight ESP-IDF device driver for the MPU6500 IMU sensor.

Supports:

- Accelerometer (±2g / ±4g / ±8g / ±16g)
- Gyroscope (±250 / ±500 / ±1000 / ±2000 dps)
- Temperature reading
- Raw + scaled data output

---

## Features

✔ Clean ESP-IDF component  
✔ Register-level I²C driver  
✔ Scaled physical units (g, deg/s, °C)  
✔ Configurable sensor ranges  
✔ Example application included  

---

## Hardware

Tested with:

- ESP32 DevKit boards
- MPU6500 breakout modules

### Wiring

| MPU6500 | ESP32 |
|--------|------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

---

## Installation

Copy the component into your project:

