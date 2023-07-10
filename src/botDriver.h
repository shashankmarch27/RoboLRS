#ifndef BOT_DRIVER
#define BOT_DRIVER
#include <Arduino.h>
#include <sbus.h>

#define RPWM_1 14 // pin for RPWM
#define R_EN_1 19  // pin for R_EN
#define RPWM_2 21 // pin for RPWM
#define R_EN_2 15 // pin for R_EN

#define LPWM_1 22  // pin for LPWM
#define L_EN_1 13 // pin for L_EN
#define LPWM_2 5
#define L_EN_2 4 // pin for L_EN

#define STOP_HARD_MODE 0

/////// //////////////////////////////PWM Config
#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8
#define PWM_MIN 0   // corresponds to 0 degress
#define PWM_MAX 255 // corresponds to 180 degrees

#define MOTOR1CHANNEL 0
#define MOTOR2CHANNEL 1
#define MOTOR3CHANNEL 2
#define MOTOR4CHANNEL 3

/////////////////////////////////////////////////User Specified Data /////////////////////////////////////////////
int max_ch_rudder = 1780;
int max_ch_throttle = 1780;
int max_ch_elevator = 1780;
int max_ch_aileron = 1780;
int min_ch_rudder = 210;
int min_ch_throttle = 210;
int min_ch_elevator = 210;
int min_ch_aileron = 210;
int mid_ch_rudder = 993;
int mid_ch_throttle = 993;
int mid_ch_elevator = 993;
int mid_ch_aileron = 993;

#define deadzone 40

int motor_left_rpm = 0;
int motor_right_rpm = 0;

void initialise_driver(){

    #if STOP_HARD_MODE 

    pinMode(R_EN_1, OUTPUT);
    pinMode(L_EN_1, OUTPUT);
    pinMode(R_EN_2, OUTPUT);
    pinMode(L_EN_2, OUTPUT);
    pinMode(LPWM_2,OUTPUT);
    pinMode(RPWM_2,OUTPUT);
    ledcSetup(MOTOR1CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR2CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(LPWM_1, MOTOR1CHANNEL);
    ledcAttachPin(RPWM_1, MOTOR2CHANNEL);

    #else
    pinMode(LPWM_1,OUTPUT);
    pinMode(LPWM_2,OUTPUT);
    pinMode(RPWM_1,OUTPUT);
    pinMode(RPWM_2,OUTPUT);
    ledcSetup(MOTOR1CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR2CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR3CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR4CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(R_EN_1, MOTOR1CHANNEL);
    ledcAttachPin(R_EN_2, MOTOR2CHANNEL);
    ledcAttachPin(L_EN_1, MOTOR3CHANNEL);
    ledcAttachPin(L_EN_2, MOTOR4CHANNEL);

    #endif
}

void drive_motor(sbuspacket_t* packet){

    if ((packet->channel5 < 1000) || packet->failsafe){
        #if STOP_HARD_MODE 
        motor_left_rpm = 0;
        motor_right_rpm = 0;
        digitalWrite(LPWM_2,HIGH);
        digitalWrite(RPWM_2,HIGH);

        #else
        motor_left_rpm = 0;
        motor_right_rpm = 0;
        #endif
    }
    else{
        #if STOP_HARD_MODE 
        digitalWrite(R_EN_1, HIGH);
        digitalWrite(R_EN_2, HIGH);
        digitalWrite(L_EN_1, HIGH);
        digitalWrite(L_EN_2, HIGH);
        #endif

        if (!packet->frame_lost){
            if(packet->channel4 > 993 + deadzone){
                motor_left_rpm = map(packet->channel4,mid_ch_rudder,max_ch_rudder,PWM_MIN,PWM_MAX);
                motor_right_rpm = map(packet->channel4,mid_ch_rudder,max_ch_rudder,PWM_MIN,-PWM_MAX);
            }
            else if(packet->channel4 < 993 - deadzone){
                motor_left_rpm = map(packet->channel4,min_ch_rudder,mid_ch_rudder,-PWM_MAX,PWM_MIN);
                motor_right_rpm = map(packet->channel4,min_ch_rudder,mid_ch_rudder,PWM_MAX,PWM_MIN);
            }
            else{
                if(packet->channel1 > 993 + deadzone){
                motor_left_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                motor_right_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX)* map(packet->channel1,mid_ch_aileron,max_ch_aileron,PWM_MAX,-PWM_MAX) / PWM_MAX;
                }
                else if(packet->channel1 < 993 - deadzone){
                    motor_left_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX) * map(packet->channel1,mid_ch_aileron,min_ch_aileron,PWM_MAX,-PWM_MAX) / PWM_MAX;
                    motor_right_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                }
                else{
                    motor_left_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                    motor_right_rpm = map(packet->channel3,min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                }
            }
            if(packet->channel6 > 1000){
                motor_left_rpm = -motor_left_rpm;
                motor_right_rpm = -motor_right_rpm;
            }
        }
    }

    motor_left_rpm = constrain(motor_left_rpm, -PWM_MAX, PWM_MAX);
    motor_right_rpm = constrain(motor_right_rpm, -PWM_MAX, PWM_MAX);

    #if STOP_HARD_MODE 
    if(motor_left_rpm >= 0){
        ledcWrite(MOTOR1CHANNEL,motor_left_rpm);
        digitalWrite(LPWM_2,LOW);
    }
    else{
        ledcWrite(MOTOR1CHANNEL,255 - abs(motor_left_rpm));
        digitalWrite(LPWM_2,HIGH);
    }

    if(motor_right_rpm >= 0){
        ledcWrite(MOTOR2CHANNEL,motor_right_rpm);
        digitalWrite(RPWM_2,LOW);
    }
    else{
        ledcWrite(MOTOR2CHANNEL,255 - abs(motor_right_rpm));
        digitalWrite(RPWM_2,HIGH);
    }

    #else
    if(motor_left_rpm >= 0){
        digitalWrite(LPWM_1,LOW);
        digitalWrite(LPWM_2,HIGH);
    }
    else{
        digitalWrite(LPWM_1,HIGH);
        digitalWrite(LPWM_2,LOW);
    }

    if(motor_right_rpm >= 0){
        digitalWrite(RPWM_1,LOW);
        digitalWrite(RPWM_2,HIGH);
    }
    else{
        digitalWrite(RPWM_1,HIGH);
        digitalWrite(RPWM_2,LOW);
    }
    ledcWrite(MOTOR3CHANNEL,abs(motor_left_rpm));
    ledcWrite(MOTOR4CHANNEL,abs(motor_left_rpm));
    ledcWrite(MOTOR1CHANNEL,abs(motor_right_rpm));
    ledcWrite(MOTOR2CHANNEL,abs(motor_right_rpm));
    #endif
}
#endif