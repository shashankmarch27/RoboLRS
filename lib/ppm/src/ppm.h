#ifndef PPM_H
#define PPM_H
#include <Arduino.h>

#ifndef PACKET_TYPE
#define PACKET_TYPE
struct packet{
  uint16_t aileron = 1500;
  uint16_t elevator = 1500;
  uint16_t throttle = 1000;
  uint16_t rudder = 1500;
  uint16_t aux1 = 1000;
  uint16_t aux2 = 1000;
  uint16_t aux3 = 1000;
  uint16_t aux4 = 1000;
  uint16_t aux5 = 1000;
  uint16_t aux6 = 1000;
  uint16_t aux7 = 1000;
  uint16_t aux8 = 1000;
  uint16_t aux9 = 1000;
  uint16_t aux10 = 1000;
  uint16_t aux11 = 1000;
  uint16_t aux12 = 1000;
  uint16_t rssi = 0;

};
#endif

class ppm
{

private:
    int ppm_pin;
    uint t_sync;
    uint8_t max_number_of_channels;
    unsigned long start_pulse = 0;
    unsigned long end_pulse = 0;
    unsigned long pulse_width = 0;

    int rx_index;
    uint8_t data_rx[25];
    uint8_t data_tx[25];
    int current_millis;
    int previous_millis;
    int8_t number_of_channels_read = -1; // -1 means waiting for first rising edge

public:
    static ppm* instance; // this approach limits the number of ppm inputs to 1 only
    void static IRAM_ATTR read_ppm_signal();
    packet data_struct;
    uint16_t data[16] = {0};
    bool failsafe;
    bool frame_lost;
    uint8_t channels_being_received = 0;
    uint number_of_packets_received = 0;

    ppm(int pin, uint t_sync = 5000, uint8_t max_number_of_channels = 8);
    void init();
    packet* read();  
    void write(); // dummy functions just to maintain compatibility
    void deinit();
    ~ppm();
};

#endif