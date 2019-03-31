# ESP8266-homie-PIR-DHT
DHT/PIR sensor on NodeMCU using Homie and PlatformIO

Currently uses Homeie v3.0.1 [Homie 3.0.1](https://github.com/homieiot/convention/releases/tag/v3.0.1)

Project is built in PlatformIO, see below. 

## Setup
### Using with PlatformIO

[PlatformIO](http://platformio.org) is an open source ecosystem for IoT development with cross platform build system, library manager and full support for Espressif ESP8266 development. It works on the popular host OS: Mac OS X, Windows, Linux 32/64, Linux ARM (like Raspberry Pi, BeagleBone, CubieBoard).

1. Install [PlatformIO IDE](http://platformio.org/platformio-ide)
2. Download Repo
3. Add repo PlatformIO
4. Make any changes you want
5. Upload to NodeMCU chip

### Upload Firmware directly
If you think my code looks good to go, you can download firmware.bin in "Builds" directory and upload that to your NodeMCU using something like espTool or NodeMCU Flasher. Google these.

### Configuration

1. Upload project to Node chip
2. Open this URL: https://homieiot.github.io/homie-esp8266/configurators/v2/
3. Connect to the device's generated WiFi network, should be called something like "Homie-123456abcdef".
4. Follow UI to configure.

## Other
### Issues
Currently OTA is not enabled. 

My code is trash :) I'm a rookie!

Some of this code has been borrowed from others and added to this conglomerate. Will credit them when I get chance. 

### openHAB
If using with openHAB v2.5 or above, once MQTT is configured, this device will automatically show up in Inbox to add. 