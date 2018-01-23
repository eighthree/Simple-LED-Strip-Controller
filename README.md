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

* This project is in progress.

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
* Perform 'ESP8266 Sketch Data Upload' for jscolor
* Compile and upload sketch

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


## HomeKit
Install https://github.com/nfarina/homebridge on a Raspberry Pi or similar and
install this accessory https://github.com/metbosch/homebridge-http-rgb-bulb

The address for the LED strip is deskstrip.local, you should probably change
this to something else if you are planning to have more than one installation.

### Sample Homebridge Accessory Config
`{
 "set_url": "http://deskstrip.local/rgb?color=0x%s",
 "get_url": "http://deskstrip.local/rgb?format=hex",
 "http_method": "GET",
 "manufacturer": "github.com/eighthree",
 "model": "Simple LED Strip",
 "serial": "VULPEio-001"
}`

## Examples
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
