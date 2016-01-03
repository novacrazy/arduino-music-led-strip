//
// Created by Aaron on 1/2/2016.
//

#ifndef CONTROLLER_FSCALE_HPP
#define CONTROLLER_FSCALE_HPP

/* fscale
 Floating Point Autoscale Function V0.1
 Paul Badger 2007
 Modified from code by Greg Shakar

 This function will scale one set of floating point numbers (range) to another set of floating point numbers (range)
 It has a "curve" parameter so that it can be made to favor either the end of the output. (Logarithmic mapping)

 It takes 6 parameters

 originalMin - the minimum value of the original range - this MUST be less than origninalMax
 originalMax - the maximum value of the original range - this MUST be greater than orginalMin

 newBegin - the end of the new range which maps to orginalMin - it can be smaller, or larger, than newEnd, to facilitate inverting the ranges
 newEnd - the end of the new range which maps to originalMax  - it can be larger, or smaller, than newBegin, to facilitate inverting the ranges

 inputValue - the variable for input that will mapped to the given ranges, this variable is constrained to originaMin <= inputValue <= originalMax
 curve - curve is the curve which can be made to favor either end of the output scale in the mapping. Parameters are from -10 to 10 with 0 being
          a linear mapping (which basically takes curve out of the equation)

 To understand the curve parameter do something like this:

 void loop(){
  for ( j=0; j < 200; j++){
    scaledResult = fscale( 0, 200, 0, 200, j, -1.5);

    Serial.print(j, DEC);
    Serial.print("    ");
    Serial.println(scaledResult, DEC);
  }
}

And try some different values for the curve function - remember 0 is a neutral, linear mapping

To understand the inverting ranges, do something like this:

 void loop(){
  for ( j=0; j < 200; j++){
    scaledResult = fscale( 0, 200, 200, 0, j, 0);

    //  Serial.print lines as above

  }
}

*/


#include <math.h>

//fscale(inputValue, originalMin, originalMax, newBegin, newEnd, curve) {
float fscale(float, float, float, float, float, float = 0.0);

#endif //CONTROLLER_FSCALE_HPP
