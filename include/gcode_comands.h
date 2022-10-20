#ifndef __GCODE_COMANDS_H__
#define __GCODE_COMANDS_H__
#include "config.h"

void G0_Move(GStepper<STEPPER2WIRE> &motorX,GStepper<STEPPER2WIRE> &motorY,GStepper<STEPPER2WIRE> &motorZ,double *target);

void G1_Move(GStepper<STEPPER2WIRE> &motorX, GStepper<STEPPER2WIRE> &motorY, GStepper<STEPPER2WIRE> &motorZ, double *target);

void G28_Move(GStepper<STEPPER2WIRE> &motorX, GStepper<STEPPER2WIRE> &motorY, GStepper<STEPPER2WIRE> &motorZ);



#endif // __GCODE_COMANDS_H__