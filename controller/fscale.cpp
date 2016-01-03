//
// Created by Aaron on 1/2/2016.
//

#include "fscale.hpp"

float fscale(float inputValue, float originalMin, float originalMax, float newBegin, float newEnd, float curve) {

    // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
    if (originalMin > originalMax) {
        return 0;
    }

    float normalizedCurVal, newRange;

    bool invFlag = false;

    // Check for out of range inputValues
    if (inputValue < originalMin) {
        inputValue = originalMin;

    } else if (inputValue > originalMax) {
        inputValue = originalMax;
    }

    if (newEnd > newBegin) {
        newRange = newEnd - newBegin;

    } else {
        newRange = newBegin - newEnd;
        invFlag = true;
    }

    if (curve > 10) {
        curve = 10;

    } else if (curve < -10) {
        curve = -10;
    }

    // convert linear scale into logarithmic exponent for other pow function
    curve = pow(10, curve * -0.1);

    normalizedCurVal = (inputValue - originalMin) / (originalMax - originalMin);

    if (invFlag) {
        // invert the ranges
        return newBegin - (pow(normalizedCurVal, curve) * newRange);

    } else {
        return (pow(normalizedCurVal, curve) * newRange) + newBegin;
    }
}
