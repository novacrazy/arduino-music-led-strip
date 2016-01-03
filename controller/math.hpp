//
// Created by Aaron on 1/2/2016.
//

#ifndef CONTROLLER_MATH_HPP
#define CONTROLLER_MATH_HPP

#include <Arduino.h>
#include <math.h>

//fscale(inputValue, originalMin, originalMax, newBegin, newEnd, curve) {
inline float fscale(float inputValue, float originalMin, float originalMax, float newBegin, float newEnd, float curve) {
    return newBegin +
           (pow((inputValue - originalMin) / (originalMax - originalMin),
                pow(10, constrain(curve, -10, 10) * -0.1)) * (newEnd - newBegin));
}

inline float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif //CONTROLLER_MATH_HPP
