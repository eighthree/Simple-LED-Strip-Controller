/*  Simple LED Strip Controller 1.2.0
 *  Author: Timothy Garcia (http://timothygarcia.ca)
 *  Date: September 2017
 *  
 *  Description:
 *  Control an LED light strip via a web browser
 *  Allow configuration of device wifi settings via AP
 *  Serves a light browser-based controller with simple CSS for functionality
 *  Hardware mode switch between colour settings
 *  
 *  Hardware Used:
 *  NodeMCU 1.0 / 10K Ohm Resistor / 470 Ohm Resistor / Momentary Switch / Protoype Board / 3-pin header (for LED)
 *  
 *  Extra Credits:
 *  Adafruit Industries www.adafruit.com
 *  Led Strip Animations: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 *  ESP8266 wifi based on examples by Nuno Santos: https://techtutorialsx.com
 *  LadyAda www.ladyada.net/learn/arduino/lesson5.html
 *  Brian L. https://www.youtube.com/user/witnessmenow
 *  Embedded Icons: https://material.io/icons/
 *  
 *  License: This code is public domain. You can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation.  <http://www.gnu.org/licenses/>.
 *  Certain libraries used may be under a different license.
*/ 

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>            //https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel
#include <ESP8266WebServer.h>     //https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
#include <DoubleResetDetector.h>  //https://github.com/datacute/DoubleResetDetector/

// Neopixel Setup
#define PIN D6
#define NUM_LEDS 29

// DRD Setup
#define DRD_TIMEOUT 10          // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0           // RTC Memory Address for the DoubleResetDetector to use

// AP Setup
#define PORTAL_AP_NAME "VULPEioNMCU1"     // Set Portal Name

// Mode Switch Setup
uint8_t TRIGGER_PIN = D1;           // Placeholder Trigger Pin
uint8_t val;                        // variable for reading the pin status
uint8_t val2;                       // variable for reading the delayed status
uint8_t buttonState;                // variable to hold the button state

// For LED animations that loop/cycle
uint8_t LED_STATE = 0;

char* LED_MODES[] ={"off","on","red","rainbow","cylon","sparkle","fire","sunset","theatre","blue","green","purple"};

// Timing Related to Colour
uint16_t rainbowColor;
unsigned long previousMillis = 0;
const long interval = 20;

// Initialise Neopixels, HTTP Server & DRD Reset Detector
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);      // ESP8266 Web Server Port: Default 80
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

// Callback for AP Setup
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  strip.setPixelColor(0, strip.Color(100, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 200, 0));
  strip.setPixelColor(2, strip.Color(100, 0, 200));
  strip.show();
  drd.stop();
}
void setup() {
  //Serial.begin(115200);
  //Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  strip.begin();              // Start neopixels
  strip.setBrightness(5);     // Set brightness for strip to 5, prevents drawing too much power.
  strip.show();               // Initialize all pixels to 'off'

  
  if (drd.detectDoubleReset()) { // Check for double reset flag on setup
      //Serial.println("Double Reset Detected");
    
      digitalWrite(LED_BUILTIN, LOW);
    
      //Serial.println("\n Configuration Portal Requested, Control Server Stopping");

      // Stop serving port 80
      server.stop();
    
      //WiFiManager
      //Local intialization. Once its business is done, there is no need to keep it around
      WiFiManager wifiManager;
      wifiManager.setAPCallback(configModeCallback);

      //sets timeout until configuration portal gets turned off
      //useful to make it all retry or go to sleep
      //in seconds
      wifiManager.setTimeout(120);
    
      //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
      WiFi.mode(WIFI_STA);
    
      if (!wifiManager.startConfigPortal(PORTAL_AP_NAME)) {
        Serial.println("Failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }

      //Serial.println("Connected to Wireless AP.");  // If you get here you have connected to the WiFi
      //Serial.println("IP address: ");
      //Serial.println(WiFi.localIP());               //Print the local IP

      // Restart Server
      server.begin();
  } else {
      //Serial.println("No Double Reset Detected");
  }

  server.begin();  //Start the server
  //Serial.println("Server listening");
  
  buttonState = digitalRead(TRIGGER_PIN);
  //Serial.println("Current button state on startup:");
  //Serial.println(buttonState);

  colorWipe(strip.Color(100, 100, 100), 50); // Set strip colour to white

  //Associate the handler function to the path
  server.on("/", [](){
    handleRootPath(LED_STATE);
  });   
  server.on("/on", ledON);
  server.on("/off", ledOFF);
  server.on("/rainbow", rainbowLight);
  server.on("/red", redLight);
  server.on("/cylon", cylonLight);
  server.on("/sparkle", sparkleLight);
  server.on("/fire", fireLight);
  server.on("/sunset", sunsetLight);
  server.on("/theatre", theatreLight);
  server.on("/blue", blueLight);
  server.on("/green", greenLight);
  server.on("/purple", purpleLight);
  
}


void loop() {

    // Mode Switcher
    // Based on Lady Ada's bike lights
    val = digitalRead(TRIGGER_PIN);      // read input value and store it in val
    delay(10);                         // 10 milliseconds is a good amount of time
    val2 = digitalRead(TRIGGER_PIN);     // read the input again to check for bounces
    
    if (val == val2) {                 // make sure we got 2 consistant readings!
      if (val != buttonState) {          // the button state has changed!
        if (val == HIGH) {                // check if the button is pressed

            switch(LED_STATE) {
              case 0:
                ledON();
                break;
              case 1:
                redLight();
                break;
              case 2:
                LED_STATE += 1;
                break;
              case 3:
                rainbowLight();
                LED_STATE += 1;
                break;
              case 4:
                cylonLight();
                LED_STATE += 1;
                break;
              case 5:
                sparkleLight();
                LED_STATE += 1;
                break;
              case 6:
                fireLight();
                LED_STATE += 1;
                break;
              case 7: {
                sunsetLight(); 
                LED_STATE += 1;
                break; }
              case 8:
                theatreLight();
                LED_STATE += 1;
                break;
              case 9:
                blueLight();
                LED_STATE += 1;
                break;
              case 10:
                greenLight();
                LED_STATE += 1;
                break;
              case 11:
                purpleLight();
                LED_STATE = 0;
                break;
             } // End Switch
             
          }
        }
      }
   
   buttonState = val;                 // save the new state in our variable
    
    // Debug Button State
    //Serial.println(buttonState);
    //Serial.println(LED_STATE);

    switch(LED_STATE) {
      case 1: {
          strip.setBrightness(10);
          colorWipe(strip.Color(100,100,100), 50);
          break;
      }
      case 2: {
          strip.setBrightness(5);
          colorWipe(strip.Color(255, 0, 0), 50);
          break;
      }
      case 3: {
          unsigned long currentMillis = millis();
            if(currentMillis - previousMillis >= interval) {
              previousMillis = currentMillis;   
               rainbow(20); 
            }              
          break; }
      case 4:
          CylonBounce(0xff, 0, 0, 5, 10, 50);
          break;
      case 5:
          SnowSparkle(0x10, 0x10, 0x10, 20, 200);
          break;
      case 6:
          Fire(55,120,15);
          break;
      case 7: {
          strip.setBrightness(8);
          colorWipe(strip.Color(255, 108, 0), 50);
           break;
      }
      case 8: {
          theaterChase(strip.Color(127, 127, 127), 50);
          break;
      }
      case 9: {
          strip.setBrightness(15);
          colorWipe(strip.Color(0, 0, 255), 50);
           break;
      }
      case 10: {
          strip.setBrightness(20);
          colorWipe(strip.Color(0, 255, 0), 50);
           break;
      }
      case 11: {
          strip.setBrightness(20);
          colorWipe(strip.Color(80, 0, 200), 50);
          break;
      }
      case 0: {
          strip.setBrightness(5);
          colorWipe(strip.Color(0, 0, 0), 50);
          break;
      }
    }
    
    server.handleClient();  
  
    // Call the double reset detector loop method every so often,
    // so that it can recognise when the timeout expires.
    // You can also call drd.stop() when you wish to no longer
    // consider the next reset as a double reset.
    drd.loop();

}


// Server Control Paths
void handleRootPath(uint8_t activeButton) {           
  String content = "<html><head><title>Desk Light Strip</title>"; 
  
  // Base 64 encoded favicon
  content += "<link rel=\"shortcut icon\" href=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACQAAAAkCAYAAADhAJiYAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZiS0dEAAAAAAAA+UO7fwAAAAlwSFlzAAAASAAAAEgARslrPgAAAeBJREFUWMPt179qFFEUBvCfGq1iI0bU3Yg2guAbKGhs1Sj49wEk2Jg+3XZqaadgJT6BsNgYxGUD+ggqWOiagF20M+6uxcyQ48JsZmcGArIfDHzLPfe73zlzz72zTDFFNeypQeM8zqV8Dd3dTqqFYfq0qort3e1s/itDN7FSYo0V3Kjb+H0MJPvk0chYS/4eysYGWK7LzN1gZoivOFrA0HF8D2MD3KpqZh6/gmgXRyao0DG8D+M/0Ry34E576DFmU97DdfyYIKENLGI9/X0QD8tW5yT+hOwu5MSNq1CGhRCzhRNlKnQb+1L+Ae/KZoa3WE35TKo9saFLgT+rYCbDyxztwobOBt6pwdBajnZhQ4cD79Vg6Fvgc2UMbQW+vwZDBwL/nRc0M0ZgHadT3sDHnLiu5HjIeB4agW+Uyei17Va9V0OFloJeOy9o3CuLk+7UYChqtMsIzCt2MBbB6MHYLCv0PAh98W/nFcUhfA46TyskpiG5EDOxTrrAJGY6Yf6m5MKthEX0g+gnXCwwb2GkMn1crWomw/KIqSHeSDrnjOSLYDblS5J7K8b28aAuM7FS8fUVfTZxpW4zGebwRNIpOxnp44UJ90zZP4pNXMNlnLLdxj1JN7bxSj134BRTTFEr/gJjkZYrYDLmgAAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxNy0wOS0wN1QwMjoxMTo1MyswMDowMAsCEWsAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTctMDktMDdUMDI6MTE6NTMrMDA6MDB6X6nXAAAAKHRFWHRzdmc6YmFzZS11cmkAZmlsZTovLy90bXAvbWFnaWNrLTdmN3p1bmtNdOrSigAAAABJRU5ErkJggg==\" />";
  content += "</head>";

  // Stylesheet
  content += "<style type=\"text/css\">body{font-family:verdana,sans-serif;background-color: #000000; background-image: linear-gradient(6deg, #000000 0%, #3e3e3e 100%);}.footnote{color:#fff; font-size: 1em;}svg {fill:#000;}div a:hover svg { fill: #fff; }a:active,a:focus, {outline:0;border:none;-moz-outline-style: none; }.power{display:flex; width: 100%; flex-wrap:wrap; margin-left:-5px;}.controller{display:flex;width:100%;flex-wrap:wrap;margin-left:-5px;}a{display:block;width:49%;margin-left:10px;margin-bottom:10px;flex-grow:2;}a button {transition: all 1s ease;} input[type=\"button\"]{ outline:none; } input[type=\"button\"]::-moz-focus-inner { border: 0; } a button:hover,a button:active {-webkit-box-shadow: 0px 0px 20px 0px rgba(255,255,255,1);-moz-box-shadow: 0px 0px 20px 0px rgba(255,255,255,1);box-shadow: 0px 0px 20px 0px rgba(255,255,255,1);} a button{ text-transform: capitalize; background-color: #6b6b6b;background-image: linear-gradient(6deg, #e2e2e2 0%, #ffffff 100%);font-weight:bold;font-size:3em;width:100%;height:25%;border:0px;border-radius: 15px;}button:hover{color:#fff; background-color: #caffff; background-image: linear-gradient(180deg, #caffff 0%, #93d8d8 100%);cursor:hand;}";
  content += "#on:active, #on:hover, .on { color:#fff; background-color: #ffcd6a; background-image: linear-gradient(0deg, #ffcd6a 0%, #fffcde 100%); }";
  content += "#off:active, #off:hover, .off { color:#fff; background-color: #212121; background-image: linear-gradient(135deg, #212121 0%, #7a7a7a 100%); color:#fff;}";
  content += "#red:active, #red:hover, .red { color:#fff; background-color: #ff5959; background-image: linear-gradient(45deg, #ff5959 0%, #bb0003 100%); }";
  content += "#blue:active, #blue:hover, .blue { color:#fff; background-color: #21D4FD; background-image: linear-gradient(19deg, #21D4FD 0%, #7021ff 100%); }";
  content += "#green:active, #green:hover, .green { color:#fff; background-color: #7cc000; background-image: linear-gradient(19deg, #7cc000 0%, #00bd5d 100%); }";
  content += "#purple:active, #purple:hover, .purple { color:#fff; background-color: #5f03d2; background-image: linear-gradient(19deg, #5f03d2 0%, #B721FF 100%); }";
  content += "#rainbow:active, #rainbow:hover, .rainbow { color:#fff; background-color: #fd4000; background-image: linear-gradient(0deg, #fd4000 0%, #fdd90a 27%, #2AF598 51%, #00e9e7 71%, #718bef 100%); }";
  content += "#cylon:active, #cylon:hover, .cylon { color:#fff; background-color: #5b0000; background-image: linear-gradient(90deg, #5b0000 0%, #ff0004 50%, #5f0000 100%); }";
  content += "#sparkle:active, #sparkle:hover, .sparkle { color:#fff; background-color: #6f6f6f; background-image: linear-gradient(19deg, #6f6f6f 0%, #ffffff 100%); }";
  content += "#fire:active, #fire:hover, .fire { color:#fff; background-color: #fff8f8; background-image: linear-gradient(147deg, #fff8f8 0%, #FFE53B 23%, #FF2525 100%); }";
  content += "#sunset:active, #sunset:hover, .sunset { color:#fff; background-color: #FBAB7E; background-image: linear-gradient(322deg, #FBAB7E 0%, #F7CE68 100%); }";
  content += "#theatre:active, #theatre:hover, .theatre { color:#fff; background-color: #960000; background-image: linear-gradient(180deg, #960000 19%, #ff0000 79%, #000000 100%); }</style>";
  content += "<body>";

  if (server.hasHeader("User-Agent")){
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }

  // Power DIV, Separate from the rest and static
  // We want to keep the power related buttons static so they're
  // in a separate div
  content += "<div class=\"power\">";
  if(activeButton == 1) {
    content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[1]) +  "\")'\"><button class=\"" + String(LED_MODES[1]) + "\" id=\"" + String(LED_MODES[1]) +  "\">";
  } else {
    content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[1]) +  "\")'\"><button id=\"" + String(LED_MODES[1]) +  "\">";
  }
  content += "<svg style=\"width:25%;height:25%\" viewBox=\"0 0 24 24\"><path d=\"M11,0V4H13V0H11M18.3,2.29L15.24,5.29L16.64,6.71L19.7,3.71L18.3,2.29M5.71,2.29L4.29,3.71L7.29,6.71L8.71,5.29L5.71,2.29M12,6A4,4 0 0,0 8,10V16H6V18H9V23H11V18H13V23H15V18H18V16H16V10A4,4 0 0,0 12,6M2,9V11H6V9H2M18,9V11H22V9H18Z\" /></svg>";
  content += "</button></a>";

  if(activeButton == 0) {
    content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[0]) +  "\")'\"><button class=\"" + String(LED_MODES[0]) + "\" id=\"" + String(LED_MODES[0]) +  "\">";
  } else {
    content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[0]) +  "\")'\"><button id=\"" + String(LED_MODES[0]) +  "\">";
  }
  content += "<svg style=\"width:25%;height:25%\" viewBox=\"0 0 24 24\"><path d=\"M12,3C10.05,3 8.43,4.4 8.08,6.25L16.82,15H18V13H16V7A4,4 0 0,0 12,3M3.28,4L2,5.27L8,11.27V13H6V15H9V21H11V15H11.73L13,16.27V21H15V18.27L18.73,22L20,20.72L15,15.72L8,8.72L3.28,4Z\" /></svg>";
  content += "</button></a>";
  content += "</div>";

  // Controller DIV
  content += "<div class=\"controller\">";

  // Create buttons dynamically and add a 'class'
  // This reflects the current LED_STATE, as named in LED_MODE
  for(uint8_t i = 2; i <= 11; i++) {
    if(activeButton == i) {
      content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[i]) +  "\")'\"><button class=\"" + String(LED_MODES[i]) + "\" id=\"" + String(LED_MODES[i]) +  "\">" + String(LED_MODES[i]) +  "</button></a>";
    } else {
      content += "<a onclick='makeAjaxCall(\"/" + String(LED_MODES[i]) +  "\")'\"><button id=\"" + String(LED_MODES[i]) +  "\">" + String(LED_MODES[i]) +  "</button></a>";
    }
  } 
  content += "</div>";
  
  // Footnote
  content += "<div class=\"footnote\">&nbsp; Simple LED Strip Controller Version 1.2.0 <br/><a href=\"https://github.com/eighthree/Simple-LED-Strip-Controller\" target=\"_new\">View Project on Github</a><br/></div>";

  // JS for ajax request & button toggles
  content += "<script type=\"text/javascript\">function makeAjaxCall(e){var t=new XMLHttpRequest;t.onreadystatechange=function(){t.readyState==XMLHttpRequest.DONE&&(200==t.status?console.log(\"response form Ajax\"):400==t.status?alert(\"There was an error 400\"):alert(\"something else other than 200 was returned\"))},t.open(\"GET\",e,!0),t.send()}";
  content += "function changeClass(){for(var t=document.getElementsByTagName(\"button\"),e=0;e<t.length;e++){var n=t[e].getAttribute(\"id\");this.getAttribute(\"id\")==n?document.getElementById(n).className=n:document.getElementById(n).className=\"inactive\"}}window.onload=function(){for(var t=document.getElementsByTagName(\"button\"),e=0;e<t.length;e++){var n=t[e].getAttribute(\"id\");document.getElementById(n).addEventListener(\"click\",changeClass)}};";
  content += "</script>";

  content += "</body></html>";
  server.send(200, "text/html", content);
}

// Set LED State
// Things that should happen before/after the variable is modified
// should occur here.
void ledOFF(){
  handleRootPath(LED_STATE);  // Refresh the root page + pass the LED state for CSS changes
  LED_STATE = 0;              // Change LED State
}

void ledON(){
  handleRootPath(LED_STATE);
  LED_STATE = 1;
}

void redLight(){
  handleRootPath(LED_STATE); 
  LED_STATE = 2;
}

void rainbowLight(){
  handleRootPath(LED_STATE);
  LED_STATE = 3;
}

void cylonLight(){
  handleRootPath(LED_STATE);
  LED_STATE = 4;
}

void sparkleLight(){
  handleRootPath(LED_STATE);
  LED_STATE = 5;
}

void fireLight(){
  handleRootPath(LED_STATE);
  LED_STATE = 6;
}

void sunsetLight(){
  handleRootPath(LED_STATE); 
  LED_STATE = 7;
}

void theatreLight(){
  handleRootPath(LED_STATE);
  LED_STATE = 8;
}

void blueLight() {
  handleRootPath(LED_STATE);
  LED_STATE = 9; 
}

void greenLight() {
  handleRootPath(LED_STATE); 
  LED_STATE = 10;
}

void purpleLight() {
  handleRootPath(LED_STATE); 
  LED_STATE = 11;
}

// LED Animations & Control
// You may use the FastLED library with the following animations
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Modified to work with better timing
void rainbow(uint8_t wait) {
  uint16_t i;

   for(i=0; i<strip.numPixels(); i++) {
     strip.setPixelColor(i, Wheel((i+rainbowColor) & 255));
   }
   strip.show();
   rainbowColor++;
   if(rainbowColor > 255) {
        rainbowColor = 0;
   }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Shifts lights a-la-CYLONS from Battlestar Galactica
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){

  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  
  delay(ReturnDelay);
}

// Fire-like, works best on multiple light arrays
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

// Randomly blinks a light brightly
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
  
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}
