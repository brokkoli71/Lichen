# Hardware

ESP32-based sensor unit measuring **air quality, humidity and temperature**, showing
them on a small OLED and serving them over HTTP.

## Bill of materials

| part | detail |
|---|---|
| MCU board | Classic **ESP32**. Two different boards have been used and both work — the code doesn't depend on a specific one. Classic pin numbering is assumed (GPIO34 is input-only); an ESP32-C3/S2/S3 would need the pins remapped. |
| Display | 1.3" OLED, 128 × 64, I2C, **SH1106**, blue — [arduino-projekte.info](https://arduino-projekte.info/products/1-3-oled-display-128x64-i2c-sh1106-blue?variant=50481301750024) |
| Air quality sensor | **MQ135** module (analog) |
| Temperature / humidity | **DHT11** module |
| Power switch | Mini rocker switch, KCD1-101 style (19 × 13 mm panel cutout) |
| Case | 4 mm plywood, lasercut — see [`../case/`](../case/) |

Two sensors: the MQ135 covers air quality, the DHT11 covers both temperature and
humidity — hence the separate `Co2` and `temp` openings in the case.

> **The MQ135 is not a CO2 sensor.** It's a tin-oxide sensor that responds to VOCs,
> ammonia, benzene, smoke and alcohol as much as to CO2, and the firmware converts its
> ADC reading to "ppm" with a plain linear rescale rather than the sensor's actual
> response curve. Readings trend with air quality but are not a calibrated
> concentration. See [`../firmware/README.md`](../firmware/README.md).

DHT11 accuracy is ±2 °C and ±5 % RH, with 1 °C / 1 % resolution — fine for a room
display, not for anything needing precision. A DHT22 is pin-compatible if that matters
later; it would need `DHT_TYPE` changed.

## Wiring

| function | GPIO |
|---|---|
| MQ135 analog out | 34 |
| DHT11 data | 27 |
| OLED SDA | 23 |
| OLED SCL | 18 |
| Power switch | 25 (`INPUT_PULLUP`, closed to GND = on) |

## Openings in the case

Measured from `case/cut/lichen_sept_9_v4.svg` — the physical constraints any
replacement part has to satisfy. All in mm, as cut.

| feature | opening | mounting holes | fits |
|---|---|---|---|
| Display window | 31.93 × 16.99 | 4 × ⌀2.85 on **31.0 × 29.0** | SH1106 module |
| Air quality sensor | ⌀19.92 round, board footprint 19.0 × 31.0 | 4 × ⌀2.85 on **15.0 × 27.6** | MQ135 can + its breakout |
| Temp / humidity | 16.50 × 13.20 | 1 × ⌀2.85 alongside | DHT11 body is 15.5 × 12, so ~0.6 mm all round |
| Power switch | 19.52 × 12.54 | — | KCD1-101 rocker |
| USB-C | 12.50 × 7.00 | — | sized for the plug's plastic shell, not just the receptacle |

⌀2.85 as drawn cuts closer to ⌀3.0 after kerf — sized for M2.5 with clearance, or a
tight M3.

All of these were fitted by hand against the real parts and carry **no kerf
compensation** — do not "correct" them. See [`../case/CLAUDE.md`](../case/CLAUDE.md).

The 19.0 × 31.0 board footprint is a placement marker rather than an opening; it is
drawn in red, which is the ignore layer.
