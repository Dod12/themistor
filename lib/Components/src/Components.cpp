//
// Created by Daniel on 2021-08-06.
//

#include <math.h>
#include "Components.h"

/*
 *  LED CLASS
 */

LED::LED(const PCF8574 &interf, int p) {
    interface = interf;
    pin = p;
}

void LED::writeState(bool s) {
    state = s;
    interface.write(pin, (int) state);
}

bool LED::readState() {
    state = (bool) interface.read(pin);
    return state;
}

/*
 *  BUTTON CLASS
 */

Button::Button(const PCF8574 &interf, int p, int i_pin) {
    interface = interf;
    // interrupt_pin = i_pin;
    pin = p;
    state = (bool) interface.read(pin);
}

bool Button::readState() {
    return (bool) interface.read(pin);
}

/*
 *  THERMISTOR CLASS
 */

Thermistor::Thermistor(int pin, float calibration, float r1, double c1, double c2, double c3, bool verbose) {
    this->pin = pin;
    this->calibration = calibration;
    this->r1 = r1;
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;
    this->verbose = verbose;
}

/*
 *  THERMISTOR CLASS
 */

float Thermistor::getTemp(int nSamples) {
    double T = 0;
    for (int i = 0; i < nSamples; i++) {
        double Vo = analogRead(pin);
        // Because the Vref for the ADC can float quite a lot when using USB power, we need a calibration constant here
        // to adjust the input voltage to match the actual voltage. This is best done with a Voltmeter parallel to the
        // fixed resistor to get the difference between the ADC (Vo) and the Voltmeter (Vm). Then, the calibration
        // constant is Vo / Vm.
        Vo *= calibration;
        double r2 = r1 * (1023.0 / Vo - 1.0);
        double logR2 = log(r2);
        // Convert resistance to degrees Celsius using Steinhart-Hart equations.
        T += (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)) - 273.15;
        delay(100); // Measure several times with a 0.1s delay.
        if (verbose) {
            Serial.print(T, DEC);
            Serial.print(",");
            Serial.print(r2, DEC);
            Serial.print(",");
            Serial.println(Vo/1024.0 * 5.0, DEC);
        }
    }
    return (float)(T / (float)nSamples);
}

void Thermistor::setCalibration(float c) {
    calibration = c;
}

void Thermistor::setConstants(double const1, double const2, double const3) {
    c1 = const1;
    c2 = const2;
    c3 = const3;
}

void Thermistor::setResistance(float r) {
    r1 = r;
}

Fan::Fan(int pwm_p, int tach_p, int min_s, int max_s) {
    this->pwm_pin = pwm_p;
    this->tach_pin = tach_p;
    this->min_speed = min_s;
    this->max_speed = max_s;
    analogWrite(pwm_pin, 0);
    this->pwm = 0;
    this->min_pwm = (int) ((255/this->max_speed) * this->min_speed);
}

void Fan::setSpeed(int speed) {
    if (speed <= min_speed) {
        this->pwm = 0;
    } else if (min_speed < speed < max_speed) {
        this->pwm = (speed - min_speed) / (max_speed - min_speed);
    } else if (speed >= max_speed) {
        this->pwm = 255;
    }
    analogWrite(this->pwm_pin, this->pwm);
}

void Fan::setPWM(int pwm) {
    if (pwm < this->min_pwm) {
        //Serial.println(this->min_pwm);
        analogWrite(this->pwm_pin, this->min_pwm);
    } else if (this->min_pwm < pwm < 255) {
        //Serial.println(pwm);
        analogWrite(this->pwm_pin, pwm);
    } else if (this->pwm > 255) {
        //Serial.println(255);
        analogWrite(this->pwm_pin, 255);
    }
}
