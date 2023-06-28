#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "Credentials.h"
#include <sbus.h>
#include <ppm.h>

int channel = 1;

sbus::channel packet;

uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0x92, 0x02, 0x29};
esp_now_peer_info_t peerInfo;

// put function decelaration  here:
void OnDataSent_Master(const uint8_t *mac_addr, esp_now_send_status_t status);
void initializeESP_NOW_Master();
void sendESP_NOW_Master();
void initializeESP_NOW_Slave();
void OnDataRecv_Slave(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// put function definition here:
void OnDataSent_Master(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initializeESP_NOW_Master(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // Init ESP-NOW

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent_Master);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  bool exists = esp_now_is_peer_exist(peerInfo.peer_addr);
  if (!exists){
    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
    else{
      Serial.println("Added Peer successfully");
    }
  }
  else{
    Serial.println("peer exists");
  }
  int8_t power = 78; // 100 mw
  esp_wifi_get_max_tx_power(&power);
}

void sendESP_NOW_Master(){
  // Send message via ESP-NOW
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));

  while(result != ESP_OK){
    Serial.println("Sending");
  }
}

void initializeESP_NOW_Slave(){
  // Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  int8_t power = 78; // 100 mw
  esp_wifi_get_max_tx_power(&power);
  bool result = WiFi.softAP(SSID, PASSWORD, channel, 0);
  if (!result){
    Serial.println("AP Config failed.");
  }
  else{
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL ");
    Serial.println(WiFi.channel());
  }

  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());

  WiFi.disconnect();
  if (esp_now_init() == ESP_OK){
    Serial.println("ESPNow Init Success");
  }
  else{
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv_Slave);
}

void OnDataRecv_Slave(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&packet , data ,sizeof(packet));
}