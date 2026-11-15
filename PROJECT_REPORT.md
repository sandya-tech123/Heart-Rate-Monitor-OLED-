# Heart Rate Monitor (OLED) – Project Report

## 1. Aim
Build a low-cost heart rate monitor that measures pulse from a fingertip and displays BPM and the pulse waveform on an OLED screen.

## 2. Components
- Microcontroller: Arduino Uno / Nano or ESP32
- Pulse sensor: MAX30102 (red + infrared LEDs, photodetector, I2C)
- Display: 0.96" SSD1306 OLED, 128x64, I2C
- Jumper wires, breadboard, USB cable

## 3. Working principle (photoplethysmography)
Each heartbeat pushes blood into the fingertip. Blood absorbs more infrared light, so the light reaching the photodetector dips slightly with every beat. The sensor reports this as a changing IR value. The software finds the peaks of this signal and measures the time between them.

BPM = 60 / (time between two beats in seconds)

## 4. Software design
1. Read IR value from the MAX30102 on every loop.
2. If IR is below the finger threshold (50,000), show "PLACE FINGER" and reset the average.
3. Otherwise run beat detection (`checkForBeat`): detects a peak in the filtered signal.
4. On each beat, compute instantaneous BPM. Accept only 30–220 BPM.
5. Keep the last 4 accepted values and display their average to reduce jitter.
6. Scale the raw signal to 0–22 pixels and scroll it across the bottom of the screen.
7. Flash the heart icon on every detected beat.

## 5. Simulator
`web/index.html` reproduces the OLED screen (128x64, 1-bit pixels) in a browser. It generates a synthetic pulse signal with adjustable BPM, noise and finger on/off, runs a peak detector with a refractory period and a 4-beat average, and compares the detected BPM to the set value. Use it to test the display layout and the algorithm without hardware.

## 6. Testing
| Test | Expected | 
|---|---|
| No finger | Screen shows PLACE FINGER, BPM empty |
| Finger placed | "--" for the first beats, then BPM appears |
| Set 72 BPM in simulator | Detected BPM within ±2 after 5 beats |
| High noise | BPM may wobble; averaging limits the error |
| Remove finger | Screen returns to PLACE FINGER |

## 7. Limitations and future work
- Motion artefacts disturb the reading; keep the finger still.
- Not a medical device.
- Future: SpO2 from red/IR ratio, BPM history graph, buzzer on beat, Bluetooth/Wi-Fi logging.
