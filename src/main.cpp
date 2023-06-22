#include <Arduino.h>
#include <sbus.h>
#include <ppm.h>

// put object declarations here:
// sbus receive(&Serial2,17,16); 
ppm receive(35,5000, 8);
// put function declarations here:


void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  receive.init();
}

void loop(){
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();  // call this line at regular interval for ota functionality
  receive.read();

  TelnetStream.print(receive.data[AILERON]);
  TelnetStream.print(" ");
  TelnetStream.print(receive.data[ELEVATOR]);
  TelnetStream.print(" ");
  TelnetStream.print(receive.data[THROTTLE]);
  TelnetStream.print(" ");
  TelnetStream.print(receive.data[RUDDER]);
  TelnetStream.println(" ");
}
