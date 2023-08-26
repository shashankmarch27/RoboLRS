#include <Arduino.h>
#include <sbus.h>
#include <crsf.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

// Creating a AsyncWebServer object 
AsyncWebServer server(80);
crsf receive(&Serial2,5,18,UNINVERTED_CRSF);

crsf_channels_t packet;

// put function declarations here:
void setup();
void loop();
void notFound(AsyncWebServerRequest *request);

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  receive.init();

  // Initialize LittleFS
  LittleFS.begin();

  // Begin Wifi access point
  if (!WiFi.softAP(SSID, PASSWORD)) {
    log_e("Soft AP creation failed.");
    while(1);
  }

  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // Route for root index.css
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  // Route for root index.js
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/javascript"); });
            
  server.onNotFound(notFound);
  
  server.begin();

  // initializeESP_NOW_Master();
}

void loop(){
  // put your main code here, to run repeatedly:

  receive.read();
  packet = receive.getChannel();
  Serial.print(packet.channel1);
  Serial.print("  ");
  Serial.print(packet.channel2);
  Serial.print("  ");
  Serial.print(packet.channel3);
  Serial.print("  ");
  Serial.println(packet.channel4);
  // sendESP_NOW_Master(packet);
}

// Put function definition here:
void notFound(AsyncWebServerRequest *request){
  request->send(404, "text/plain", "Not found");
}