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

crsf receive(&Serial2,16,17,UNINVERTED_CRSF);

crsf_channels_t packet;
crsfLinkStatistics_t link_status;

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
  receive.read();
  packet = receive.getChannel();
  link_status = receive.getlinkStatus();
  Serial.println(link_status.uplink_Link_quality);

  // sendESP_NOW_Master(packet);
}

// Put function definition here: