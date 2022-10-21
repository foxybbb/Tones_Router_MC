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

// Fills target container by X,Y,Z values
void setTargetPosition();

// Checks if the command is there
bool isCommand(char command);

// Get a value after big chars
double getCommandValue(char command);

void G0_Move(Motor &motorX, Motor &motorY, Motor &motorZ,double *target);
void G1_Move(Motor &motorX, Motor &motorY, Motor &motorZ, double *target);
void G28_Move(Motor &motorX, Motor &motorY, Motor &motorZ);

int procesGCommand(int commandNumber, double *target);

#endif // __GCODE_COMANDS_H__