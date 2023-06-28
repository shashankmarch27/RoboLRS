#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

#define MASTER 1

sbus receive(&Serial2,16,17);

// put function declarations here:
void setup();
void loop();
void printChannelvalue();

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  #if defined(MASTER)
  receive.init();
  initializeESP_NOW_Master();
  #else
  initializeESP_NOW_Slave();
  #endif
}

void loop(){
  // put your main code here, to run repeatedly:
  #if defined(MASTER)
  receive.read(packet);
  sendESP_NOW_Master();
  #else if
  printChannelvalue();
  #endif
}

// Put function definition here:
void printChannelvalue(){
  Serial.print(packet.channelValue[AILERON]);
  Serial.print("  ");
  Serial.print(packet.channelValue[ELEVATOR]);
  Serial.print("  ");
  Serial.print(packet.channelValue[THROTTLE]);
  Serial.print("  ");
  Serial.print(packet.channelValue[RUDDER]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX1]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX2]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX3]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX4]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX5]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX6]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX7]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX8]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX9]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX10]);
  Serial.print("  ");
  Serial.print(packet.channelValue[AUX11]);
  Serial.print("  ");
  Serial.println(packet.channelValue[AUX12]);
}