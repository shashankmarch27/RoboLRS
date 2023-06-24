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
int channel = 1;

typedef struct struct_message
{
  int b = 256;
} struct_message;

enum signalmode
{
  SBUS,
  PPM,
  AUTODETECT
} inputmode;

enum devicemode
{
  MASTER,
  SLAVE,
  OTA
} deviceoperationmode;
// Create a struct_message called myData
struct_message dataPacket;

// put object declarations here:

sbus receive(&Serial2, 16, 17);
// ppm receive(16);

// put function declarations here:
void OnDataSent_Master(const uint8_t *mac_addr, esp_now_send_status_t status);
void initializeESP_NOW_Master();
void sendESP_NOW_Master();
void initializeESP_NOW_Slave();
void OnDataRecv_Slave(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void printStatus();
void printInstructions();
void handelSerial();
void printChannelValues();
void setup();
void loop();

// callback when data is sent
void OnDataSent_Master(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initializeESP_NOW_Master()
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
  esp_now_register_send_cb(OnDataSent_Master);

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

void sendESP_NOW_Master()
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

void initializeESP_NOW_Slave()
{
  // Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);

  bool result = WiFi.softAP(SSID, PASSWORD, channel, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  }
  else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL ");
    Serial.println(WiFi.channel());
  }

  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());

  WiFi.disconnect();
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv_Slave);
}

void OnDataRecv_Slave(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);
  Serial.print("Last Packet Recv Data: ");
  Serial.println(*data);
  Serial.println("");
}

void printStatus()
{
  Serial.print("MAC Address:");
  if (deviceoperationmode == MASTER)
    Serial.println(WiFi.macAddress());
  else
    Serial.println(WiFi.softAPmacAddress());
  Serial.print("Input Mode:");
  Serial.println(inputmode == PPM ? "PPM" : inputmode == SBUS     ? "SBUS"
                                        : inputmode == AUTODETECT ? "Autodetect"
                                                                  : "INVALID");
  Serial.print("WiFi Channel:");
  Serial.println(WiFi.channel());
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
    case '2':
      deviceoperationmode = MASTER;
      break;
    case '3':
      deviceoperationmode = SLAVE;
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
  deviceoperationmode = SLAVE;

  if (deviceoperationmode == MASTER)
    initializeESP_NOW_Master();
  else if (deviceoperationmode == SLAVE)
    initializeESP_NOW_Slave();
}

void loop()
{
  // put your main code here, to run repeatedly:
  handelSerial(); // handel Serial
  receive.read();
}
