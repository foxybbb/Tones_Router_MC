#include "gcode_comands.h"

// Fills target container by X,Y,Z values
void setTargetPosition() {
        if (Gparser.HasWord('X'))
          TARGET[X_AXIS] = getCommandValue('X');
        if (Gparser.HasWord('Y'))
          TARGET[Y_AXIS] = getCommandValue('Y');
        if (Gparser.HasWord('Z'))
          TARGET[Z_AXIS] = getCommandValue('Z');
}

// Checks if the command is there
bool isCommand(char command) {
  return Gparser.HasWord(command);
}

// Get a value after big chars
double getCommandValue(char command){
  //(double)Gparser.GetWordValue('S')
  return Gparser.GetWordValue(command);
}

void G0_Move(Motor &motorX, Motor &motorY, Motor &motorZ, double *target)
{
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
  motorX.setTarget(stepsToMM_X(target[X_AXIS]));
  motorY.setTarget(stepsToMM_Y(target[Y_AXIS]));
  motorX.setTarget(stepsToMM_Z(target[Z_AXIS]));
}
void G1_Move(Motor &motorX, Motor &motorY, Motor &motorZ, double *target)
{
}
void G28_Move(Motor &motorX, Motor &motorY, Motor &motorZ)
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
