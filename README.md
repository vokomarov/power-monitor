# Power Monitor

Firmware for ESP32-C3 SuperMini development board that reads `GPIO5` pin which signals about availability of electricity at your home.

Board is connected and powered on [UPS-DC-36W using HX12500-9Y connector](https://www.youtube.com/watch?v=heOfX0curtU&t=234s). UPS pins to ESP32 pins:
- `G` to `GND`
- `V` to `3V3`
- `GN` to `GPIO5` (inverted)

`GN` pin sends inverted signal, means it set 3.3 volts when no power grid electricity, and send zero volts when electricity is available.

## Setup

- Copy `secrets.h.example` to `secrets.h`
- Configure WiFi and Telegram credentials in `secrets.h`
- Figure out IP address of your board once connected (or assign IP to board MAC statically on your router which is recommended)

### Home Assistant

In order to enable automation based on power availability Power Monitor can report `binary_sensor` to your Home Assistant.

Home Assistant Webhook API is used. First please configure your trigger and sensor by adding following configuration to Home Assistant's `configuration.yml`:

```yml
template:
  - trigger:
      - trigger: webhook
        webhook_id: {{ your-secret-webhook-ID }}
    binary_sensor:
      - name: "Power Monitor"
        unique_id: "binary_sensor.power_monitor"
        state: "{{ trigger.json.available }}"
        device_class: power
```

Don't forget to reboot Home Assistant instance.

Test it by using proper values from the config.

```
curl --location 'http://homeassistant.local:8123/api/webhook/{{ your-secret-webhook-ID }}' \
	--header 'Content-Type: application/json' \
	--data '{"available":true}'
```

You should see updates in your Home Assistant Settings / Devices / Entites and search for "Power Monitor".

Now configure `secrets.h` accordingly and flash updated firmware.

## Development

### USB Serial

Comment line with `monitor_port` and add next config to `platformio.ini` to enable on boot USB serial:

```ini
monitor_rts = 0
monitor_dtr = 0
build_flags = 
	-DCORE_DEBUG_LEVEL=5
	-DARDUINO_USB_MODE=1
	-DARDUINO_USB_CDC_ON_BOOT=1
```

### Remote Serial

Serial over Telnet can be enabled by installing TelnetSerial package. Next line should be added to `platformio.ini` (based on IP of your board):

```ini
monitor_port = socket://192.168.50.100:23
```

Then `TelnetSerial.print` output will be accessible over the network:

```bash
$ nc 192.168.0.10 23
```

### OTA

Firmware upgrade (OTA) enabled by default using PlatformIO, keep next configs with IP address of your board

```ini
upload_protocol = espota
upload_port = 192.168.0.10
```

If you messed up with OTA codebase and it is no longer working - comment lines above and flash firmware via USB.

## Credits

Thanks to [@Akceptor](https://github.com/Akceptor) for the original idea: https://github.com/Akceptor/esp32/blob/main/ElectricityMonitor.ino
Thanks to [@BootuzDinamontuz](https://www.youtube.com/@BootuzDinamontuz) for the UPS-DC-36W pinout: https://www.youtube.com/watch?v=heOfX0curtU
