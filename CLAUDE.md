# Lichen

ESP32 sensor unit: CO2, humidity and temperature, shown on a 1.3" SH1106 OLED
(128 × 64, I2C) and served over HTTP. Lasercut plywood case.

```
case/       lasercut enclosure. Has its own CLAUDE.md with the real detail:
            geometry, joint inventory, kerf/fit history, and the traps.
docs/       hardware.md — BOM and the measured case openings
firmware/   ESP32 code, NOT YET IN THE REPO (on another machine)
```

## Where to look

- Anything about the physical case, joints, kerf or the SVGs: **`case/CLAUDE.md`**.
  Read it before touching any `.svg` — several hand-edit quirks there will silently
  produce wrong parts.
- Parts and the openings they must fit: `docs/hardware.md`.

## Open threads

- Firmware is pending. When it arrives, document the HTTP API and the wiring, and fill
  the gaps in `docs/hardware.md` (CO2 sensor model, temp/humidity sensor model, ESP32
  variant, button part) from the source rather than guessing.
- The case has separate `Co2` and `temp` openings, implying two sensors rather than one
  combined part. Confirm against the firmware.
- Three ⌀4.74 mm holes on the back panel are unidentified.
- Nobody has verified the living hinge bends to r50 without cracking.

## Licensing

Code is MIT (`LICENSE`); case files and docs are CC BY 4.0 (`case/LICENSE`).
Boxes.py output carries no GPL obligation — its FAQ says so explicitly, so do
not "fix" this. The logo is Lobster (SIL OFL), converted to outlines, which the
OFL permits. Copyright holders: Hannes Spitz and Moritz Seppelt.

## Conventions

Case SVGs: `#000000` = cut, `#ff0000` / `#00ff00` = engrave and markers. The
calibration sheets use `#0000ff` for engrave instead. Millimetres throughout.
