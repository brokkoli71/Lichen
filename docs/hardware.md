# Hardware

ESP32-based sensor unit measuring **CO2, humidity and temperature**, showing them on a
small OLED and serving them over HTTP.

## Bill of materials

| part | detail | status |
|---|---|---|
| MCU board | ESP32 — exact variant not yet recorded | ⬜ fill in |
| Display | 1.3" OLED, 128 × 64, I2C, **SH1106**, blue — [arduino-projekte.info](https://arduino-projekte.info/products/1-3-oled-display-128x64-i2c-sh1106-blue?variant=50481301750024) | ✅ |
| CO2 sensor | see measured footprint below | ⬜ fill in |
| Temperature / humidity sensor | see measured footprint below | ⬜ fill in |
| Button | one, front panel | ⬜ fill in |
| Case | 4 mm plywood, lasercut — see [`../case/`](../case/) | ✅ |

The case has **separate `Co2` and `temp` openings**, so temperature/humidity appears to
be a second sensor rather than being read from the CO2 part — worth confirming when the
firmware lands, since several CO2 sensors (SCD30, SCD4x) report all three.

## Openings in the case

Measured from `case/cut/lichen_sept_9_v4.svg` — these are the physical constraints any
replacement part has to satisfy. All in mm, as cut.

| feature | opening | mounting holes |
|---|---|---|
| Display window | 31.93 × 16.99 | 4 × ⌀2.85, on a **31.0 × 29.0** rectangle |
| CO2 sensor | ⌀19.92 round opening, board footprint 19.0 × 31.0 | 4 × ⌀2.85, on a **15.0 × 27.6** rectangle |
| Temp / humidity | 16.50 × 13.20 | 1 × ⌀2.85 alongside |
| Button | 19.52 × 12.54 | — |
| USB-C | 12.50 × 7.00 | — |
| unassigned | — | 3 × ⌀4.74 on the back panel |

⌀2.85 as drawn cuts closer to ⌀3.0 after kerf — sized for M2.5 with clearance, or a
tight M3. The three ⌀4.74 holes are not identified; if you know what they are, note it
here.

All of these were fitted by hand against the real parts and carry **no kerf
compensation** — do not "correct" them. See [`../case/CLAUDE.md`](../case/CLAUDE.md).

> ⚠️ The 19.0 × 31.0 CO2 board footprint is drawn in **red** and is a marker, not a
> cutout. If red maps to cut on your laser it puts a hole through the back panel.
