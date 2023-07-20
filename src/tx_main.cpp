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
  Serial.print(packet.ch0);
  Serial.print("\t");
  Serial.print(packet.ch1);
  Serial.print("\t");
  Serial.print(packet.ch2);
  Serial.print("\t");
  Serial.print(packet.ch3);
  Serial.print("\t");
  Serial.print(packet.ch4);
  Serial.print("\t");
  Serial.print(packet.ch5);
  Serial.print("\t");
  Serial.print(packet.ch6);
  Serial.print("\t");
  Serial.print(packet.ch7);
  Serial.print("\t");
  Serial.print(packet.ch8);
  Serial.print("\t");
  Serial.print(packet.ch9);
  Serial.print("\t");
  Serial.print(packet.ch10);
  Serial.print("\t");
  Serial.print(packet.ch11);
  Serial.print("\t");
  Serial.print(packet.ch12);
  Serial.print("\t");
  Serial.print(packet.ch13);
  Serial.print("\t");
  Serial.print(packet.ch14);
  Serial.print("\t");
  Serial.print(packet.ch15);
  Serial.println("\t");


  // sendESP_NOW_Master();
}

// Put function definition here: