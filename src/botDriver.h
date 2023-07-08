#ifndef BOT_DRIVER
#define BOT_DRIVER
#include <Arduino.h>
#include <sbus.h>

#define RPWM_1 9 // pin for RPWM
#define R_EN_1 4  // pin for R_EN
#define RPWM_2 14 // pin for RPWM
#define R_EN_2 15 // pin for R_EN

#define LPWM_1 5  // pin for LPWM
#define L_EN_1 13 // pin for L_EN
#define LPWM_2 21
#define L_EN_2 19 // pin for L_EN

#define STOP_HARD_MODE 0

/////// //////////////////////////////PWM Config
#define PWM_FREQUENCY 10000

#define PWM_RESOLUTION 8
#define PWM_MIN 0   // corresponds to 0 degress
#define PWM_MAX 255 // corresponds to 180 degrees

#define motor1Channel 0
#define motor2Channel 1
#define motor3Channel 2
#define motor4Channel 3

/////////////////////////////////////////////////User Specified Data /////////////////////////////////////////////
int max_ch_rudder = 1811;
int max_ch_throttle = 1811;
int max_ch_elevator = 1811;
int max_ch_aileron = 1811;
int min_ch_rudder = 173;
int min_ch_throttle = 173;
int min_ch_elevator = 173;
int min_ch_aileron = 173;
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
    ledcSetup(motor1Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor2Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(LPWM_1, motor1Channel);
    ledcAttachPin(RPWM_1, motor2Channel);

    #else
    pinMode(LPWM_1,OUTPUT);
    pinMode(LPWM_2,OUTPUT);
    pinMode(RPWM_1,OUTPUT);
    pinMode(RPWM_2,OUTPUT);
    ledcSetup(motor1Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor2Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor3Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor4Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(R_EN_1, motor1Channel);
    ledcAttachPin(R_EN_2, motor2Channel);
    ledcAttachPin(L_EN_1, motor3Channel);
    ledcAttachPin(L_EN_2, motor4Channel);

    #endif
}

void drive_motor(sbus::sbusChannel_t* packet){

    if ((packet->channelValue[AUX1] < 1000) || packet->failsafe){
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
            if(packet->channelValue[RUDDER] > 993 + deadzone){
                motor_left_rpm = map(packet->channelValue[RUDDER],mid_ch_rudder,max_ch_rudder,PWM_MIN,PWM_MAX);
                motor_right_rpm = map(packet->channelValue[RUDDER],mid_ch_rudder,max_ch_rudder,PWM_MIN,-PWM_MAX);
            }
            else if(packet->channelValue[RUDDER] < 993 - deadzone){
                motor_left_rpm = map(packet->channelValue[RUDDER],min_ch_rudder,mid_ch_rudder,-PWM_MAX,PWM_MIN);
                motor_right_rpm = map(packet->channelValue[RUDDER],min_ch_rudder,mid_ch_rudder,PWM_MAX,PWM_MIN);
            }
            else{
                if(packet->channelValue[AILERON] > 993 + deadzone){
                motor_left_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                motor_right_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX)* map(packet->channelValue[AILERON],mid_ch_aileron,max_ch_aileron,PWM_MAX,-PWM_MAX) / PWM_MAX;
                }
                else if(packet->channelValue[AILERON] < 993 - deadzone){
                    motor_left_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX) * map(packet->channelValue[AILERON],mid_ch_aileron,min_ch_aileron,PWM_MAX,-PWM_MAX) / PWM_MAX;
                    motor_right_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                }
                else{
                    motor_left_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                    motor_right_rpm = map(packet->channelValue[THROTTLE],min_ch_throttle,max_ch_throttle,PWM_MIN,PWM_MAX);
                }
            }
            if(packet->channelValue[AUX2] > 1000){
                motor_left_rpm = -motor_left_rpm;
                motor_right_rpm = -motor_right_rpm;
            }
        }
    }

    motor_left_rpm = constrain(motor_left_rpm, -PWM_MAX, PWM_MAX);
    motor_right_rpm = constrain(motor_right_rpm, -PWM_MAX, PWM_MAX);

    Serial.print(motor_left_rpm);
    Serial.print("  ");
    Serial.println(motor_right_rpm);

    #if STOP_HARD_MODE 
    if(motor_left_rpm > 0){
        ledcWrite(motor1Channel,motor_left_rpm);
        digitalWrite(LPWM_2,LOW);
    }
    else{
        ledcWrite(motor1Channel,255 - abs(motor_left_rpm));
        digitalWrite(LPWM_2,HIGH);
    }

    if(motor_right_rpm > 0){
        ledcWrite(motor1Channel,motor_right_rpm);
        digitalWrite(RPWM_2,LOW);
    }
    else{
        ledcWrite(motor1Channel,255 - abs(motor_right_rpm));
        digitalWrite(RPWM_2,HIGH);
    }

    #else
    if(motor_left_rpm > 0){
        digitalWrite(LPWM_1,LOW);
        digitalWrite(LPWM_2,HIGH);
    }
    else{
        digitalWrite(LPWM_1,HIGH);
        digitalWrite(LPWM_2,LOW);
    }

    if(motor_right_rpm > 0){
        digitalWrite(RPWM_1,LOW);
        digitalWrite(RPWM_2,HIGH);
    }
    else{
        digitalWrite(RPWM_1,HIGH);
        digitalWrite(RPWM_2,LOW);
    }
    ledcWrite(motor3Channel,abs(motor_left_rpm));
    ledcWrite(motor4Channel,abs(motor_left_rpm));
    ledcWrite(motor1Channel,abs(motor_right_rpm));
    ledcWrite(motor2Channel,abs(motor_right_rpm));
    #endif
}
#endif