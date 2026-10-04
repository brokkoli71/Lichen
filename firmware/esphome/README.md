# Lichen on ESPHome

`lichen.yaml` is an [ESPHome](https://esphome.io) config for the same hardware and
wiring as `lichen2/` (see `../README.md`). It reports to Home Assistant over the native
API instead of serving `/mq135`, and shows the outside weather from HA on the OLED.

## What it does

**Sent to Home Assistant** every 10 s: `Temperature`, `Humidity` and `Air quality` (the
raw 12-bit MQ135 ADC reading, deliberately not converted to "ppm"). HA keeps
the history, so there's no on-device queue.

**OLED** shows the overview page. With the switch (GPIO25) on, it cycles every 5 s
through five pages instead:

1. Overview: inside temperature, humidity and air quality, then outside temperature
   with today's forecast max/min
2. Inside temperature, last hour
3. Humidity, last hour
4. Air quality, last hour
5. Outside temperature, last 24 h

The graphs autoscale. Their history lives in RAM, so they start empty after a reboot.
The 24 h outside graph takes a day to fill.

Top right is the connection icon: crossed out means no Wi-Fi, blinking means Wi-Fi is
up but Home Assistant isn't connected, and steady means connected to HA.

## 1. Install ESPHome

Pick one:

- **CLI on this machine** (used below): `uv tool install esphome` or `pipx install esphome`
- **Home Assistant add-on "ESPHome Device Builder"**: edit and flash from the HA web UI.
  Copy `lichen.yaml` in and put the secrets in its own `secrets.yaml` editor. The first
  flash happens over USB from Chrome or Edge.

## 2. Secrets

```sh
cd firmware/esphome
cp secrets.yaml.example secrets.yaml
openssl rand -base64 32        # paste as api_encryption_key
```

Fill in your Wi-Fi as well. `secrets.yaml` is gitignored, so never commit it.

## 3. First flash over USB

Linux needs permission to open the serial port. On Arch that means the `uucp` group,
once, followed by logging out and back in:

```sh
sudo usermod -aG uucp $USER
```

Plug the ESP32 in, then:

```sh
esphome run lichen.yaml
```

This compiles the firmware, asks which port to use (pick `/dev/ttyUSB0` or
`/dev/ttyACM0`) and then shows the logs. The first build downloads the toolchain and
takes a few minutes. If the upload hangs at `Connecting...`, hold the board's **BOOT**
button until it starts.

**Later flashes go over Wi-Fi.** Run the same command and pick `lichen.local` (or
the IP) instead of the serial port. The USB cable isn't needed again.

**`Auth Expired` in the log** means the router never heard the board's login. The
board receives the router's signal more easily than the router receives the board's,
because of the ESP32's small PCB antenna. Keep the antenna end clear of wires and
metal, or move the board closer to the router. It isn't a password problem: a wrong
password fails the same way.

## 4. Add it to Home Assistant

HA normally discovers it on its own: *Settings → Devices & services* shows a "Lichen"
discovered device. Otherwise add the **ESPHome** integration manually with host
`lichen.local`. Either way it asks for the `api_encryption_key`.

## 5. Outside weather

The current outside temperature comes from the `temperature` attribute of a weather
entity, by default `weather.forecast_home` (the Met.no integration HA sets up during
onboarding).

Weather entities no longer carry the forecast as attributes, so the daily max/min need
two template sensors in HA's `configuration.yaml`:

```yaml
template:
  - trigger:
      - trigger: time_pattern
        minutes: /30
      - trigger: homeassistant
        event: start
    action:
      - action: weather.get_forecasts
        data:
          type: daily
        target:
          entity_id: weather.forecast_home
        response_variable: daily
    sensor:
      - name: Forecast today max
        unique_id: forecast_today_max
        unit_of_measurement: "°C"
        device_class: temperature
        state: "{{ daily['weather.forecast_home'].forecast[0].temperature }}"
      - name: Forecast today min
        unique_id: forecast_today_min
        unit_of_measurement: "°C"
        device_class: temperature
        state: "{{ daily['weather.forecast_home'].forecast[0].templow }}"
```

Restart HA after adding this. If your entity names differ, change the `substitutions:`
at the top of `lichen.yaml`. Until a value arrives, the display shows `--`.
