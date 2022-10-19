#include "GCodeParser.h"
#include "Arduino.h"
#include "config.h"
#include "GyverStepper.h"

GCodeParser Gparser = GCodeParser();
GStepper<STEPPER2WIRE> motorX(400, 2, 5, 8);
GStepper<STEPPER2WIRE> motorY(400, 3, 6, 8);
GStepper<STEPPER2WIRE> motorZ(200, 4, 7, 8);

double SlotTemperature;
double TargetTemperature;
bool isHeaterOn = 0;

long getMotorSteps(double distance, int axis)
{
  switch (axis)
  {
  case X_AXIS:
    return (long)(distance * (double)STEPS_PER_MM_X);
    break;
  case Y_AXIS:
    return (long)(distance * (double)STEPS_PER_MM_Y);
    break;
  case Z_AXIS:
    return (long)(distance * (double)STEPS_PER_MM_Z);
    break;

  default:
    return 0;
    break;
  }
}

bool CheckTargetPos(double *target)
{
    if (target[Z_AXIS] > 0 || target[Y_AXIS] < 0 || target[X_AXIS] < 0)
    {
        Serial.print("ERROR");
        Serial.print(":");

        if (target[X_AXIS] < 0)
        {
            Serial.println("X value should be positive");
        }
        else if (target[Y_AXIS] < 0)
        {
            Serial.println("Y value should be positive");
        }
        else if (target[Z_AXIS] > 0)
        {
            Serial.println("Z value should be negative");
        }
        return 0;
    }
    else
        return 1;
}
 // stop all motors
void G_Stop_All(){
   
    motorX.stop();
    motorY.stop();
    motorZ.stop();
}
void G_Stop(int motor){
    switch (motor)
    {
    case 1:
        motorX.stop();
        break;
    case 2:
        motorY.stop();
        break;
    case 3:
        motorZ.stop();
        break;
    default:
        break;
    }
}

void setMotorsFlowPos(){
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);

}

void G0_Move(double *target){
  setMotorsFlowPos();
  motorX.setTarget(getMotorSteps(target[X_AXIS], X_AXIS));
  motorY.setTarget(getMotorSteps(target[Y_AXIS], Y_AXIS));
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
  motorZ.setTarget(getMotorSteps(target[Z_AXIS], Z_AXIS));
}
void G1_Move(double *target){

}


void processGCommand(int commandNumber, double *target)
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
  Serial.println("Error: Unknown G Command");
    break;
  }
}

void processMCommand(int commandNumber)
{
}

void TaskSerialParser(void *pvParameters)
{

    if (Serial.available() > 0)
    {
        double target[] = {0.0, 0.0, 0.0};
        int codeNumber;

        if (Gparser.AddCharToLine(Serial.read()))
        {
            Gparser.ParseLine();

#if defined(DEBUG_MODE)
            Serial.print("Command Line: ");
            Serial.println(Gparser.line);
#endif

            Gparser.RemoveCommentSeparators();

            if (Gparser.HasWord('G'))
            {
                codeNumber = (int)Gparser.GetWordValue('G');
                if (Gparser.HasWord('X'))
                    target[X_AXIS] = (double)Gparser.GetWordValue('X');
                if (Gparser.HasWord('Y'))
                    target[Y_AXIS] = (double)Gparser.GetWordValue('Y');
                if (Gparser.HasWord('Z'))
                    target[Z_AXIS] = (double)Gparser.GetWordValue('Z');

                if (!CheckTargetPos(target))
                {
                    processGCommand(codeNumber, target);
                }
            }
            else
            {
                if (Gparser.HasWord('M')) 
                {
                    codeNumber = (int)Gparser.GetWordValue('M');
                    switch (codeNumber)
                    {
                    case 104: // Set a new target hot end temperature and continue without waiting. 
                                 //The firmware will continue to try to reach and hold the temperature in the background.
                        if (Gparser.HasWord('S'))
                        {
                            TargetTemperature = (double)Gparser.GetWordValue('S');
                            isHeaterOn = true;
                        }
                        else if (Gparser.HasWord('F'))
                            isHeaterOn = false;
                        break;
                    case 105:
                        Serial.print("M105: ");
                        Serial.println(SlotTemperature);
                        break;
                    case 109: // heat and wait need realized
                        if (Gparser.HasWord('S'))  // Set target temperature and wait (if heating up)
                        {
                            TargetTemperature = (double)Gparser.GetWordValue('S');
                            isHeaterOn = true;
                        }
                        else if (Gparser.HasWord('R')) //Set target temperature, wait even if cooling
                        {
                            
                            TargetTemperature = (double)Gparser.GetWordValue('R');
                        }
                        else if (Gparser.HasWord('F'))
                            isHeaterOn = false;
                        break;
                    case 114:
                        Serial.print("M114: ");
                        Serial.print("X:");
                        Serial.print(target[X_AXIS]);
                        Serial.print("Y:");
                        Serial.print(target[Y_AXIS]);
                        Serial.print("Z:");
                        Serial.println(target[Z_AXIS]);
                        break;
                    case 112: // huts down the machine, turns off all the steppers and heaters, and if possible, turns off the power supply. A reset is required to return to operational mode.
                            // emergency stopping
                        motorX.brake();
                        motorY.brake();
                        motorZ.brake();
                    break;
                    default:
                        Serial.println("Error: Unknown M Command");
                        break;
                    }
                }
                else
                    Serial.println("Error: Unknown Command");
            }
        }
    }
}


