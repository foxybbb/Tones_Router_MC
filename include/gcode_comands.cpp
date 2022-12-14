#include "gcode_comands.h"
#include <GyverStepper.h>
#include <Arduino.h>

void G0_Move(GStepper<STEPPER2WIRE> &motorX, GStepper<STEPPER2WIRE> &motorY, GStepper<STEPPER2WIRE> &motorZ, double *target)
{
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
  motorX.setTarget(stepsToMM_X(target[X_AXIS]));
  motorY.setTarget(stepsToMM_Y(target[Y_AXIS]));
  motorX.setTarget(stepsToMM_Z(target[Z_AXIS]));
}

void G1_Move(GStepper<STEPPER2WIRE> &motorX, GStepper<STEPPER2WIRE> &motorY, GStepper<STEPPER2WIRE> &motorZ, double *target)
{
}

void G28_Move(GStepper<STEPPER2WIRE> &motorX, GStepper<STEPPER2WIRE> &motorY, GStepper<STEPPER2WIRE> &motorZ)
{
  motorZ.setRunMode(KEEP_SPEED);
  motorZ.setSpeed(1000);
  if (digitalRead(LIMSW_X_PIN))
  {
    motorZ.setSpeed(-10);
    while (digitalRead(LIMSW_X_PIN))
    {
    }
    motorZ.brake();
  }
  motorZ.reset();
}