# Simple LED Strip Controller 1.2.5

*  Control an LED light strip via a web browser
*  Allow configuration of device wifi settings via AP
*  Serves a light browser-based controller with simple CSS for functionality
*  Hardware mode switch between colour settings
*  Supports Homekit via Homebridge

![led-page-125](https://user-images.githubusercontent.com/1100950/30235717-a35707d8-94c0-11e7-8738-b49d1f1450ab.PNG)

**[Status](#status)** |
**[Hardware Used](#hardware-used)** |
**[Libraries](#libraries)** |
**[HomeKit](#homekit)** |
**[License](#license)**

## Status

This project is in progress.

## Hardware Used
#### Requirements
* 1x NodeMCU 1.0 (ESP8266) _*_
* 1x WS2812b/SK6812 LED Strip or "_Neopixel_" LED Strip

_* You can use any Wifi-capable Arduino, your mileage may vary_

#### Optional Components
* 1x 10k Ω resistor
* 1x 470 Ω resistor
* 1x Momentary Switch (for switching modes)
* 1x 3-pin male header (for LED connections)

## Libraries
* ESP8266WiFi, https://github.com/esp8266/Arduino
* DNSServer, https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer
* WiFiManager, https://github.com/tzapu/WiFiManager
* Adafruit_NeoPixel.h, https://github.com/adafruit/Adafruit_NeoPixel _*_
* ESP8266WebServer.h, https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
* DoubleResetDetector.h>, https://github.com/datacute/DoubleResetDetector/
* Google Material Icons, https://material.io/icons/
* JSColor 2.0.4 http://jscolor.com/

## HomeKit
Install https://github.com/nfarina/homebridge on a Raspberry Pi or similar and
install this accessory https://github.com/metbosch/homebridge-http-rgb-bulb

_* FastLED should work as well but is not tested._

## License
This code is public domain. You can redistribute it and/or modify it under the terms of the
GNU Lesser General Public License as published by the Free Software Foundation.  <http://www.gnu.org/licenses/>. Certain libraries used may be under a different license.
