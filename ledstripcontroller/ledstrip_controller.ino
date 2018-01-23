/*
  LED Strip Controller 2.0-ws
  Web Sockets
*/
#include "config.h"

/* File System/Hardware */
#include "FS.h"
#include <DoubleResetDetector.h>
#include <WS2812FX.h>
#include <ArduinoJson.h>

/* Wifi & Network Related Libraries*/
#include <ESP8266WiFi.h>         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>     
#include <ESP8266mDNS.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>

/* Time Related */
#include <time.h>

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);      // ESP8266 Web Server Port: Default 80
WebSocketsServer webSocket = WebSocketsServer(81);
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

extern const char index_html[];

/* Websocket Governor, handles events etc */
#include "ws_governor.h"

void setup() {
  Serial.begin(115200);
  Serial.println(PORTAL_FW);

  /* 
   *  Check for double reset flag on setup
   *  and run wifi manager, for connecting the device
   *  to your local wifi.
  */
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

  /* ws2812fx setup */
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  if (!MDNS.begin(PORTAL_AP_NAME)) {
  Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  
  Serial.println("mDNS responder started");

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // start server / spiff 
  server.begin();
  SPIFFS.begin();

  /* 
   *  handle server requests 
  */
  server.on("/", []() {
      // send index.html
      server.send(200, "text/html", index_html);
  });

  server.on("/modes.json", []() {
    jsonModeList();
  });

  server.on("/settings.json", []() {
    jsonSettingsList();
  });

  server.on("/hex", []() {
    txtCurrentHex();
  });

  server.on("/rgb", handleGenericArgs);
  
  /* Serve Static Files */
  server.serveStatic("/jscolor.min.js",  SPIFFS, "/jscolor.min.js" ,"max-age=86400"); 


  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

  time_t now = time(nullptr);
  Serial.println(ctime(&now));

  // https://forum.arduino.cc/index.php?topic=228884.msg2702097#msg2702097
  ipaddress = WiFi.localIP().toString();
  
  originalram = ESP.getFreeHeap();
  Serial.println("Heap on Startup: " + originalram);
  Serial.println("\nSet Up Complete! Thank you!");
}

void loop() {
  
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  uint32_t ram = ESP.getFreeHeap();
  
  webSocket.loop();
  server.handleClient();
  ws2812fx.service();

  // losing bytes work around
  if(HEAPCHECKER && currentMillis - previousMillis > interval){          
    tcpCleanup();           
    Serial.printf("tcpCleanup completed\n");
    previousMillis = currentMillis;
    Serial.printf("RAM: %d  change %d\n", ram, (ram - originalram ));
  }

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

/* https://github.com/esp8266/Arduino/issues/1532#issuecomment-176869292 */
void tcpCleanup()
{
  while(tcp_tw_pcbs!=NULL)
  {
    tcp_abort(tcp_tw_pcbs);
  }
}
