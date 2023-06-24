#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>
#include <esp_now.h>
#include <WiFi.h>
#include "OTA.h"
#include "Credentials.h"

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
esp_now_peer_info_t peerInfo;

char data;

typedef struct struct_message
{
  int b = 256;
} struct_message;

enum mode
{
  SBUS,
  PPM,
  AUTODETECT
} inputmode;
// Create a struct_message called myData
struct_message dataPacket;

// put object declarations here:

sbus receive(&Serial2, 16, 17);
// ppm receive(16);

// put function declarations here:

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initializeESP_NOW()
{
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
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  bool exists = esp_now_is_peer_exist(peerInfo.peer_addr);
  if (!exists)
  {
    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }
  }
}

void sendESP_NOW()
{
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataPacket, sizeof(dataPacket));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
}

void printStatus()
{
  Serial.print("MAC Address:");
  Serial.println(WiFi.macAddress());
  Serial.print("Input Mode:");
  Serial.println(inputmode == PPM ? "PPM" : inputmode == SBUS     ? "SBUS"
                                        : inputmode == AUTODETECT ? "Autodetect"
                                                                  : "INVALID");
}

void printInstructions()
{
  Serial.println("1. Print Status");
  Serial.println("2. Change to master mode");
  Serial.println("3. Change to slave mode");
  Serial.println("4. Find slave devices");
  Serial.println("5. Add slave device");
  Serial.println("6. Remove slave device");
  Serial.println("7. Force PPM");
  Serial.println("8. Force SBUS");
  Serial.println("9. Autodetect Signal");
}
void handelSerial()
{
  if (Serial.available())
  {
    data = Serial.read();
    switch (data)
    {
    case '1':
      printStatus();
      break;
    case '7':
      inputmode = PPM;
      Serial.println("in PPM mode");
      break;
    case '8':
      inputmode = SBUS;
      Serial.println("in SBUS mode");
      break;

    default:
      Serial.println("invalid option");
      printInstructions();
      break;
    }
  }
}
void printChannelValues()
{
  
  Serial.print(receive.data[AILERON]);
  Serial.print(" ");
  Serial.print(receive.data[ELEVATOR]);
  Serial.print(" ");
  Serial.print(receive.data[THROTTLE]);
  Serial.print(" ");
  Serial.print(receive.data[RUDDER]);
  Serial.println(" ");

}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  receive.init();
  printInstructions();
  inputmode = PPM;
  initializeESP_NOW();
}

void loop()
{
  // put your main code here, to run repeatedly:
  handelSerial(); // handel Serial
  receive.read();
}
