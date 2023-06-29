/*
  Robotics Pec
  robotics@pec.edu.in

                     GNU GENERAL PUBLIC LICENSE
                        Version 3, 29 June 2007

  Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
  Everyone is permitted to copy and distribute verbatim copies
  of this license document, but changing it is not allowed.

  The GNU General Public License is a free, copyleft license for
  software and other kinds of works.

  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
  APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
  HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
  OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
  IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
  ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
*/

#ifndef BOT_DRIVER
#define BOT_DRIVER
#include <Arduino.h>
#include <sbus.h>

#define AILERON 0
#define ELEVATOR 1
#define THROTTLE 2
#define RUDDER 3
#define ARM 4

#define RPWM_1 22 // pin for RPWM
#define R_EN_1 4  // pin for R_EN

#define LPWM_1 5  // pin for LPWM
#define L_EN_1 13 // pin for L_EN

// pins for motor 2
#define RPWM_2 14 // pin for RPWM
#define R_EN_2 15 // pin for R_EN

#define LPWM_2 21

// pin for LPWM
#define L_EN_2 19 // pin for L_EN

#define CW 1  // don't change
#define CCW 0 // don't change

#define STOP_HARD_MODE 1
#define STOP_SOFT_MODE 0

/////// //////////////////////////////PWM Config
#define PWM_FREQUENCY 50
#define PWM_RESOLUTION 8
#define PWM_MIN 0   // corresponds to 0 degress
#define PWM_MAX 255 // corresponds to 180 degrees

#define motor1Channel 0
#define motor2Channel 1
#define motor3Channel 2
#define motor4Channel 3

///////////////////////////////////LOW pass filter config / integrator

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

#define deadzone 20 // not yet used

int motor_left_rpm_1 = 0;
int motor_left_rpm_2 = 0;
int motor_right_rpm_3 = 0;
int motor_right_rpm_4 = 0;
int16_t forward = 0;
int16_t right = 0;
int16_t rotate_right = 0;

void initialise_driver()
{

    pinMode(R_EN_1, OUTPUT);

    pinMode(L_EN_1, OUTPUT);

    pinMode(R_EN_2, OUTPUT);

    pinMode(L_EN_2, OUTPUT);

    ledcSetup(motor1Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor2Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor3Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(motor4Channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(LPWM_1, motor1Channel);
    ledcAttachPin(RPWM_1, motor2Channel);
    ledcAttachPin(LPWM_2, motor3Channel);
    ledcAttachPin(RPWM_2, motor4Channel);
    // calibrate();
}

void drive_motor(sbus::channel &packet)
{

    // ARM on channel 5
    if ((packet.channelValue[ARM] < 1000) || packet.failsafe)
    {
        motor_left_rpm_1 = 0;
        motor_left_rpm_2 = 0;
        motor_right_rpm_3 = 0;
        motor_right_rpm_4 = 0; // stop all motors if not armed or failsafe
        digitalWrite(R_EN_1, LOW);
        digitalWrite(R_EN_2, LOW);
        digitalWrite(L_EN_1, LOW);
        digitalWrite(L_EN_2, LOW);
        ledcWrite(motor1Channel, motor_left_rpm_1);
        ledcWrite(motor2Channel, motor_left_rpm_2);
        ledcWrite(motor3Channel, motor_right_rpm_3);
        ledcWrite(motor4Channel, motor_right_rpm_4);
    }
    else
    {
        // Serial.println("armed");
        if (!packet.frame_lost)
        {

            if (packet.channelValue[ELEVATOR] >= mid_ch_elevator + deadzone)
            {
                Serial.println("go forward");
                forward = map(packet.channelValue[ELEVATOR], mid_ch_elevator, max_ch_elevator, PWM_MIN, PWM_MAX);
                forward = constrain(forward, PWM_MIN, PWM_MAX);
                // go forward
            }
            else if (packet.channelValue[ELEVATOR] < mid_ch_elevator - deadzone)
            {

                forward = map(packet.channelValue[ELEVATOR], mid_ch_elevator, min_ch_elevator, PWM_MIN, PWM_MAX);
                forward = -constrain(forward, PWM_MIN, PWM_MAX);
                // go backward
            }
            else
            {
                forward = 0;
            }

            if (packet.channelValue[AILERON] >= mid_ch_aileron + deadzone)
            {
                Serial.println("go right");
                right = map(packet.channelValue[AILERON], mid_ch_aileron, max_ch_aileron, PWM_MIN, PWM_MAX);
                right = constrain(right, PWM_MIN, PWM_MAX);
                // go right
            }
            else if (packet.channelValue[AILERON] < mid_ch_aileron - deadzone)
            {

                right = map(packet.channelValue[AILERON], mid_ch_aileron, min_ch_aileron, PWM_MIN, PWM_MAX);
                right = -constrain(right, PWM_MIN, PWM_MAX);
                // go left
            }
            else
            {
                right = 0;
            }

            // handel rotation
            if (packet.channelValue[RUDDER] >= mid_ch_rudder + deadzone)
            {
                Serial.println("rotate right");
                rotate_right = map(packet.channelValue[RUDDER], mid_ch_rudder, max_ch_rudder, PWM_MIN, PWM_MAX);
                rotate_right = constrain(rotate_right, PWM_MIN, PWM_MAX);
                // rotate right
            }
            else if (packet.channelValue[RUDDER] < mid_ch_rudder - deadzone)
            {

                rotate_right = map(packet.channelValue[RUDDER], mid_ch_rudder, min_ch_rudder, PWM_MIN, PWM_MAX);
                rotate_right = -constrain(rotate_right, PWM_MIN, PWM_MAX);
                // rotate left
            }
            else
            {
                rotate_right = 0;
            }

            if (rotate_right > 0)
            {
                // rotate right
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = rotate_right;
                motor_right_rpm_3 = rotate_right;
                motor_right_rpm_4 = 0;
            }
            else if (rotate_right < 0)
            {
                // rotate left
                motor_left_rpm_1 = -rotate_right;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = -rotate_right;
            }
            else if (forward == 0 && right == 0)
            {
                // go nowhere
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = 0;
            }
            else if (forward > 0 && right > 0)
            {
                // go forward right
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = forward;
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = forward - right;
            }
            else if (forward > 0 && right == 0)
            {
                // go forward
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = forward;
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = forward;
            }
            else if (forward > 0 && right < 0)
            {
                // go forward left
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = forward + right; // right is negative so to decrease you have to add
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = forward;
            }
            else if (forward < 0 && right == 0)
            {
                // go backward
                motor_left_rpm_1 = -forward;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = -forward;
                motor_right_rpm_4 = 0;
            }
            else if (forward < 0 && right > 0)
            {
                // go backward right
                motor_left_rpm_1 = -forward;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = -forward - right;
                motor_right_rpm_4 = 0;
            }
            else if (forward < 0 && right < 0)
            {
                // go backward left
                motor_left_rpm_1 = -forward + right;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = -forward;
                motor_right_rpm_4 = 0;
            }
            // else if (forward == 0 && right >= 0)
            // {
            //   // rotate right
            //   motor_left_rpm_1 = 0;
            //   motor_left_rpm_2 = right;
            //   motor_right_rpm_3 = right;
            //   motor_right_rpm_4 = 0;
            // }
            // else if (forward == 0 && right <= 0)
            // {
            //   // rotate left
            //   motor_left_rpm_1 = -right;
            //   motor_left_rpm_2 = 0;
            //   motor_right_rpm_3 = 0;
            //   motor_right_rpm_4 = -right;
            // }
            else
            {
                motor_left_rpm_1 = 0;
                motor_left_rpm_2 = 0;
                motor_right_rpm_3 = 0;
                motor_right_rpm_4 = 0;
            }

            digitalWrite(R_EN_1, HIGH);
            digitalWrite(R_EN_2, HIGH);
            digitalWrite(L_EN_1, HIGH);
            digitalWrite(L_EN_2, HIGH);
            motor_left_rpm_1 = constrain(motor_left_rpm_1, PWM_MIN, PWM_MAX);
            motor_left_rpm_2 = constrain(motor_left_rpm_2, PWM_MIN, PWM_MAX);
            motor_right_rpm_3 = constrain(motor_right_rpm_3, PWM_MIN, PWM_MAX);
            motor_right_rpm_4 = constrain(motor_right_rpm_4, PWM_MIN, PWM_MAX);
            ledcWrite(motor1Channel, motor_left_rpm_1);
            ledcWrite(motor2Channel, motor_left_rpm_2);
            ledcWrite(motor3Channel, motor_right_rpm_3);
            ledcWrite(motor4Channel, motor_right_rpm_4);
        }
    }
    // debugging
    for (int8_t i = 0; i < 5; i++)
    {
        Serial.print(packet.channelValue[i]);
        Serial.print("\t");
    }

    Serial.print(packet.frame_lost);
    Serial.print("\t");

    Serial.println(packet.failsafe);
}

// void calibrate()
// {
//     Serial.println("Rotate sticks in all directions");
//     int max_ch_rudder = 1000, max_ch_throttle = 1000, max_ch_elevator = 1000, max_ch_aileron = 1000;
//     int min_ch_rudder = 1000, min_ch_throttle = 1000, min_ch_elevator = 1000, min_ch_aileron = 1000;

//     unsigned t = millis();
//     while (true and millis() - t < 15000)
//     {
//         if (sbus_rx.Read())
//         {
//             data = sbus_rx.data();
//             Serial.print("Move sticks to all corners   ");
//             Serial.println(t + 15000 - millis());
//             max_ch_aileron = packet.channelValue[AILERON] > max_ch_aileron ? packet.channelValue[AILERON] : max_ch_aileron;
//             max_ch_elevator = packet.channelValue[ELEVATOR] > max_ch_elevator ? packet.channelValue[ELEVATOR] : max_ch_elevator;
//             max_ch_throttle = packet.channelValue[THROTTLE] > max_ch_throttle ? packet.channelValue[THROTTLE] : max_ch_throttle;
//             max_ch_rudder = packet.channelValue[RUDDER] > max_ch_rudder ? packet.channelValue[RUDDER] : max_ch_rudder;

//             min_ch_aileron = packet.channelValue[AILERON] < min_ch_aileron ? packet.channelValue[AILERON] : min_ch_aileron;
//             min_ch_elevator = packet.channelValue[ELEVATOR] < min_ch_elevator ? packet.channelValue[ELEVATOR] : min_ch_elevator;
//             min_ch_throttle = packet.channelValue[THROTTLE] < min_ch_throttle ? packet.channelValue[THROTTLE] : min_ch_throttle;
//             min_ch_rudder = packet.channelValue[RUDDER] < min_ch_rudder ? packet.channelValue[RUDDER] : min_ch_rudder;
//         }
//     }

//     t = millis();
//     while (millis() - t < 10000)
//     {
//         if (sbus_rx.Read())
//         {
//             data = sbus_rx.data();
//             Serial.print("Center all sticks   ");
//             Serial.println(t + 10000 - millis());
//         }
//     }
//     mid_ch_aileron = packet.channelValue[AILERON];
//     mid_ch_elevator = packet.channelValue[ELEVATOR];
//     mid_ch_throttle = packet.channelValue[THROTTLE];
//     mid_ch_rudder = packet.channelValue[RUDDER];

//     Serial.println("Calibration done paste this in code");
//     Serial.print("int max_ch_rudder = ");
//     Serial.print(max_ch_rudder);
//     Serial.println(";");
//     Serial.print("int max_ch_throttle = ");
//     Serial.print(max_ch_throttle);
//     Serial.println(";");
//     Serial.print("int max_ch_elevator = ");
//     Serial.print(max_ch_elevator);
//     Serial.println(";");
//     Serial.print("int max_ch_aileron = ");
//     Serial.print(max_ch_aileron);
//     Serial.println(";");

//     Serial.print("int min_ch_rudder = ");
//     Serial.print(min_ch_rudder);
//     Serial.println(";");
//     Serial.print("int min_ch_throttle = ");
//     Serial.print(min_ch_throttle);
//     Serial.println(";");
//     Serial.print("int min_ch_elevator = ");
//     Serial.print(min_ch_elevator);
//     Serial.println(";");
//     Serial.print("int min_ch_aileron = ");
//     Serial.print(min_ch_aileron);
//     Serial.println(";");

//     Serial.print("int mid_ch_rudder = ");
//     Serial.print(mid_ch_rudder);
//     Serial.println(";");
//     Serial.print("int mid_ch_throttle = ");
//     Serial.print(mid_ch_throttle);
//     Serial.println(";");
//     Serial.print("int mid_ch_elevator = ");
//     Serial.print(mid_ch_elevator);
//     Serial.println(";");
//     Serial.print("int mid_ch_aileron = ");
//     Serial.print(mid_ch_aileron);
//     Serial.println(";");

//     while (true)
//     {
//     }
// }
#endif