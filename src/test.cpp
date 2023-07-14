#include <Arduino.h>
#include <crsf.h>

crsfpacket_t data;
uint8_t rx_data[70];

crsf reciever(&Serial2,16,17,INVERTED_CRSF);

void setup(){
    Serial.begin(115200);
    reciever.init();
}

void loop(){
    reciever.read(&data);
}