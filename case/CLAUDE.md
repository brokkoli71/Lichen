# Lichen case — lasercut enclosure

Enclosure for the Lichen sensor unit: MQ135 air quality sensor, DHT11
temperature/humidity, OLED display, power switch, USB-C. Arched box — a flat-topped
semicircular front and back, wrapped by a single band that bends around the arch on a
lattice living hinge, closed by a bottom plate.

Everything here is **millimetres**, `viewBox` user units = mm. This is the `case/`
subdirectory of the Lichen repo; `README.md` is the human-facing entry point, this file
holds the detail. Project-level orientation is in the repo-root `CLAUDE.md`.

```
cut/          the sheet to cut
calibration/  fit-test sheets
archive/      superseded revisions and earlier design iterations
```

## Which file to cut

**`cut/lichen_sept_9_v4.svg`** — the only active file. 0.08 everywhere, bottom-plate
tabs uncompensated. Everything in `archive/` is superseded; the notable ones are
`lichen_sept_9_v3.svg` (what the existing physical parts were cut from, 0.09 width) and
`lichen_april_16_v2.svg` (the hand-edited original, no kerf compensation at all).

## Origin

boxes.py `UBox`, then heavily hand-edited in Inkscape:

```
boxes UBox --x=70 --y=100 --h=100 --radius=50 --thickness=4 \
           --burn=0.0 --labels=0 --reference=0 --spacing=0.5
```

Do **not** regenerate from boxes.py — the sensor cutouts, logo and labels were added by
hand and would be lost. Edit the SVG.

## Parts and assembly

| part | id | size | role |
|---|---|---|---|
| shell | `path3` + `path4` | 257.08 × 78 | wraps the arch; `path4` = flex slits |
| front panel | `path2` | 100 × 100, r50 | logo, `display` |
| back panel | `path2-2` | 100 × 100, r50 | `temp`, `Co2`, `usb c`, `anknopf` |
| bottom plate | `path14` | 100 × 70 | labelled `bottom` |

Shell length 257.08 = `50 + pi*50 + 50`, exactly the panel's U-perimeter.
Shell width 78 = `70 + 2*4`. Flex span 157.08 = `pi*50`.

**24 joints in 3 families** (8 mm fingers, 16 mm pitch, 4 mm material):

- plate → both panels: 10 tabs into 10 enclosed holes (`path8..12`, `path8-2..12-3`)
- plate → both shell walls: 6 tabs into 6 enclosed holes (`path19..21`, `path19-9..21-0`)
- panels → shell: 8 tabs into 8 edge notches on the shell's long edges

Tabs total 24, sockets total 24 — if that ever stops balancing, something is wrong.

## Fit — read before changing any joint

Two directions behave differently, and this is the thing to remember:

```
width     (finger direction)  closes by  4*burn - 2*kerf    both parts move toward each other
thickness (board direction)   closes by  2*burn -   kerf    only the slot moves; the board is fixed
```

So the same burn is **tighter in thickness than in width**, and the thickness direction
has no compliance — a 4.00 mm board forced through a 3.95 mm slot splits. Thickness is
therefore never compensated; it stays at 4.00.

Empirically, the bottom plate does **not** want a tight fit: it is captured on four
sides and glued, and a tight one cracked the ply during assembly. A coupon that feels
"too loose" with 2 tabs is usually fine across a glued 24-joint box, so erring loose is
much cheaper than erring tight.

`cut/lichen_sept_9_v4.svg`:

| feature | burn | drawn |
|---|---|---|
| panel tabs / shell notches | 0.08 | 8.16 / 7.84 |
| the 16 finger holes (width) | 0.08 | 7.84 |
| **bottom plate tabs** | **0.00** | **8.00** (uncompensated) |
| thickness dimension | — | 4.00 (never compensated) |

At kerf 0.15 that gives panel→shell +0.02 interference, plate→socket 0.14 mm clearance,
thickness 0.15 mm clearance. Notch depth and tab protrusion stay 4.00 — they are
kerf-neutral and set flushness, not fit.

Independent of burn: **plate-to-panel-gap clearance = `8.0 - 2t`** (kerf cancels). Zero
at t = 4.00 by boxes.py's own geometry, negative above it. If the plate will not enter,
measure the ply before touching burn.

## Traps in these SVGs

- **Joint vs non-joint discriminator** (used by all the compensation scripts): joints
  are closed `<path>`s built only from 4 mm / 8 mm axis-aligned runs. Sensor cutouts are
  `<rect>` / `<circle>` / `<ellipse>` — never paths. The only path exceptions are
  `path4` (open polyline, flex slits) and `text4-9` (logo). This split is clean; rely
  on it.
- **Sensor cutouts were fitted by hand against real parts. Never kerf-compensate
  them.** They cut ~one kerf oversize by design.
- **Non-uniform group scales.** `rect5` (`display`) is drawn 28 × 19 but cuts at
  **31.93 × 16.99**; `rect5-1` (`anknopf`) drawn 21 × 12.5, cuts **19.52 × 12.54**. The
  Inkscape W/H dialog lies here — trust the rendered mm. The cut sizes are the ones that
  were fitted by hand, so they are correct; only the dialog is misleading.
- **Blue stroke conflict.** The 16 finger holes carry `stroke="#0000ff"` as an attribute
  while `style` says `#000000`. CSS wins, so they render black, but laser software may
  read the attribute and drop them onto the wrong layer. Fixed from v3 onward; **still
  present in `archive/lichen_april_16_v2.svg` and both `only_front_back` files**.
- Screw holes are positioned relative to their cutout — the CO2 group and the display
  group both. Any edit must preserve those offsets.
- The display screw holes existed only in `archive/lichen_april_16_v2_only_front_back_02.svg`
  until they were merged in. That same edit also moved the display cutout down
  1.748 mm, which was **not** carried over, because the current display position is what
  is physically on the cut sheets. Open question whether that move was intentional.

## Living hinge

`path4` is the lattice hinge: 77 slit lines at 2 mm pitch across the 152 mm middle of
the shell, letting the flat 4 mm strip bend to r50. It has been cut and bent
successfully, so it works as drawn. It carries no kerf compensation and has been left
untouched through every revision — don't "fix" it.

## Conventions

`#000000` = cut. `#00ff00` = engrave, which is only the logo. `#ff0000` = ignore: the
working labels and the sensor board placement marker. Hairline `stroke-width="0.05"`,
`fill:none`, boxes.py style.
