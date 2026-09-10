# Lichen

A CO2, humidity and temperature sensor in a wooden case. Readings show on a small OLED
on the front, and are also served over HTTP so other things can pull them.

<p align="center"><img src="case/preview.png" width="720" alt="Lasercut sheet for the Lichen case"></p>

An ESP32 drives a 1.3" SH1106 OLED behind an arched plywood shell that bends around a
lattice living hinge. Four lasercut parts, 24 finger joints, one sheet of 4 mm ply.

## Status

| | |
|---|---|
| Case | ✅ cut and assembled; files calibrated against the machine — see [`case/`](case/) |
| Hardware / BOM | 🟡 display and board known, sensor models still to record — see [`docs/hardware.md`](docs/hardware.md) |
| Firmware | ⬜ not in the repo yet — see [`firmware/`](firmware/) |
| API | ⬜ to be documented once the firmware is added |

## Repo layout

```
case/       lasercut enclosure — cut files, calibration sheets, archive
docs/       hardware.md: bill of materials and the case openings
firmware/   ESP32 code (pending)
```

## Cutting the case

Two files in [`case/cut/`](case/cut/):

- **`lichen_sept_9_v4.svg`** — a complete case from scratch
- **`lichen_sept_9_v3_front_panel.svg`** — the display panel alone, to replace one panel
  against a shell already cut

They're calibrated differently on purpose. Read [`case/README.md`](case/README.md)
before cutting — in particular, **black is the only cut colour**, and the red rectangle
on the back panel is the CO2 board footprint, not a cutout.

Kerf compensation is machine-specific. [`case/calibration/`](case/calibration/) has a
test sheet that measures your kerf and board thickness and tells you what to use.

## Licensing

| what | licence |
|---|---|
| Code (`firmware/`, and any code added later) | [MIT](LICENSE) |
| Case design files and documentation (`case/`, `docs/`) | [CC BY 4.0](case/LICENSE) |

Both are permissive: use it, change it, build it, sell it — just keep the
attribution. Note that CC BY does not grant trademark rights, so the Lichen name
and logo are not licensed along with the files.

The case was generated with [Boxes.py](https://github.com/florianfesti/boxes)
(GPLv3) and then edited by hand. Boxes.py's own
[FAQ](https://florianfesti.github.io/boxes/html/faq.html) states that "the
resulting drawings do not fall under the GPL license", so these SVGs carry no GPL
obligation. The generator URL and parameters are preserved in each file's header.

The "Lichen" wordmark is set in [Lobster](https://fonts.google.com/specimen/Lobster)
(SIL Open Font License), converted to outlines.

