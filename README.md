# Micro Controller Snake
Small game based on the idea of snake. Runs on the Heltec WifiKit 8 (easily adapted to another esp8266 based device, as long as there is a screen attached).

This code is based on the source written for the 2018 Area41 Security Conference Badge.

## Howto
### Preparation
Initialise the project.

Important:
- If you pulled this repo, already done, won't be needed anymore
- If you use another IDE, either remove or replace the --ide param

> pio project init --board heltec_wifi_kit_8 --ide clion

To use CLion as an IDE, or when things wont work as expected, (re-)run this command. Sometimes the IDE gets confused by PlatformIO and needs its config to be re-initialised.

> pio project init --ide clion

### PIN_IN

This code reads some registers to get the pin state of the buttons. You will need the following fix or you will get the following error:

> undefined reference to `PIN_IN'

Search for a file called "*eagle.app.v6.*.ld".

It usually is at:

```
/home/user/.platformio/packages/framework-arduinoespressif8266/tools/sdk/ld/eagle.app.v6.common.ld
```

Then add this line to that file, just besides another PROVIDE you find at the beginning:

```
PROVIDE(PIN_IN = 0x60000318);
```

Accessing PIN_IN in your source will give you access to the register with all the pins current states.


### Having fun
To update middleware

> pio update

To compile

> pio run

To upload to the device (attached via USB)

> pio run -t upload

To start the serial monitor

> pio device monitor

## What to look into after things compile and run
- 
- 