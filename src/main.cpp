#include <Arduino.h>
#include <PCF8574.h>
#include "../lib/Components/src/Components.h"

/* Pinout on board:
 *
 * Fans:
 *
 * TACH1:   D2
 * PWM1:    D3
 * TACH2:   D4
 * PWM2:    D5
 * TACH3:   D7
 * PWM3:    D6
 * TACH4:   D8
 * PWM4:    D9
 *
 * Thermistors:
 *
 * TC0:     A3
 * TC1:     A2
 * TC2:     A1
 * TC3:     A6
 * TC4:     A0
 *
 * Serial:
 *
 * INT:     A5
 * SCL:     A7
 * SDA:     A8
 */

// Define pins

#define P_TACH1   2
#define P_PWM1    3
#define P_TACH2   4
#define P_PWM2    5
#define P_TACH3   7
#define P_PWM3    6
#define P_TACH4   8
#define P_PWM4    9
#define P_TC0     A3
#define P_TC1     A2
#define P_TC2     A1
#define P_TC3     A6
#define P_TC4     A0

// Define constant parameters

#define MIN_TEMP 20
#define MAX_TEMP 40
#define CALIBRATION_CONSTANT 0.91 //1.3850415512 //0.722 //1.2962716378 //1.362183755
#define N_SAMPLES 100
#define FAN_SPEED 1500

TwoWire W = TwoWire();
PCF8574 Interface = PCF8574(0x20, &W);
LED COOL = LED(Interface, 0);
LED MID = LED(Interface, 1);
LED HOT = LED(Interface, 2);
Thermistor T1 = Thermistor(P_TC1, CALIBRATION_CONSTANT, 100.0,
                           1.618689044e-03, 4.374086885e-04, -18.50448816e-07, false);
struct Fan_struct {
    Fan Fan_1 = Fan(P_PWM1, P_TACH1, 600, 1500);
    Fan Fan_2 = Fan(P_PWM2, P_TACH2, 600, 1500);
    Fan Fan_3 = Fan(P_PWM3, P_TACH3, 600, 1500);
    Fan Fan_4 = Fan(P_PWM4, P_TACH4, 600, 1500);
    void setPWM (int pwm) {
        Fan_1.setPWM(pwm);
        Fan_2.setPWM(pwm);
        Fan_3.setPWM(pwm);
        Fan_4.setPWM(pwm);
    }
    Fan_struct() {
        pinMode(P_PWM1, OUTPUT);
        pinMode(P_PWM2, OUTPUT);
        pinMode(P_PWM3, OUTPUT);
        pinMode(P_PWM4, OUTPUT);
        pinMode(P_TACH1, INPUT);
        pinMode(P_TACH2, INPUT);
        pinMode(P_TACH3, INPUT);
        pinMode(P_TACH4, INPUT);
    };
};

Fan_struct Fans = Fan_struct();

void setup() {

    Serial.begin(9600);
    Serial.println("T,R,U");
    Interface.begin();
}

void loop() {
    float T = T1.getTemp(N_SAMPLES);
    Serial.print("Temperature: ");
    Serial.print(T, DEC);
    // Constrain to the range 20-40°C, and range between 0.0 and 1.0.
    auto Tr = (float) (constrain(T, MIN_TEMP, MAX_TEMP) - MIN_TEMP) / (MAX_TEMP - MIN_TEMP);
    Fans.setPWM((int) (Tr*255.0));
    Serial.print(", Fan: ");
    Serial.println((int) ((Tr*255.0)), DEC);
    if (T <= 24.0) {
        Serial.println("LOW");
        //INT.write(0,1);
        //INT.write(1,0);
        //INT.write(2,0);
        COOL.writeState(true);
        MID.writeState(false);
        HOT.writeState(false);
    } else if (24.0 < T && T <= 26.0) {
        Serial.println("MID");
        //INT.write(0,0);
        //INT.write(1,1);
        //INT.write(2,0);
        COOL.writeState(false);
        MID.writeState(true);
        HOT.writeState(false);
    } else if (T > 26.0) {
        Serial.println("HIGH");
        //INT.write(0,0);
        //INT.write(1,0);
        //.write(2,1);
        COOL.writeState(false);
        MID.writeState(false);
        HOT.writeState(true);
    }
}