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
  receive.read();

  Serial.print(receive.data[AILERON]);
  Serial.print(" ");
  Serial.print(receive.data[ELEVATOR]);
  Serial.print(" ");
  Serial.print(receive.data[THROTTLE]);
  Serial.print(" ");
  Serial.print(receive.data[RUDDER]);
  Serial.println(" ");
}
