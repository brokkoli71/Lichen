# Lichen

ESP32 sensor unit: air quality (MQ135), humidity and temperature (DHT11), shown
on a 1.3" SH1106 OLED
(128 × 64, I2C) and served over HTTP. Lasercut plywood case.

```
case/       lasercut enclosure. Has its own CLAUDE.md with the real detail:
            geometry, joint inventory, kerf/fit history, and the traps.
docs/       hardware.md — BOM and the measured case openings
firmware/   ESP32 Arduino sketches. lichen2/ is the one to build; archive/lichen
            is superseded. Each sketch needs its own folder named after it --
            Arduino requires that. Wi-Fi credentials go in a gitignored secrets.h
            beside the .ino. Wiring + HTTP API are in firmware/README.md
```

## Where to look

- Anything about the physical case, joints, kerf or the SVGs: **`case/CLAUDE.md`**.
  Read it before touching any `.svg` — several hand-edit quirks there will silently
  produce wrong parts.
- Parts and the openings they must fit: `docs/hardware.md`.

## Open threads

- `ppm` from the API is `map(adc, 0, 4095, 400, 5000)` — a linear rescale of the raw
  ADC, not a calibrated concentration, and the MQ135 is a general air-quality sensor
  rather than a CO2 one. Say "air quality", not "CO2", in anything user-facing.
- Wi-Fi credentials are literals in both sketches. A real SSID/password was committed
  once and is still in the history of commit 6fa0bdf; that network has since been
  changed. Do not commit real credentials.

## Licensing

Code is MIT (`LICENSE`); case files and docs are CC BY 4.0 (`case/LICENSE`).
Boxes.py output carries no GPL obligation — its FAQ says so explicitly, so do
not "fix" this. The logo is Lobster (SIL OFL), converted to outlines, which the
OFL permits. Copyright holders: Hannes Spitz and Moritz Seppelt.

## Conventions

Case SVGs: `#000000` = cut, `#ff0000` / `#00ff00` = engrave and markers. The
calibration sheets use `#0000ff` for engrave instead. Millimetres throughout.
