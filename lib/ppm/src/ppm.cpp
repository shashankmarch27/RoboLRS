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
}
void ppm::deinit()
{
    detachInterrupt(ppm_pin);
    failsafe = true;
    frame_lost = true;
}
void ppm::read()
{
    // will check whther the receiver is missing or not
    if (micros() - end_pulse > 2000000) // no edge in 2 seconds
    {
        detachInterrupt(ppm_pin);
        failsafe = true;
        frame_lost = true;
    }
}
void ppm::write()
{
}