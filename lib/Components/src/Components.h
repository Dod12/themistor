//
// Created by Daniel on 2021-08-06.
//

#ifndef Components_h
#define Components_h
#include <PCF8574.h>
#include <arduino.h>

class LED {
public:
    LED(const PCF8574 &interf, int pin);
    void writeState(bool state);
    bool readState();
private:
    int pin;
    PCF8574 interface;
    bool state = true;
};

class Button {
public:
    Button(const PCF8574 &interf, int pin, int i_pin);
    bool readState();
    // void interruptHandler(); Let's handle this somewhere else for now.
private:
    int pin;
    // int interrupt_pin;
    PCF8574 interface;
    bool state;
};

class Thermistor {
public:
    Thermistor(int pin, float calibration, float r1, double c1, double c2, double c3, bool v);
    float getTemp(int nSamples);
    void setCalibration(float calibration);
    void setConstants(double c1, double c2, double c3);
    void setResistance(float r1);
private:
    int pin;
    float calibration, r1;
    double c1, c2, c3;
    bool verbose;
};

class Fan {
public:
    Fan(int pwm_pin, int tach_pin, int min_speed, int max_speed);
    void setSpeed(int speed);
    int getSpeed();
    void setPWM(int pwm);
    int getPWM();
private:
    int pwm_pin;
    int tach_pin;
    int min_speed;
    int max_speed;
    int pwm;
    int min_pwm;
};

#endif

