/*
  ws2912fx commands

  @danhalbert
  string hexstring = "#FF3Fa0";

  // Get rid of '#' and convert it to integer
  long number = strtol( &hexstring[1], NULL, 16);
  Based on:
  https://github.com/toblum/McLighting/blob/561baad2b10cb11ab1b151b926b0aa9edb2db7f8/Arduino/McLighting/request_handlers.h#L221
  
*/
void setColour(uint8_t * wsPayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &wsPayload[1], NULL, 16);
  
  // Split them up into r, g, b values
  R_LED = rgb >> 16;
  G_LED = rgb >> 8 & 0xFF;
  B_LED = rgb & 0xFF;
  yield();
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setColor(rgb);
}

void setFX (uint8_t * wsPayload) {
  uint8_t modeValue = (uint8_t) strtol((const char *) &wsPayload[1], NULL, 10);
  Serial.println(modeValue);
  CURRENT_MODE = modeValue;
  yield();
  ws2812fx.setMode(modeValue);
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
      
      ws2812fx.setMode(FX_MODE_STATIC);
      ws2812fx.setColor((uint32_t) rgb_value);
      yield();
      server.send(200, "text/plain", String(rgb_value));
    } else if(server.argName(i) == "format") {
        if(server.arg(i) == "hex") {
          yield();
          server.send(200, "text/plain", currentHEX);    
        }
    }
    
  } // end for
  
}


/*
  JSON Builders
*/
String jsonBuildSettingsList() {
  time_t now = time(nullptr);

  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& settings = jsonBuffer.createObject();
  settings["device_name"] = PORTAL_AP_NAME;
  settings["device_time"] = ctime(&now);
  settings["device_software_version"] = "2.0";
  settings["device_ip"] = ipaddress;

  JsonObject& active = settings.createNestedObject("active");
  active["red"] = R_LED;
  active["green"] = G_LED;
  active["blue"] = B_LED;     
  active["mode"] = CURRENT_MODE;
  
  String jsonString;
  settings.printTo(jsonString);

  return jsonString;
}

String jsonBuildModeList() {
  DynamicJsonBuffer jsonBuffer(4000);
  JsonObject& modes = jsonBuffer.createObject();
  JsonArray& effect = modes.createNestedArray("effects");

  /* https://github.com/bblanchon/ArduinoJson/issues/258#issuecomment-205877000 */
  for (int i = 0; i < ws2812fx.getModeCount(); i++) {
    JsonObject& attribute = effect.createNestedObject();
    attribute["id"] = i;
    attribute["name"] = ws2812fx.getModeName(i);
  }
  
  String jsonString;
  modes.printTo(jsonString);

  return jsonString;
}

void jsonModeList() {
  yield();
  server.send ( 200, "application/json", jsonBuildModeList() );
}

void jsonSettingsList() {
  yield();
  server.send( 200, "application/json", jsonBuildSettingsList() );
}

/*"set_url": "http://deskstrip.local/rgb?color=0x%s", "get_url": "http://deskstrip.local/hex",  */
void txtCurrentHex() {
  String currentHex = String((R_LED << 16L) | (G_LED << 8) | B_LED, HEX); 
  yield();
  server.send( 200, "text/plain", currentHex);
}

/*
  webSocketEvent
*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
              setColour(payload);
              yield();
              webSocket.sendTXT(num, "OK");
            } else if (payload[0] == '*') {
              setFX(payload);
              yield();
              webSocket.sendTXT(num, "OK");
            }

            break;
    }

}

