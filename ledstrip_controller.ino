/*  Simple LED Strip Controller 1.0 */
/*  
 *  Author: Timothy Garcia (http://timothygarcia.ca)
 *  Date: September 2017
 *  
 *  Description:
 *  Control a LED light strip via a web browser
 *  Allow configuration of device wifi settings via AP
 *  Serves a light browser-based controller with simple CSS for functionality
 *  
 *  Hardware Used:
 *  NodeMCU 1.0 / 10K Ohm Resistor / 470 Ohm Resistor / Momentary Switch / Protoype Board / 3-pin header (for LED)
 *  
 *  Extra Credits:
 *  Adafruit Industries www.adafruit.com
 *  Led Strip Animations: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 *  ESP8266 wifi based on examples by Nuno Santos: https://techtutorialsx.com
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

// Neopixel Setup
#define PIN D6
#define NUM_LEDS 29
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Trigger Configuration Portal: Default D1
#define TRIGGER_PIN D1
#define PORTAL_AP_NAME "VULPEioNMCU1"

// ESP8266 Web Server Port: Default 80
ESP8266WebServer server(80);

// For LED animations that loop/cycle
int LED_STATE = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);

  // Set brightness for strip to 5, prevents drawing too much power.
  strip.begin();
  strip.setBrightness(5);
  strip.show(); // Initialize all pixels to 'off'

 // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect…");
  }

  server.begin();  //Start the server
  Serial.println("Server listening");

  colorWipe(strip.Color(100, 100, 100), 50); // Set strip colour to white

  //Associate the handler function to the path
  server.on("/", handleRootPath);      
  server.on("/on", ledON);
  server.on("/off", ledOFF);
  server.on("/rainbow", rainbowLight);
  server.on("/red", redLight);
  server.on("/cylon", cylonLight);
  server.on("/sparkle", sparkleLight);
  server.on("/fire", fireLight);
  server.on("/sunset", sunsetLight);
  
}


void loop() {
  
  // Check if configuration portal requested.
  if ( digitalRead(TRIGGER_PIN) == HIGH ) {
    Serial.println("\n Configuration Portal Requested, Control Server Stopping");

    // Stop serving port 80
    server.stop();
    
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

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

    // If you get here you have connected to the WiFi
    Serial.println("Connected to Wireless AP.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP

    // Restart Server
    server.begin();
    
  } else {
     
    // put your main code here, to run repeatedly:
    // Debug LED Staste: Serial.println(LED_STATE);

    switch(LED_STATE) {
      case 3:
          strip.setBrightness(5); 
          rainbow(20); // TODO: Causes the server to stop loading page, requires manually entering address to stop animating.
          break;
      case 4:
          CylonBounce(0xff, 0, 0, 5, 10, 50);
          break;
      case 5:
          SnowSparkle(0x10, 0x10, 0x10, 20, 200);
          break;
      case 6:
          Fire(55,120,15);
          break;
    }
    
    server.handleClient();  
   
  }

}


// Server Control Paths

void handleRootPath() {           
  String content = "<html><head><title>Desk Light Strip</title></head>";
  content += "<style>svg {fill:#000;}div a:hover svg { fill: #fff; }body{font-family:verdana,sans-serif;background-color: #000000; background-image: linear-gradient(6deg, #000000 0%, #3e3e3e 100%);}.footnote{color:#fff; font-size: 1.5em;}.power{display:flex; width: 100%; flex-wrap:wrap;}.controller{display:flex;width:100%;flex-wrap:wrap;margin-left:-5px;}a{display:block;width:49%;margin-left:10px;margin-bottom:10px;flex-grow:2;} a button{background-color: #6b6b6b;background-image: linear-gradient(6deg, #e2e2e2 0%, #ffffff 100%);font-weight:bold;font-size:1.75em;width:100%;height:25%;border:0px;border-radius: 15px;}button:hover{color:#fff; background-color: #caffff; background-image: linear-gradient(180deg, #caffff 0%, #93d8d8 100%);cursor:hand;}</style>";
  content += "<body>";

  if (server.hasHeader("User-Agent")){
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }

  // Style Buttons
  switch(LED_STATE) {
    case 0:
      content += "<style>#on button{ background-color: #fbf5c0; background-image: linear-gradient(0deg, #fbf5c0 0%, #c3fcf8 50%, #ffffff 100%); }</style>";
      break;
    case 1:
      content += "<style>#off button{ background-color: #212121; background-image: linear-gradient(135deg, #212121 0%, #7a7a7a 100%); color:#fff;}</style>";
      break;
    case 2:
      content += "<style>#red {order:-1;}#red button{ background-color: #af0002; background-image: linear-gradient(0deg, #af0002 0%, #FF2525 74%); }</style>";
      break;
    case 3:
      content += "<style>#rainbow {order:-1;}#rainbow button{ background-color: #dc0003; background-image: linear-gradient(182deg, #dc0003 8%, #ff9600 22%, #9bb509 48%, #0dc5d0 66%, #e969ff 91%, #7200ff 100%);";
      break;
    case 4:
      content += "<style>#cylon {order:-1;}#cylon button{ background-color: #000000; background-image: linear-gradient(0deg, #000000 0%, #ff0002 50%, #000000 100%);</style>";
      break;
    case 5:
      content += "<style>#sparkle {order:-1;}#sparkle button{background-color: #000000; background-image: linear-gradient(0deg, #000000 0%, #ffffff 30%, #535353 57%, #ffffff 77%, #000000 91%, #4a4a4a 100%);</style>";
      break;
    case 6:
      content += "<style>#fire {order:-1;}#fire button{ background-color: #af0002; background-image: linear-gradient(0deg, #af0002 0%, #FF2525 74%); }</style>";
      break;
    case 7:
      content += "<style>#sunset {order:-1;}#sunset button{ background-color: #FBAB7E; background-image: linear-gradient(322deg, #FBAB7E 0%, #F7CE68 100%); }</style>";
      break;


  }

  // Power DIV, Separate from the rest and static
  content += "<div class=\"power\">";
  content += "<a href=\"/on\" id=\"on\" \"><button>";
  content += "<svg style=\"width:25%;height:25%\" viewBox=\"0 0 24 24\"><path d=\"M11,0V4H13V0H11M18.3,2.29L15.24,5.29L16.64,6.71L19.7,3.71L18.3,2.29M5.71,2.29L4.29,3.71L7.29,6.71L8.71,5.29L5.71,2.29M12,6A4,4 0 0,0 8,10V16H6V18H9V23H11V18H13V23H15V18H18V16H16V10A4,4 0 0,0 12,6M2,9V11H6V9H2M18,9V11H22V9H18Z\" /></svg>";
  content += "</button></a>";
  content += "<a href=\"/off\" id=\"off\" \"><button>";
  content += "<svg style=\"width:25%;height:25%\" viewBox=\"0 0 24 24\"><path d=\"M12,3C10.05,3 8.43,4.4 8.08,6.25L16.82,15H18V13H16V7A4,4 0 0,0 12,3M3.28,4L2,5.27L8,11.27V13H6V15H9V21H11V15H11.73L13,16.27V21H15V18.27L18.73,22L20,20.72L15,15.72L8,8.72L3.28,4Z\" /></svg>";
  content += "</button></a>";
  content += "</div>";

  // Controller DIV, shifts button order when active
  content += "<div class=\"controller\">";
  content += "<a href=\"/red\" id=\"red\" \"><button>Red </button></a>";
  content += "<a href=\"/sparkle\" id=\"sparkle\" \"><button>Sparkle </button></a>";
  content += "<a href=\"/cylon\" id=\"cylon\" \"><button>Cylon </button></a>";
  content += "<a href=\"/fire\" id=\"fire\" \"><button>Fire </button></a>";
  content += "<a href=\"/sunset\" id=\"sunset\" \"><button>Sunset </button></a>";
  content += "<a href=\"/rainbow\" id=\"rainbow\" \"><button>Rainbow <sup>**</sup> </button></a>";
  content += "</div>";

  // Footnote
  content += "<div class=\"footnote\"><sup>** Currently buggy. Works but you'll have to manually enter IPADDRESS/off to turn it off.</sup>";
  content += "</div>";
  
  content += "</body></html>";
  server.send(200, "text/html", content);

 
}


void ledON(){
  LED_STATE = 0;
  strip.setBrightness(10);
  colorWipe(strip.Color(100,100,100), 50);
  handleRootPath();
}


void ledOFF(){
  LED_STATE = 1;
  strip.setBrightness(5);
  colorWipe(strip.Color(0, 0, 0), 50);
    handleRootPath();
}

void redLight(){
  LED_STATE = 2;
  strip.setBrightness(5);
  colorWipe(strip.Color(255, 0, 0), 50);
  handleRootPath(); 
}

void rainbowLight(){
  LED_STATE = 3;
  handleRootPath();
}

void cylonLight(){
  LED_STATE = 4;
  handleRootPath();
}

void sparkleLight(){
  LED_STATE = 5;
  handleRootPath();
}

void fireLight(){
  LED_STATE = 6;
  handleRootPath();
}

void sunsetLight(){
  LED_STATE = 7;
  strip.setBrightness(8);
  colorWipe(strip.Color(251, 171, 76), 50);
  handleRootPath(); 
}

// LED Animations & Control
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


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
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


void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  
  for(int i=0; i<NUM_LEDS*2; i++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
      
      showStrip();
      delay(WaveDelay);
  }
}


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


void BouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
  
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  
  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
  
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
  
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
  
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],red,green,blue);
    }
    
    showStrip();
    setAll(0,0,0);
  }
}


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

