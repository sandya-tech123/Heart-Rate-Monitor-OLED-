# Heart Rate Monitor (OLED)

A heart rate monitor that reads a pulse sensor (MAX30102) and shows BPM and a live pulse waveform on a 128x64 SSD1306 OLED.

This repository is the **software side** of the project:

| Folder | What it is |
|---|---|
| `web/index.html` | Browser simulator of the OLED monitor. Runs the same beat-detection idea as the firmware, no hardware needed. |
| `firmware/heart_rate_monitor/` | Arduino sketch for Uno / Nano / ESP32. |
| `docs/PROJECT_REPORT.md` | Project report: aim, design, algorithm, testing, results. |

## Run the simulator
Open `web/index.html` in any browser. To host it as a website, enable GitHub Pages (Settings > Pages > Deploy from branch) and copy `index.html` to the repository root or `/docs`.

## Run the firmware
1. Install in Arduino IDE Library Manager: *SparkFun MAX3010x*, *Adafruit SSD1306*, *Adafruit GFX*.
2. Open `firmware/heart_rate_monitor/heart_rate_monitor.ino`.
3. Select your board and upload. Open Serial Monitor at 115200 baud.

## Wiring (I2C)
| Module | Pin | Uno/Nano | ESP32 |
|---|---|---|---|
| MAX30102 | VIN / GND | 3.3V / GND | 3.3V / GND |
| MAX30102 | SDA / SCL | A4 / A5 | GPIO21 / GPIO22 |
| SSD1306 OLED | VCC / GND | 5V or 3.3V / GND | 3.3V / GND |
| SSD1306 OLED | SDA / SCL | A4 / A5 | GPIO21 / GPIO22 |

Both modules share the same two I2C lines (addresses 0x57 and 0x3C).

> This is a learning project, not a medical device.
