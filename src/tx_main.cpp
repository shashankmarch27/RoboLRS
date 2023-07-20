#include <Arduino.h>
#include <sbus.h>
#include <crsf.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

crsf receive(&Serial2,16,17);

// put function declarations here:
void setup();
void loop();

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  receive.init();
  // initializeESP_NOW_Master();
}

void loop(){
  // put your main code here, to run repeatedly:
  receive.read(&packet);
  Serial.print(packet.channel0);
  Serial.print("\t");
  Serial.print(packet.channel1);
  Serial.print("\t");
  Serial.print(packet.channel2);
  Serial.print("\t");
  Serial.print(packet.channel3);
  Serial.print("\t");
  Serial.print(packet.channel4);
  Serial.print("\t");
  Serial.print(packet.channel5);
  Serial.print("\t");
  Serial.print(packet.channel6);
  Serial.print("\t");
  Serial.print(packet.channel7);
  Serial.print("\t");
  Serial.print(packet.channel8);
  Serial.print("\t");
  Serial.print(packet.channel9);
  Serial.print("\t");
  Serial.print(packet.channel10);
  Serial.print("\t");
  Serial.print(packet.channel11);
  Serial.print("\t");
  Serial.print(packet.channel12);
  Serial.print("\t");
  Serial.print(packet.channel13);
  Serial.print("\t");
  Serial.print(packet.channel14);
  Serial.print("\t");
  Serial.print(packet.channel15);
  Serial.println("\t");


  // sendESP_NOW_Master();
}

// Put function definition here: