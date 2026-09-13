# Lichen case

Lasercut enclosure for the Lichen sensor unit — MQ135 air quality sensor, DHT11
temperature/humidity sensor, OLED display, power switch, USB-C.

![Cut sheet](preview.png)

An arched box: flat-topped semicircular front and back panels, wrapped by a single
band that bends around the arch on a lattice living hinge, closed by a bottom plate.
Four parts, 24 finger joints, one 377 × 200 mm sheet of **4 mm plywood**.

## What to cut

| file | use |
|---|---|
| **`cut/lichen_sept_9_v4.svg`** | a complete case, from scratch |
| **`cut/lichen_sept_9_v3_front_panel.svg`** | the display panel on its own (120 × 112 mm), to replace one panel against a shell already cut at burn 0.09 |

Those two are calibrated differently on purpose — see *Fit* below. Everything in
`archive/` is superseded; don't cut from it.

### Before you press go

**Black is the only cut colour.** Red and green are engrave/marker layers — set them
to mark, or disable them.

> ⚠️ The red 19 × 31 mm rectangle on the back panel is the **CO2 board footprint, not a
> cutout**. If red is mapped to cut, you get a hole straight through the panel.

Two cutouts are distorted by non-uniform group scales left over from hand-editing:
`display` cuts at **31.93 × 16.99 mm** and `anknopf` at **19.52 × 12.54 mm**, which is
*not* what Inkscape's W/H boxes report. Trust the rendered millimetres. All sensor
cutouts were fitted by hand against real parts and carry no kerf compensation.

## Fit

Joint clearance is kerf-compensated; boxes.py calls the per-side amount `burn`.
The two directions do not behave the same:

```
width     (finger direction)  closes by  4*burn - 2*kerf   both parts move toward each other
thickness (board direction)   closes by  2*burn -   kerf   only the slot moves; the board is fixed
```

So the same burn is always tighter across the board's thickness, and that direction has
no give — a 4.00 mm board forced through a 3.95 mm slot splits rather than flexing.
The thickness dimension is therefore left uncompensated at 4.00 throughout.

`v4` uses:

| feature | burn | drawn |
|---|---|---|
| panel tabs / shell notches | 0.08 | 8.16 / 7.84 |
| the 16 finger holes (width) | 0.08 | 7.84 |
| bottom plate tabs | 0.00 | 8.00 — deliberately loose |
| thickness dimension | — | 4.00 |

The bottom plate is loose on purpose: it is captured on four sides and glued, and a
tight one cracked the ply on assembly.

## Calibrating for your machine

`calibration/lichen_fit_test_min.svg` (266 × 96 mm) sweeps burn 0.000 → 0.250 across
matched tab/socket pairs, and measures your kerf and real board thickness. Cut it in
the material you'll use for the case — kerf and thickness both move with the stock.

Note it uses **blue** for engraving, unlike the case files.

Treat its result as a starting point, not a verdict: two tabs pushed together by hand
feel firmer than the same fit does across 24 joints assembled at once. This design
landed a full step looser than the coupon suggested.

## Layout

```
cut/          the two current, ready-to-cut files
calibration/  fit-test sheets
archive/      superseded revisions and earlier design iterations
CLAUDE.md     detailed notes: geometry, joint inventory, quirks, fit history
preview.png   render of the full sheet
```

## Licence

[CC BY 4.0](LICENSE) — attribution required, everything else permitted. The
Lichen name and logo are not covered (CC BY does not license trademarks).

Generated with [Boxes.py](https://github.com/florianfesti/boxes) and hand-edited;
per its FAQ the generated drawings carry no GPL obligation.

