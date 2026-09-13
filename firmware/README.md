# Firmware

Arduino sketches for the ESP32. Sample the sensors, draw to the OLED, serve the
readings over HTTP.

| sketch | status |
|---|---|
| **`lichen2.ino`** | **current** — non-blocking loop, Wi-Fi state machine, debounced switch, scrolling graphs |
| `lichen.ino` | earlier, simpler version. Kept for reference; blocking loop, plain text readout, no graphs |

Both target the same wiring and expose the same endpoint, so either will run on the
hardware. New work goes in `lichen2.ino`.

## Wiring

| function | GPIO | notes |
|---|---|---|
| MQ135 analog out | **34** | ADC1, input-only pin. 12-bit, `ADC_11db` attenuation |
| DHT11 data | **27** | |
| OLED SDA | **23** | I2C |
| OLED SCL | **18** | I2C |
| Power switch | **25** | `INPUT_PULLUP` — closed to GND reads LOW = ON |

Any classic ESP32 board works; two different ones have been used. The pin numbers
assume classic ESP32 numbering (GPIO34 is input-only there), so an ESP32-C3/S2/S3
would need remapping.

## Wi-Fi

Credentials live in `secrets.h`, which is **gitignored**. Create it once:

```sh
cp secrets.h.example secrets.h
```

then fill in your network:

```c
const char* ssid     = "your ssid";
const char* password = "your password";
```

Both sketches `#include "secrets.h"`, so the build fails with a missing-header error
until you've made it — that's deliberate, it's the reminder. Never put real credentials
in a `.ino`; this is a public repo and a password committed once stays in the history.

`lichen2.ino` connects only while the switch is on, with a 15 s timeout, and shows a
status icon in the bottom-right of the display: dot only = radio off, animating arcs =
connecting, full arcs = connected, arcs with a slash = failed. A failed attempt retries
when the switch is cycled off and on again.

## HTTP API

One endpoint, on port 80:

```
GET /mq135  ->  application/json
```

```jsonc
{
  "current_adc":          2047,     // raw 12-bit ADC from the MQ135
  "current_ppm":          2700,     // see the caveat below
  "current_temperature":  21.4,     // °C
  "current_humidity":     48.0,     // % RH
  "wifi_state":           2,        // lichen2.ino only
  "queue": [                        // newest first
    { "adc": 2047, "ppm": 2700, "temperature": 21.4, "humidity": 48.0 }
  ]
}
```

`wifi_state`: `0` off, `1` connecting, `2` connected, `3` failed.

`queue` holds up to 128 samples taken every 5 s — a little under 11 minutes of history.
`lichen2.ino` returns only slots that contain real data; `lichen.ino` stops at the first
zero, which also truncates on a genuine zero reading.

> **`ppm` is not a calibrated concentration.** The conversion is
> `map(adc, 0, 4095, 400, 5000)` — a linear rescale of the raw ADC into the range
> 400–5000. It ignores the MQ135's logarithmic Rs/R0 response, is not calibrated against
> a reference, and applies no temperature or humidity compensation. Treat it as a
> relabelled raw reading that trends with air quality, not as ppm. `current_adc` is the
> honest number.

## Dependencies

Arduino IDE / arduino-cli with the ESP32 core, plus:

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) (and AsyncTCP)
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) (and Adafruit Unified Sensor)
- [U8g2](https://github.com/olikraus/u8g2)

`WiFi.h` and `Wire.h` come with the ESP32 core.

> **Note on folder layout.** The Arduino IDE expects one sketch per folder, named after
> it, and concatenates every `.ino` in a folder into a single compilation unit. Both
> sketches sitting in `firmware/` therefore won't build as-is — you'd get duplicate
> `setup()`/`loop()`. To build, copy the sketch you want into its own folder
> (`lichen2/lichen2.ino`) along with `secrets.h`.

## Behaviour

With the switch off, the display shows `Switch OFF`, Wi-Fi is disconnected and no
sampling happens. With it on, sensors are read every 5 s and the display cycles through
three graphs — temperature, PPM, humidity — 5 s each, autoscaled to the data on screen.
A failed DHT read carries the previous value forward rather than pushing a NaN, so the
three history queues stay aligned.
