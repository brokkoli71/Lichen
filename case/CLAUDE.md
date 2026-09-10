# Lichen case — lasercut enclosure

Enclosure for the Lichen sensor unit (CO2, temperature, display, button, USB-C).
Arched box: a flat-topped semicircular front and back, wrapped by a single band
that bends around the arch with a lattice living hinge, closed by a bottom plate.

Everything here is **millimetres**, `viewBox` user units = mm.
This is the `case/` subdirectory of the Lichen repo; the firmware lives alongside
it. `README.md` here is the human-facing entry point, this file holds the detail.
Project-level orientation is in the repo-root `CLAUDE.md`.

```
cut/          the two current, ready-to-cut files
calibration/  fit-test sheets
archive/      superseded revisions and earlier design iterations
```

## Which file to cut

**`archive/lichen_sept_9_v3.svg`** is current. It is the calibrated one — see Fit below.

| file | notes |
|---|---|
| `cut/lichen_sept_9_v4.svg` | **for a full fresh cut.** 0.08 everywhere, bottom-plate tabs uncompensated |
| `cut/lichen_sept_9_v3_front_panel.svg` | **display panel alone, 0.09**, to replace a mis-cut panel against the existing shell |
| `archive/lichen_sept_9_v3.svg` | what the current physical parts were cut from: 0.09 width |
| `archive/lichen_sept_9_v2_burn0.1.svg` | superseded — burn 0.1 cracked the ply on assembly |
| `archive/lichen_april_16_v2.svg` | the hand-edited original, **no kerf compensation at all** |
| `archive/lichen_april_16.svg` | earlier revision |
| `lichen_april_16_v2_only_front_back*.svg` | panel-only subsets |
| `archive/lichen_sept_9_v3_display_holes_only.svg` | ~~overlay of just the 4 screw holes~~ **superseded** — re-registering the sheet was not accurate enough in practice |
| `calibration/lichen_fit_test_min.svg` | fit-test coupon sheet, minimal wording, shareable |
| `calibration/lichen_fit_test.svg` | first (larger, 3-finger) version of the same test |

## Origin

boxes.py `UBox`, then heavily hand-edited in Inkscape:

```
boxes UBox --x=70 --y=100 --h=100 --radius=50 --thickness=4 \
           --burn=0.0 --labels=0 --reference=0 --spacing=0.5
```

Do **not** regenerate from boxes.py — the sensor cutouts, logo and labels were
added by hand and would be lost. Edit the SVG.

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

## Fit calibration — read before changing any joint

History: original was `burn=0` (~0.3 mm slop, glue-only) → coupon test picked
0.1 → **0.1 cracked the ply during real assembly** → current v3.

Two directions behave differently, and this is the thing to remember:

```
width     (finger direction)  closes by  4*burn - 2*kerf    both parts move toward each other
thickness (board direction)   closes by  2*burn -   kerf    only the slot moves; the board is fixed
```

So the same burn is **tighter in thickness than in width**, and the thickness
direction has no compliance — a 4.00 mm board forced through a 3.95 mm slot splits.

v3 therefore compensates **width only**:

- tabs 8.18 / sockets 7.82 (burn 0.09)
- socket thickness dimension left at **4.00, uncompensated** → ~0.15 mm clearance
- notch depth and tab protrusion stay 4.00 — these are kerf-neutral and set flushness, not fit
- bottom plate body 70.0 → 69.80, for 0.2 mm entry clearance

A coupon that feels "too loose" with 2 tabs is usually fine as a glued 24-joint
box. Erring loose is much cheaper than erring tight.

### What actually worked (empirical, supersedes the coupon)

The combination that assembled successfully was **the old burn-0 bottom plate
together with 0.09 everywhere else**. So:

- the bottom plate does **not** want a tight fit — loose is fine there, it is
  captured on four sides and glued
- 0.09 was still marginally tight for a full recut

`cut/lichen_sept_9_v4.svg` is therefore built as:

| feature | burn | drawn |
|---|---|---|
| panel tabs / shell notches | 0.08 | 8.16 / 7.84 |
| the 16 finger holes (width) | 0.08 | 7.84 |
| **bottom plate tabs** | **0.00** | **8.00** (uncompensated) |
| thickness dimension | — | 4.00 (never compensated) |

Giving, at kerf 0.15: panel→shell +0.02 interference, plate→socket 0.14 mm
clearance (deliberately loose), thickness 0.15 mm clearance.

Independent of burn: **plate-to-panel-gap clearance = `8.0 - 2t`** (kerf cancels).
Zero at t = 4.00 by boxes.py's own geometry, negative above it. If the plate will
not enter, measure the ply before touching burn.

## Traps in these SVGs

- **Joint vs non-joint discriminator** (used by all the compensation scripts):
  joints are closed `<path>`s built only from 4 mm / 8 mm axis-aligned runs.
  Sensor cutouts are `<rect>` / `<circle>` / `<ellipse>` — never paths.
  The only path exceptions are `path4` (open polyline, flex slits) and
  `text4-9` (logo). This split is clean; rely on it.
- **Sensor cutouts were fitted by hand against real parts. Never kerf-compensate
  them.** They cut ~one kerf oversize by design.
- **Non-uniform group scales.** `rect5` (`display`) is drawn 28 × 19 but cuts at
  **31.93 × 16.99**; `rect5-1` (`anknopf`) drawn 21 × 12.5, cuts **19.52 × 12.54**.
  The Inkscape W/H dialog lies here. Trust the rendered mm. *Unresolved.*
- **`rect13-2` is red** (`#ff0000`) — the CO2 board footprint marker, 19 × 31.
  It must **not** be cut, or you get a hole through the back panel.
  Confirm red/green map to mark-or-ignore before every job. *Unconfirmed.*
- **Blue stroke conflict.** The 16 finger holes carry `stroke="#0000ff"` as an
  attribute while `style` says `#000000`. CSS wins, so they render black, but
  laser software may read the attribute and drop them onto the wrong layer.
  Fixed in v3; **still present in `archive/lichen_april_16_v2.svg` and both
  `only_front_back` files** (16 and 10 elements respectively).
- The 6 shell holes were 0.088 mm off the shell centreline from a stray Inkscape
  nudge. Fixed in v3 (recentred on 33 / 49 / 65).
- The CO2 cutout's 4 screw holes are positioned relative to the cutout. Any edit
  must preserve those offsets. Same for the display's 4 screw holes.
- **The display screw holes lived only in `_02`.** `archive/lichen_april_16_v2_only_front_back_02.svg`
  (13:59, newest of the April files) is the only place they existed; they were never
  merged back into v2, so v3 lacked them until they were added on 2026-09-09.
  That `_02` edit made exactly two changes vs `only_front_back.svg`: it added these
  4 holes **and moved the display cutout down 1.748 mm**. Nothing else moved.
  v3 has the holes but keeps v2's display position, because v3's display is what is
  physically on the already-cut sheet. **Open question: was that 1.748 mm move
  intentional and should v3 adopt it too?**

## Replacing a single panel

`cut/lichen_sept_9_v3_front_panel.svg` is the display panel on its own, on a
120 x 112 mm canvas. It is at **0.09, not 0.08** — deliberately, because it has to
mate with the shell that is already cut at 0.09. Use v4's burn only if the whole
set is being recut. It carries the 4 display screw holes.

## Re-cutting onto an already-cut sheet (did not work)

Re-registering an already-cut sheet was tried and abandoned — the alignment was
not repeatable enough, hence the whole-panel recut file above.
`archive/lichen_sept_9_v3_display_holes_only.svg` has the identical canvas
(`377.08 x 200`, `viewBox 0 0 377.08 200`) and identical absolute coordinates as
v3, so a sheet re-registered at the same origin lines up. Two layers:
`CUT` (the 4 holes, black) and `REFERENCE - DO NOT CUT` (panel outline + display
cutout, magenta dashed) for eyeballing alignment in the laser preview.
**Disable the reference layer before running** or it will re-cut the panel outline.

## Conventions

`#000000` = cut (34 elements in v3). `#ff0000` = labels + the CO2 marker.
`#00ff00` = logo. Hairline `stroke-width="0.05"`, `fill:none`, boxes.py style.

## Not verified

The living hinge bending to r50 without cracking has never been checked in this
work — `path4` has been left untouched throughout.
