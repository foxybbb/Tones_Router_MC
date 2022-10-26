#ifndef __GCODE_COMANDS_H__
#define __GCODE_COMANDS_H__
//#include "config.h"
#include <Arduino.h>
#include <GCodeParser.h>
#include <GyverStepper.h>

#include "tones_types.h"
#include "motor.h"
#include "config.h"

using GParser = GCodeParser;

// contain in static memory
static GParser Gparser = GCodeParser();
static double TARGET[] = {0.0, 0.0, 0.0};

// Checks if the command is there
bool isCommand(char command) {
  return Gparser.HasWord(command);
}

// Get a value after big chars
double getCommandValue(char command){
  //(double)Gparser.GetWordValue('S')
  return Gparser.GetWordValue(command);
}

// Fills target container by X,Y,Z values
void setTargetPosition() {
        if (Gparser.HasWord('X'))
          TARGET[X_AXIS] = getCommandValue('X');
        if (Gparser.HasWord('Y'))
          TARGET[Y_AXIS] = getCommandValue('Y');
        if (Gparser.HasWord('Z'))
          TARGET[Z_AXIS] = getCommandValue('Z');
}

void G0_Move(Motor &motorX, Motor &motorY, Motor &motorZ,double *target);
void G1_Move(Motor &motorX, Motor &motorY, Motor &motorZ, double *target);
void G28_Move(Motor &motorX, Motor &motorY, Motor &motorZ);

int procesGCommand(int commandNumber, double *target)
{
  switch (commandNumber)
  {

  case 0:
    G0_Move(target);
    Serial.println("G0:OK");
    break;
  case 1:
    G1_Move(target);
    Serial.println("G1:OK");
    break;
  case 28:
    G28_Move();
    Serial.println("G28:OK");
    break;
  default:
    break;
  }
  return 0;
}


#endif // __GCODE_COMANDS_H__