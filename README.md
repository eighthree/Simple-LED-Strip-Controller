# Simple LED Strip Controller 2.0
*  Control an LED light strip via a web browser
*  Allow configuration of device wifi settings via AP
*  Serves a light browser-based controller with simple CSS for functionality
*  Supports Homekit via Homebridge
*  WS2812FX Library for led animations &amp; control

![Controller Dashboard](/screenshots/v2.PNG?raw=true "Version 2.0")

**[Status](#status)** |
**[Hardware](#hardware)** |
**[Software Setup](#software-setup)** |
**[HomeKit](#homekit)** |
**[Examples](#examples)** |
**[License](#license)**

## Status

#### 2.0
* Uses websockets
* Minimized SPIFF use, embeds minified CSS/JS
* New web interface, uses Google fonts and jQuery
* Exposes all WS2812FX library modes and stores via localStorage
* Backwards compatible with previous homebridge support
* 1.0x related random crashes and reboots should no longer happen, see: https://github.com/esp8266/Arduino/commit/3267443348a3b8f783dc2dd77f9db3a9a4aacc58


#### 1.x
* No longer supported, 2.0 is backwards compatible just make sure to update the configuration.

## Hardware
#### Requirements
* 1x ESP8266 _*_
* 1x WS2812b/SK6812 LED Strip or "_Neopixel_" LED Strip

#### Tested On
* NodeMCU 1.0
* Wemos D1 R2

#### Hardware Setup
Adafruit has written a detailed guide on NeoPixels along with
best practices and code examples: https://learn.adafruit.com/adafruit-neopixel-uberguide/overview

## Software Setup
1. Download and install Arduino 1.8.5 IDE and the libraries listed below using the library manager (Sketch -> Include Library -> Manage Libraries)
2. Perform 'ESP8266 Sketch Data Upload' to upload minified jscolor js library
3. Edit config.h
4. Compile and upload sketch (80/160MHz, LwIP Variant v1.4 Prebuilt)
5. Hit the reset button twice to enable wifi manager, connect to the new access point (default: ledstrip) and setup wifi for the device.
6. Open browser to http://ledstrip.local , the page will reload once the first time you visit it on any device. Modes buttons should appear on refresh.
7. Eat cake.

#### Libraries
* ESP8266WiFi, https://github.com/esp8266/Arduino
* DNSServer, https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer
* WiFiManager, https://github.com/tzapu/WiFiManager
* ESP8266WebServer.h, https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
* ESP8266mDNS.h, https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS
* ArduinoJSON.h, https://github.com/bblanchon/ArduinoJson
* DoubleResetDetector.h, https://github.com/datacute/DoubleResetDetector/
* WebSockets.h, https://github.com/Links2004/arduinoWebSockets
* Google Material Icons, https://material.io/icons/
* JSColor 2.0.4, http://jscolor.com/
* WS2812FX https://github.com/kitesurfer1404/WS2812FX
* Standard Libraries: time.h, FS.h

This project is a light weight LED Strip controller.
Want multi-client, MQTT & OTA support? Check out/support this excellent project:
* https://github.com/toblum/McLighting/


## HomeKit
Install https://github.com/nfarina/homebridge on a Raspberry Pi or similar and
install this accessory https://github.com/metbosch/homebridge-http-rgb-bulb

The address for the LED strip is ledstrip.local, you should probably change
this in config.h to something else if you are planning to have more than one installation.

#### Sample Homebridge Accessory Config

```bash
{
 "set_url": "http://ledstrip.local/rgb?color=0x%s",
 "get_url": "http://ledstrip.local/rgb?format=hex",
 "http_method": "GET",
 "manufacturer": "github.com/eighthree",
 "model": "Simple LED Strip",
 "serial": "VULPEio-001"
}
```

## Usage Examples
Expand functionality by using the LED strip as a visual notification tool.

#### Colour Sensor to LED Strip (Micropython)
_examples/rgb_sensor_example.py_
* Capture object RGB value and update LED strip via HTTP GET
* Tested on Micropython 1.9.2 running on a NodeMCU 1.0

#### MAX4466 "Washer/Dryer" Notification (Arduino)
_examples/mic_sensor_example/mic_sensor_example.ino_
* Sample ambient sound and update LED strip via HTTP GET when washer/dryer is done
* Tested on Arduino 1.8.3 running on a NodeMCU 1.0

## License
This code is public domain. You can redistribute it and/or modify it under the terms of the
GNU Lesser General Public License as published by the Free Software Foundation.  <http://www.gnu.org/licenses/>. Certain libraries used may be under a different license.
