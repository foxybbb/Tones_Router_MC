#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "GyverStepper.h"
#include "config.h"
#include "tones_types.h"

using Motor = GStepper<STEPPER2WIRE>;

static Motor motorX(400, 2, 5, 8);
static Motor motorY(400, 3, 6, 8);
static Motor motorZ(200, 4, 7, 8);

// Setup settings
void setupReverse() {
  // reverse each motor
  motorX.reverse(true);
  motorY.reverse(true);
}
void setupAcceleration() {
   // set acleration and max speed for each axis
  // Ускорение = const
  motorX.setAcceleration(DEFAULT_ACELERATION);
  motorY.setAcceleration(DEFAULT_ACELERATION);
  motorZ.setAcceleration(Z_DEFAULT_ACCELERATION);
}
void setupMaxSpeed() {
  // set max speed for each axis
  motorX.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorY.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
}
void setupPinMode() {
  // Ports settings
  pinMode(LIMSW_X_PIN, INPUT);
  pinMode(LIMSW_Y_PIN, INPUT);
  pinMode(LIMSW_Z_PIN, INPUT);
}

void G0_Move(double *target)
{
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
  motorX.setTarget(stepsToMM_X(target[X_AXIS]));
  motorY.setTarget(stepsToMM_Y(target[Y_AXIS]));
  motorZ.setTarget(stepsToMM_Z(target[Z_AXIS]));
}
void G1_Move(double *target){
    STATE = MOVING;
    motorX.setRunMode(FOLLOW_POS);
    motorY.setRunMode(FOLLOW_POS);
    motorZ.setRunMode(FOLLOW_POS);
    motorZ.setTarget(-stepsToMM_Z(5));
    while (motorZ.tick())
    {
    }

    motorX.setTarget(stepsToMM_X(target[X_AXIS]));
    while (motorX.tick())
    {
      
    }
    motorY.setTarget(stepsToMM_Y(target[Y_AXIS]));
    while (motorY.tick())
    {
      
    }
    motorZ.setTarget(stepsToMM_Z(target[Z_AXIS]));
    while (motorZ.tick())
    {
      
    }
    STATE = IDLE;

}
void G28_Move()
{
  motorZ.setRunMode(KEEP_SPEED);
  if (digitalRead(LIMSW_Z_PIN) == HIGH)
  {
    motorZ.setSpeed(1000);
    while (digitalRead(LIMSW_Z_PIN) == HIGH)
    {
      motorZ.tick();
    }
    motorZ.brake();
  }
  motorZ.reset();
  motorZ.setRunMode(FOLLOW_POS);
  motorZ.setTarget(stepsToMM_Z(-1));

  motorX.setRunMode(KEEP_SPEED);
  if (digitalRead(LIMSW_X_PIN) == HIGH)
  {
    motorX.setSpeed(200);
    while (digitalRead(LIMSW_X_PIN) == HIGH)
    {
      motorX.tick();
    }
    motorX.brake();
    motorX.reset();
  }
  motorX.setRunMode(FOLLOW_POS);
  motorX.setTarget(stepsToMM_X(-20));

  motorX.setTarget(stepsToMM_X(-365));
  motorY.setRunMode(KEEP_SPEED);
  if (digitalRead(LIMSW_Y_PIN) == HIGH)
  {
    motorY.setSpeed(-200);
    while (digitalRead(LIMSW_Y_PIN) == HIGH)
    {
      motorY.tick();
    }
    motorY.brake();
    motorY.reset();
  }
  motorY.setRunMode(FOLLOW_POS);
  motorY.setTarget(stepsToMM_X(5));
  while (motorX.tick() )
  {
    
    motorY.tick();
  } 
  while (motorY.tick() )
  {
    motorX.tick();
  } 

  motorX.reset();
  motorY.reset();
}

#endif // __MOTOR_H__