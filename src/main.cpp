#include <Arduino.h>
#include "GyverStepper.h"
#include <GCodeParser.h>
#include <GyverPID.h>
#include <GyverNTC.h>
#include <GyverTimers.h>
#include "config.h"
#include "tones_types.h"

double SlotTemperature;
double TargetTemperature;
bool isHeaterOn = 0;
bool isOnTarget = 0;
GCodeParser Gparser = GCodeParser();
GStepper<STEPPER2WIRE> motorX(400, 2, 5, 8);
GStepper<STEPPER2WIRE> motorY(400, 3, 6, 8);
GStepper<STEPPER2WIRE> motorZ(200, 4, 7, 8);

// GyverPID slotTemp(5, 0.05, 0);
// GyverNTC therm(A3, 100000, 3950);

State state = State::UNKNOWN_POSITION;

double target[] = {0.0, 0.0, 0.0};

void parsing();
int procesGCommand(int commandNumber, double *target);

int procesMCommand(char *str[]);
void setup()
{
  Serial.begin(PORT_BOUDRATE);

  // reverse each motor
  motorX.reverse(true);
  motorY.reverse(true);

  // set acleration and max speed for each axis
  // Ускорение = const
  motorX.setAcceleration(DEFAULT_ACELERATION);
  motorY.setAcceleration(DEFAULT_ACELERATION);
  motorZ.setAcceleration(Z_DEFAULT_ACCELERATION);

  // set max speed for each axis
  motorX.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorY.setMaxSpeed(DEFAULT_MAX_SPEED);
  motorZ.setMaxSpeed(Z_DEFAULT_MAX_SPEED);

  state = State::UNKNOWN_POSITION;

  // Ports settings
  pinMode(LIMSW_X_PIN, INPUT);
  pinMode(LIMSW_Y_PIN, INPUT);
  pinMode(LIMSW_Z_PIN, INPUT);

  // slotTemp.setDirection(NORMAL); // Heat
  // slotTemp.setLimits(0, 255); // ограничение шим

  // slotTemp.setpoint = 70;

  // настраиваем прерывания с периодом, при котором
  // система сможет обеспечить максимальную скорость мотора.
  // Для большей плавности лучше лучше взять период чуть меньше, например в два раза
  
  Serial.println("Setup:OK");
}



void loop()
{
  parsing();
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
   state = State::MOVING;
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
    state = State::IDLE;

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

void parsing()
{
  if (Serial.available() > 0)
  {

    int codeNumber;

    // Get lines
    if (Gparser.AddCharToLine(Serial.read()))
    {
      Gparser.ParseLine();

        // Del cooments
      Gparser.RemoveCommentSeparators();

      if (Gparser.HasWord('G'))
      {
        //G1X100Y100Z234
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
        // M-commands (M(дополнительные) and G(основные) commands)
        // Settings, pos, temperature
        if (Gparser.HasWord('M'))
        {
          codeNumber = (int)Gparser.GetWordValue('M');
          switch (codeNumber)
          {
          case 104: // Set a new target hot end temperature and continue without waiting.
                    // The firmware will continue to try to reach and hold the temperature in the background.
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
          case 109:                   // heat and wait need realized
            if (Gparser.HasWord('S')) // Set target temperature and wait (if heating up)cНагрев
            {
              TargetTemperature = (double)Gparser.GetWordValue('S');
              isHeaterOn = true;
            }
            else if (Gparser.HasWord('R')) // Set target temperature, wait even if cooling ждать когда охлодится
            {

              TargetTemperature = (double)Gparser.GetWordValue('R');
            }
            else if (Gparser.HasWord('F')) // Off
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
