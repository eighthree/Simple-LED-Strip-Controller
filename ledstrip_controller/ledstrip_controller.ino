/*
  LED Strip Controller 2.0
  Author: Timothy Garcia 
  Date: September 2017

  Description:
  Control an LED Light Strip via a web browser
  Allow configuration of device wirelessly through an Access Point (AP)
  Supports Homekit via Homebridge

  Extra Credits:
  Adafruit Industries www.adafruit.com
  Led Strip Animations: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
  ESP8266 wifi based on examples by Nuno Santos: https://techtutorialsx.com
  Brian L. https://www.youtube.com/user/witnessmenow
  Embedded Icons: https://material.io/icons/
  JSColor: jscolor-2.0.4 http://jscolor.com/
  
  License: This code is public domain. You can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation.  <http://www.gnu.org/licenses/>.
  Certain libraries used may be under a different license.
*/

/*-----------------------*/

/* File System/Hardware */
#include "FS.h"
#include <DoubleResetDetector.h>
#include <WS2812FX.h>

const char *index_html = 
#include "index.html.h"
;

/* Network Related Libraries */
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/* Time Related */
#include <time.h>

int timezone = 3;
int dst = 0;

/* Sensors / Optional */
#include <DHT.h>
#include <DHT_U.h>

/*-----------------------*/

/* Sensor */
#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* LED Strip */
#define LED_PIN D4
#define LED_COUNT 120
#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_SPEED 200
#define DEFAULT_MODE FX_MODE_STATIC
#define BRIGHTNESS_STEP 15 // in/decrease brightness by this amount per click              
#define SPEED_STEP 10 // in/decrease brightness by this amount per click                   

/* Double Reset */
#define DRD_TIMEOUT 10 // Number of seconds after reset during which a subseqent reset will be considered a double reset.         
#define DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use
           
// AP Setup
#define PORTAL_AP_NAME "VULPEioWemoD1Uno" // Set Portal Name
#define PORTAL_AP_PW "password" // Set Portal Password

/*-----------------------*/

float humidity, temperature, farenheight, heatf, heatc;

// For LED animations that loop/cycle
uint8_t LED_STATE = 0;

// For Custom RGB Values
// Used by handleGenericArgs
uint8_t R_LED = 0;
uint8_t G_LED = 0;
uint8_t B_LED = 0;

String modes = "";

/*-----------------------*/

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);      // ESP8266 Web Server Port: Default 80
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {

  /*-----------------------*/
  Serial.begin(115200);
  Serial.println("\nLED Strip Controller 2.0");
  
  /* ws2812fx setup */
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();
  
  /*-----------------------*/
  // Check for double reset flag on setup
  if (drd.detectDoubleReset()) {
    Serial.println("Double Reset Detected");
    digitalWrite(LED_BUILTIN, LOW);

    // Stop serving port 80
    server.stop();
  
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setTimeout(120);
    
    if (!wifiManager.startConfigPortal(PORTAL_AP_NAME, PORTAL_AP_PW)) {
      Serial.println("Failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

  } else {
      Serial.println("No Double Reset Detected");
  }

  if (!MDNS.begin("deskstrip")) {
  Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  /* Start Services */
  
  server.begin();  //Start the server
  SPIFFS.begin();


  /* Associate the handler function to the path */
   
  server.on("/", srv_handle_index_html);
  server.on("/set", srv_handle_set);

  /* Serve Javascript with Expires Headers */
  server.serveStatic("/jscolor.min.js",  SPIFFS, "/jscolor.min.js" ,"max-age=86400"); 
  server.serveStatic("/styles.min.css",  SPIFFS, "/styles.min.css" ,"max-age=86400"); 
  server.serveStatic("/script.min.js",  SPIFFS, "/script.min.js" ,"max-age=86400"); 
  
  /* 
  Custom RGB controller
  Built for use with something like this:
  https://github.com/metbosch/homebridge-http-rgb-bulb 
  */
  server.on("/rgb", handleGenericArgs); //Associate the handler function to the path
  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTP: Brightness UP");

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  delay(1000);
  Serial.println("\nSet Up Complete! Thank you!");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  ws2812fx.service();
  server.handleClient();  
  drd.loop();
}


/* Callback for AP Setup */
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  Serial.println(WiFi.softAPIP());
  drd.stop();
}

void srv_handle_index_html() {
  server.send_P(200,"text/html", index_html);
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  Serial.println("HTTP: Index Served");
}

/* 
 Sets custom colours
 Gets rgb?color=0x values
 Provides rgb?format=hex
 for homebridge support https://github.com/metbosch/homebridge-http-rgb-bulb
*/

void handleGenericArgs() { 
long currentRGB =(R_LED << 16L) | (G_LED << 8) | B_LED;
String currentHEX = String(currentRGB, HEX); 
          
  for (int i = 0; i < server.args(); i++) {
    if(server.argName(i) == "color")
    {
      String hex_raw = server.arg(i);
      long  rgb_value = strtol(&hex_raw[2], NULL, 16);
      
      // Split them up into r, g, b values
      long r = rgb_value >> 16;
      long g = rgb_value >> 8 & 0xFF;
      long b = rgb_value & 0xFF;

      R_LED = r;
      G_LED = g;
      B_LED = b;
      LED_STATE = 255;
      ws2812fx.setMode(FX_MODE_STATIC);
      ws2812fx.setColor((uint32_t) rgb_value);
      server.send(200, "text/plain", currentHEX);   
      Serial.println("HTTP: Color Request");
    }
    
    if(server.argName(i) == "format")
    {
        if(server.arg(i) == "hex") {
          server.send(200, "text/plain", currentHEX);
          Serial.println("HTTP: Send HEX Data:" + String(currentHEX));      
        }
    }
    
  } // end for
  
}

/*
  The MIT License (MIT)
  Copyright (c) 2016  Harm Aldick 
*/
void srv_handle_set() {
  time_t now = time(nullptr);
  Serial.println("srv_handle_set");
  Serial.println(ctime(&now));
  
  for (uint8_t i=0; i < server.args(); i++){
    if(server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(&server.arg(i)[0], NULL, 16);
      if(tmp >= 0x000000 && tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
        Serial.println("HTTP: TMP Set: " + String(tmp));
      }
    }

    if(server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(&server.arg(i)[0], NULL, 10);
      ws2812fx.setMode(tmp % ws2812fx.getModeCount());
      Serial.println("HTTP: Mode Set");
    }

    if(server.argName(i) == "b") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.decreaseBrightness(BRIGHTNESS_STEP);
        Serial.println("HTTP: Brightness DOWN");
      } else {
        ws2812fx.increaseBrightness(BRIGHTNESS_STEP);
        Serial.println("HTTP: Brightness UP");
      }
    }

    if(server.argName(i) == "s") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.decreaseSpeed(SPEED_STEP);
        Serial.println("HTTP: Speed DOWN");
      } else {
        ws2812fx.increaseSpeed(SPEED_STEP);
        Serial.println("HTTP: Speed UP");
      }
    }
  }
  server.send(200, "text/plain", "OK");
  Serial.println("HTTP: SEND 200 OK");
}


