# Firmware

Not in the repo yet — currently on a second laptop, to be added.

Target board: ESP32. Expected responsibilities:

- read the CO2, humidity and temperature sensors
- drive the 1.3" SH1106 OLED (I2C)
- handle the front-panel button
- serve readings over HTTP as an API

See [`../docs/hardware.md`](../docs/hardware.md) for the parts and the case openings
they have to fit. Once the code is here, the API and wiring should be documented — the
hardware table has gaps that the source will settle.
