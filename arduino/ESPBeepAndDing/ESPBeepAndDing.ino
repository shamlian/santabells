// Load this code onto the ESP8266-based board running each Santa ringer.
// Connect a piezo buzzer to pin 4, if desired.
// Connect a motor driver with Phase and Enable inputs (I used a TI
// DRV8838 on a Pololu carrier) to pins 12 and 13.

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#define PIN_BEEP 4
#define PIN_EN 12
#define PIN_PH 13

#define TIME_ON 15 //ms

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);

char hname[11] = "beepXXXXXX";

void setup() {
  byte mac[6];

  digitalWrite(PIN_EN, LOW);
  digitalWrite(PIN_PH, LOW);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_PH, OUTPUT);
  
  Serial.begin(9600);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

   //exit after config instead of connecting
   wifiManager.setBreakAfterConfig(true);
  
  //reset settings - for testing
  //wifiManager.resetSettings();
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // Print the IP address
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  sprintf(hname+4, "%02X%02X%02X", mac[3], mac[4], mac[5]);
  
  if (MDNS.begin(hname)) {
    Serial.println("MDNS responder started");
  }

  pinMode(PIN_BEEP, OUTPUT);

  //server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  
  server.on("/0", [](){
    digitalWrite(PIN_PH, LOW);
    digitalWrite(PIN_EN, HIGH);
    delay(TIME_ON);
    digitalWrite(PIN_EN, LOW);
    server.send(200, "text/plain", "ding");
  });
  
  server.on("/1", [](){
    digitalWrite(PIN_PH, HIGH);
    digitalWrite(PIN_EN, HIGH);
    delay(TIME_ON);
    digitalWrite(PIN_EN, LOW);
    server.send(200, "text/plain", "dong");
  });

  // Start the server
  server.begin();
  Serial.println("Server started");
}

volatile unsigned int freq = 0;

void loop(void){
  server.handleClient();

  if (freq == 0) {
    analogWrite(PIN_BEEP, PWMRANGE);
  }
  else {
    /*while (f > 1000) {
      f /= 2;
    }*/
    analogWriteFreq(freq);
    Serial.println(freq);
    analogWrite(PIN_BEEP, PWMRANGE/2);
  }
}

void handleNotFound() {
  for (int i = 0; i < server.args(); i++) {
    freq = server.arg(i).toInt();
  }

  server.send(200, "text/plain", hname);
}

/*
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}*/
