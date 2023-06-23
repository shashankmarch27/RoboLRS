#ifndef PPM_H
#define PPM_H
#include <Arduino.h>

#define AILERON 0
#define ELEVATOR 1
#define THROTTLE 2
#define RUDDER 3
#define AUX1 4
#define AUX2 5
#define AUX3 6
#define AUX4 7
#define AUX5 8
#define AUX6 9
#define AUX7 10
#define AUX8 11
#define AUX9 12
#define AUX10 13
#define AUX11 14
#define AUX12 15

class ppm
{

private:
    int ppm_pin;
    uint t_sync;
    uint8_t max_number_of_channels;
    unsigned long start_pulse = 0;
    unsigned long end_pulse = 0;
    unsigned long pulse_width = 0;

    bool header_detected_sbus = false;
    int prev_buffer_sbus;
    int buffer_sbus;
    int rx_index;
    uint8_t data_rx[25];
    uint8_t data_tx[25];
    int current_millis;
    int previous_millis;
    int8_t number_of_channels_read = -1; // -1 means waiting for first rising edge

public:
    static ppm* instance; // this approach limits the number of ppm inputs to 1 only
    void static IRAM_ATTR read_ppm_signal();

    uint16_t data[16] = {0};
    bool failsafe;
    bool frame_lost;
    uint8_t channels_being_received = 0;

    ppm(int pin, uint t_sync = 5000, uint8_t max_number_of_channels = 8);
    void init();
    void read();  // dummy functions just to maintain compatibility
    void write(); // dummy functions just to maintain compatibility
    void deinit();
    ~ppm();
};

#endif