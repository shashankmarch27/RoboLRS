#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>
#include <WiFi.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

ppm::channel data;

// put object declarations here:
// sbus receive(&Serial2,16,17);
ppm receive(16);

// put function declarations here:
void setup();
void loop();

// Put function definition here:

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  receive.init();
}

void loop(){
  // put your main code here, to run repeatedly:
  receive.read(data);
  Serial.println(data.channelValue[0]);
}