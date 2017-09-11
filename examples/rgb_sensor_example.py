''' 
Simple LED Strip Controller 1.2.5 - Example Use #1: 
Micropython 1.9.2 - Colour Sensor to LED Strip
Date: September 2017
Description: Capture object RGB value and update LED strip via HTTP GET

Hardware Used:
NodeMCU 1.0 / Adafruit TCS34725 Colour Sensor
Pin Out: D1 <-> SCL, D2 <-> SDA, INT <-> D5, LED <-> D6

License: This code is public domain. You can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation.  <http://www.gnu.org/licenses/>.
Certain libraries used may be under a different license.
'''
import tcs34725 # https://github.com/adafruit/micropython-adafruit-tcs34725
import machine
import time
import network
import urequests
from machine import I2C, Pin
from tcs34725 import html_rgb, html_hex

# Setup
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect("SSID", "PASSWORD")

# TCS34725
i2c = I2C(-1, Pin(5), Pin(4))
sensor = tcs34725.TCS34725(i2c)
led = machine.Pin(12, machine.Pin.OUT)

led.off()

# Blink LED 2 times if network connects
if sta_if.isconnected():
    sta_if.ifconfig()
    led.on()
    time.sleep(0.5)
    led.off()
    time.sleep(0.5)
    led.on()
    time.sleep(0.5)
    led.off()

# Sensor Setup
sensor.active(True)
sensor.gain(16)
sensor.integration_time(402)
time.sleep_ms(500)

led.on()
sensor_rgb = html_hex(sensor.read(True))

# HTTP Request URL Setup
urequests.put("http://SIMPLE_LED_STRIP_IP/rgb?color=0x" + sensor_rgb)
led.off()
