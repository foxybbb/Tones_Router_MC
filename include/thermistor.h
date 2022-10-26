#ifndef __THERMISTOR_H__
#define __THERMISTOR_H__
#include <Arduino.h>

class thermistor
{

    thermistor::thermistor(byte pin, int resistance, int beta, int tempBase, int resistBase);
    float thermistor::computeTemp(int analog);
    float thermistor::getTemp();
    float thermistor::getTempAverage();
};

#endif // __THERMISTOR_H__