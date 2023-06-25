#include "ppm.h"

// definitions of static memeber variables
ppm *ppm::instance = NULL;

ppm::ppm(int pin, uint t_sync, uint8_t max_number_of_channels)
{
    ppm_pin = pin;
    this->t_sync = t_sync;
    this->max_number_of_channels = max_number_of_channels;
    failsafe = false;
    frame_lost = false;
    instance = this;
}
ppm::~ppm()
{
    detachInterrupt(ppm_pin);
}

void ppm::init()
{
    pinMode(ppm_pin, INPUT_PULLUP);
    attachInterrupt(ppm_pin, ppm::read_ppm_signal, FALLING);
}

void IRAM_ATTR ppm::read_ppm_signal()
{
    instance->start_pulse = instance->end_pulse;
    instance->end_pulse = micros();
    instance->pulse_width = instance->end_pulse - instance->start_pulse;

    if (instance->number_of_channels_read == -1)
    { // first rising edge seen
        instance->number_of_channels_read = 0;
        return;
    }
    else if (instance->pulse_width > instance->t_sync)
    {
        if (instance->number_of_channels_read > 0)
        {
            instance->number_of_packets_received++;
        }
        else
        {
            instance->frame_lost = true;
            instance->failsafe = true;
        }
        instance->channels_being_received = instance->number_of_channels_read;
        instance->number_of_channels_read = 0;

        return;
    }
    else if (instance->pulse_width > 2100 || instance->pulse_width<980)
    { // wrong signal
        instance->frame_lost = true;
        instance->failsafe = true;
    }
    else if (instance->number_of_channels_read > instance->max_number_of_channels)
    {
        // sync bit starts
        instance->channels_being_received = instance->number_of_channels_read;
        instance->number_of_channels_read = -1;
        return;
    }

    instance->number_of_channels_read++;
    instance->data[instance->number_of_channels_read - 1] = uint16_t(instance->pulse_width);
    instance->data[instance->number_of_channels_read - 1] = constrain(instance->data[instance->number_of_channels_read - 1] , 988 , 2011);
}
void ppm::deinit()
{
    detachInterrupt(ppm_pin);
    failsafe = true;
    frame_lost = true;
}
packet* ppm::read()
{
    // will check whther the receiver is missing or not
    if (micros() - end_pulse > 2000000) // no edge in 2 seconds
    {
        detachInterrupt(ppm_pin);
        failsafe = true;
        frame_lost = true;
    }
    data_struct.aileron = data[0];
    data_struct.elevator = data[1];
    data_struct.throttle = data[2];
    data_struct.rudder = data[3];
    data_struct.aux1 = data[4];
    data_struct.aux2 = data[5];
    data_struct.aux3 = data[6];
    data_struct.aux4 = data[7];
    data_struct.aux5 = data[8];
    data_struct.aux6 = data[9];
    data_struct.aux7 = data[10];
    data_struct.aux8 = data[11];
    data_struct.aux9 = data[12];
    data_struct.aux10 = data[13];
    data_struct.aux11 = data[14];
    data_struct.aux12 = data[15];

    return (&data_struct);


}
void ppm::write()
{
}