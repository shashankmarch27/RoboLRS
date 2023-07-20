#include <Arduino.h>
#include <sbus.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"
#include "botDriver.h"

// put function declarations here:
void setup();
void loop();

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  initializeESP_NOW_Slave();
  initialise_driver();
}

void loop(){
  // put your main code here, to run repeatedly:
  drive_motor(&packet);
}

// Put function definition here: