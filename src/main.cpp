#include <Arduino.h>
#include <GCodeParser.h>
#include <GyverPID.h>
#include <GyverNTC.h>
#include <GyverTimers.h>
#include "GyverStepper.h"
#include "motor.h"
#include "tones_types.h"
#include "config.h"
#include "parser.h"
#include "setup.h"

void setup()
{
  STATE = UNKNOWN_POSITION;
  Serial.begin(PORT_BOUDRATE);

  setupTimer();
  setupHeter();

  setupReverse();
  setupAcceleration();
  setupMaxSpeed();
  setupPinMode();
  
  Serial.println("Setup:OK");
}

void loop()
{
  parsing();
}
