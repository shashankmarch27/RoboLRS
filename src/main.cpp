#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>
#include <WiFi.h>
#include "OTA.h"
#include "Credentials.h"
#include "espnow_driver.h"

enum signalmode{
  SBUS,
  PPM,
  AUTODETECT
} inputmode;

enum devicemode{
  MASTER,
  SLAVE,
  OTA
} deviceoperationmode;

// put object declarations here:
sbus receive(&Serial2, 16, 17);
// ppm receive(16);

// put function declarations here:
void printStatus();
void printInstructions();
void handelSerial();
void printChannelValues();
void setup();
void loop();

// Put function defenation here
void printStatus(){
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

void printInstructions(){
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
void handelSerial(){
  if (Serial.available())
  {
    char data = Serial.read();
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
void printChannelValues(){

  Serial.print(dataPacketPointer->aileron);
  Serial.print(" ");
  Serial.print(dataPacketPointer->elevator);
  Serial.print(" ");
  Serial.print(dataPacketPointer->throttle);
  Serial.print(" ");
  Serial.print(dataPacketPointer->rudder);
  Serial.println(" ");
}

void setup(){
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

void loop(){
  // put your main code here, to run repeatedly:
  handelSerial(); // handel Serial
  dataPacketPointer = receive.read(); // if anything bad look into sizeof(pointer)
  // sendESP_NOW_Master();
  printChannelValues();
  delay(1);
}
