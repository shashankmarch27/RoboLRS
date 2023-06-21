#include <Arduino.h>
#include <sbus.h>

#define PPM_PIN 35 //gpio
#define T_SYNC 5000 // in micros
#define NUMBER_OF_CHANNELS 8 // max number of channels

uint16_t channels_received[16] = {0};
int8_t number_of_channels_read = -1; // -1 means waiting for first rising edge

unsigned long start_pulse = 0;
unsigned long end_pulse = 0;
unsigned long pulse_width = 0;

// put object declarations here:
sbus receive(&Serial2,17,16); 

// put function declarations here:
void IRAM_ATTR read_ppm_signal();

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PPM_PIN, INPUT_PULLUP);
  receive.init();
  attachInterrupt(PPM_PIN, read_ppm_signal, FALLING);
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

void IRAM_ATTR read_ppm_signal(){

  start_pulse = end_pulse;
  end_pulse = micros();
  pulse_width = end_pulse - start_pulse;


  if (number_of_channels_read == -1)
  {// first rising edge seen
    number_of_channels_read = 0;
    return;
  }
  else if (pulse_width > T_SYNC)
  {
    number_of_channels_read = 0;
    return;
  }else   if (number_of_channels_read > NUMBER_OF_CHANNELS)
  {
    // sync bit starts
    number_of_channels_read = -1;
    return;
  }

  number_of_channels_read++;
  channels_received[number_of_channels_read - 1] = uint16_t(pulse_width);


}