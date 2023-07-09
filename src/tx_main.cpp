#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

sbus receive(&Serial2,16,17);

// put function declarations here:
void setup();
void loop();

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  receive.init();
  initializeESP_NOW_Master();
}

void loop(){
  // put your main code here, to run repeatedly:
  receive.read(&packet);
  sendESP_NOW_Master();
}

// Put function definition here: