#include <Arduino.h>
#include "GyverStepper.h"
#include "GyverPlanner.h"
#include "GCodeParser.h"
#include <GyverPID.h>
#include <GyverNTC.h>

#define DEBUG_MODE 0
#define PORT_BOUDRATE 115200
// software configuration
#define MAX_X_POSITION 250
#define MAX_Y_POSITION 100
#define MAX_Z_POSITION 200

int STEPS_PER_MM_X = 10;
int STEPS_PER_MM_Y = 15;
int STEPS_PER_MM_Z = 400;

double SlotTemperature;
double TargetTemperature;
bool isHeaterOn = 0;

#define DEFAULT_ACELERATION 100
#define Z_DEFAULT_ACCELERATION 800
#define DEFAULT_MAX_SPEED 800
#define Z_DEFAULT_MAX_SPEED 3000

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define X_ENDSTOP_PIN 9
#define Y_ENDSTOP_PIN 10
#define Z_ENDSTOP_PIN 12

enum State
{
  IDLE,
  MOVING,
  HOMING,
  WAITING,
  DONE,
  UNKNOWN_POSITION
};

GCodeParser Gparser = GCodeParser();
GStepper<STEPPER2WIRE> motorX(400, 2, 5, 8);
GStepper<STEPPER2WIRE> motorY(400, 3, 6, 8);
GStepper<STEPPER2WIRE> motorZ(200, 4, 7, 8);

GyverPID slotTemp(5, 0.05, 0);
GyverNTC therm(A3, 100000, 3950);

State state = State::UNKNOWN_POSITION;

double target[] = {0, 0, 0};

int autoHome();
int parsing();
int procesGCommand(int commandNumber, double *target);
int moveAxisTo(double *target);
int procesMCommand(char *str[]);
void setup()
{
  Serial.begin(PORT_BOUDRATE);
  // reverse each motor
  motorX.reverse(true);
  motorY.reverse(true);
  // set acleration and max speed for each axis
  motorX.setAcceleration(DEFAULT_ACELERATION);
  motorY.setAcceleration(DEFAULT_ACELERATION);
  motorZ.setAcceleration(Z_DEFAULT_ACCELERATION);

  // set max speed for each axis
  motorX.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorY.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);

  state = State::UNKNOWN_POSITION;

  pinMode(X_ENDSTOP_PIN, INPUT);
  pinMode(Y_ENDSTOP_PIN, INPUT);
  pinMode(Z_ENDSTOP_PIN, INPUT);

  // slotTemp.setDirection(NORMAL); // Нагрев
  // slotTemp.setLimits(0, 255); // ограничение шим

  // slotTemp.setpoint = 70;
  Serial.println("Setup:OK");
}
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
unsigned long timer = 0;
void loop()
{

  // if(millis() - timer >= 100){
  //   slotTemp.input = therm.getTempAverage();
  //   timer = millis();

  //   analogWrite(,slotTemp.getResult()) ;
  //   Serial.print(therm.getTempAverage());
  //   Serial.print('\t');
  //   Serial.println(slotTemp.getResult());

  // }

  parsing();
}

int autoHome()
{
  state = State::HOMING;
  motorZ.setRunMode(KEEP_SPEED);
  motorZ.setSpeed(1000);
  while (digitalRead(Z_ENDSTOP_PIN) == HIGH)
  {
    motorZ.tick();
  }
  motorZ.brake();
  motorZ.reset();
  motorZ.setRunMode(FOLLOW_POS);
  long pos = getMotorSteps(-5.0, Z_AXIS);
  motorZ.setTarget(pos);

  while (motorZ.getCurrent() > pos)
  {
    motorZ.tick();
  }

  motorX.setRunMode(KEEP_SPEED);
  motorX.setSpeed(200);

  while (digitalRead(X_ENDSTOP_PIN) == HIGH)
  {
    motorX.tick();
  }
  motorX.brake();
  motorX.reset();
  motorX.setRunMode(FOLLOW_POS);
  pos = getMotorSteps(-20.0, X_AXIS);
  motorX.setTarget(pos);
  while (motorX.getCurrent() > pos)
  {
    motorX.tick();
  }

  motorY.setRunMode(KEEP_SPEED);
  motorY.setSpeed(-200);
  while (digitalRead(Y_ENDSTOP_PIN) == HIGH)
  {
    motorY.tick();
  }
  motorY.brake();
  motorY.reset();
  motorY.setRunMode(FOLLOW_POS);
  pos = getMotorSteps(20.0, Y_AXIS);
  motorY.setTarget(pos);
  while (motorY.getCurrent() > pos)
  {
    motorY.tick();
  }

  long posX = getMotorSteps(-365.0, X_AXIS);
  long posY = getMotorSteps(5.0, Y_AXIS);
  motorX.setTarget(posX);
  motorY.setTarget(posY);

  while (motorX.tick())
  {
    motorY.tick();
  }
  motorX.reset();
  motorY.reset();

  posX = getMotorSteps(10.0, X_AXIS);
  posY = getMotorSteps(10.0, Y_AXIS);
  motorX.setTarget(posX);
  motorY.setTarget(posY);
  while (motorX.tick())
  {
    motorY.tick();
  }
  state = State::IDLE;
  return 0;
}
int parsing()
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

               
                    procesGCommand(codeNumber, target);
                
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

int moveAxisTo(double *target)
{
  state = State::MOVING;
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);
  motorX.setTarget(getMotorSteps(target[X_AXIS], X_AXIS));
  motorY.setTarget(getMotorSteps(target[Y_AXIS], Y_AXIS));
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);
  motorZ.setTarget(getMotorSteps(target[Z_AXIS], Z_AXIS));
  return 0;
}
int moveToAxisAndWait(double *target)
{
  state = State::MOVING;
  motorX.setRunMode(FOLLOW_POS);
  motorY.setRunMode(FOLLOW_POS);
  motorZ.setRunMode(FOLLOW_POS);
  motorZ.setTarget(-getMotorSteps(5, Z_AXIS));
  while (motorZ.tick())
  {
  }

  motorX.setTarget(getMotorSteps(target[X_AXIS], X_AXIS));
  while (motorX.tick())
  {
  }
  motorY.setTarget(getMotorSteps(target[Y_AXIS], Y_AXIS));
  while (motorY.tick())
  {
  }
  motorZ.setTarget(getMotorSteps(target[Z_AXIS], Z_AXIS));
  while (motorZ.tick())
  {
  }
  state = State::IDLE;
  return 0;
}
int procesGCommand(int commandNumber, double *target)
{
  switch (commandNumber)
  {
    
  case 0:
    return moveAxisTo(target);
    Serial.println("G0:OK");
    break;
  case 1:
    return moveToAxisAndWait(target);
    Serial.println("G1:OK");
    break;
  case 28:
    return autoHome();
    Serial.println("G28:OK");
    break;
  default:
    break;
  }
  return 0;
}

int procesMCommand(int code)
{
   
    

}
