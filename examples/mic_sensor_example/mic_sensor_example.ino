/*
Simple LED Strip Controller 1.2.5 - Example Use #2
Arduino - MAX4466 Electret Microphone Amplifier "Washer/Dryer" Visual Notification
Date: September 2017
Description: Change LED Strip colour when washer/dryer stops making
sounds.

Modified from https://learn.adafruit.com/led-ampli-tie/overview

Wash Cycle peakToPeak Values
Min: 5
Max: 352
Avg: 55.00984121

Dryer Cycle peakToPeak Values
Min: 5
Max: 266
Avg: 64.24

Hardware Used:
Node MCU 1.0 / Adafruit MAX4466 Electret Microphone Amplifier
https://www.adafruit.com/product/1063
Pin: OUT <-> A0, VCC <-> 3V3, GND <-> GND
Deep Sleep: Reset <-> D0, Connect these pins via a jumper for deep sleep function

License: This code is public domain. You can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation.  <http://www.gnu.org/licenses/>.
Certain libraries used may be under a different license.
*/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>

#define MIC_PIN   A0
#define SAMPLE_WINDOW   25  
#define INPUT_FLOOR 10      //Lower range of analogRead input
#define INPUT_CEILING 300   //Max range of analogRead input, the lower the value the more sensitive (1023 = max)
 
byte peak = 16;      // Peak level of column; used for falling dots
unsigned int sample;

// Setup
const char* ssid = "SSID";
const char* password = "PASSWORD";

void setup() {
  // Set these values to reflect polling times
  Serial.setTimeout(5000);
  Serial.begin(9600);
  
  while(!Serial) { } // Wait for serial to init

  Serial.println("Sampling ambient sounds...");

 // put your main code here, to run repeatedly:
  unsigned long startMillis= millis(); 
  float peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  
  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  if(peakToPeak <= 5) {
    
   WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print("Connecting..");
    }
    
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      
      HTTPClient http;  //Declare an object of class HTTPClient
      http.begin("http://SIMPLE_LED_STRIP_IP/rgb?color=0xFF0000");  //  Specify request destination
      
      int httpCode = http.GET();                      //  Send the request
      
      if (httpCode > 0) {                             //  Check the returning code
        String payload = http.getString();            //  Get the request response payload
        Serial.println(payload);                      //  Print the response payload
      }
      
      http.end();   //  Close connection
   
    }
  }

  Serial.println(peakToPeak);
  Serial.println("Going into deep sleep for 20 seconds");
  ESP.deepSleep(20e6); // 20e6 is 20 microseconds
}

void loop() {
 
}
