#include <Arduino.h>
#include "GyverStepper.h"
#include <GCodeParser.h>
#include <GyverPID.h>
#include <GyverNTC.h>
#include <GyverTimers.h>
#include "config.h"
#include "tones_types.h"

using GParser = GCodeParser;
using Motor = GStepper<STEPPER2WIRE>;

struct {
  double SlotTemperature;
  double TargetTemperature;

  bool isHeaterOn = 0;
  bool isOnTarget = 0;
} Temperature;

/*class Motors{
private:
  // For readable
  using Motor = GStepper<STEPPER2WIRE>;

  // Empty memory
  Motor* motorX = nullptr;
  Motor* motorY = nullptr;
  Motor* motorZ = nullptr;

public:
  Motors() {
    defaultSettings();
  }

  void defaultSettings() {
    motorX = new Motor(400, 2, 5, 8);
    motorY = new Motor(400, 3, 6, 8);
    motorZ = new Motor(200, 4, 7, 8);
  }

  // Empty memory
  ~Motors() {
    if(motorX) delete motorX;
    if(motorY) delete motorY;
    if(motorZ) delete motorZ;
  }

  // Are not used yet
  void setMotorX(int stepsPerRev = 400, uint8_t pin1 = 2, uint8_t pin2 = 5, uint8_t pin3 = 8);
  void setMotorY(int stepsPerRev = 400, uint8_t pin1 = 3, uint8_t pin2 = 6, uint8_t pin3 = 8);
  void setMotorZ(int stepsPerRev = 200, uint8_t pin1 = 4, uint8_t pin2 = 7, uint8_t pin3 = 8);

  // get any motor to work with it
  Motor* getMotorX() const {
    return motorX;
  }
  Motor* getMotorY() const {
    return motorY;
  }
  Motor* getMotorZ() const {
    return motorZ;
  }
};
*/

static State STATE = State::UNKNOWN_POSITION;

static double TARGET[] = {0.0, 0.0, 0.0};

// contain in static memory
static GParser Gparser = GCodeParser();

static Motor motorX(400, 2, 5, 8);
static Motor motorY(400, 3, 6, 8);
static Motor motorZ(200, 4, 7, 8);

// parsing
void parsing();

// Fills target container by X,Y,Z values
void setTargetPosition();

// Setup settings
void setupReverse();
void setupAcceleration();
void setupMaxSpeed();
void setupPinMode();

// Checks if the command is there
bool isCommand(char command);

// Get a value after big chars
double getCommandValue(char command);

// Cpommands
int procesGCommand(int commandNumber, double *target);
int procesMCommand(char *str[]);

void setup()
{
  STATE = State::UNKNOWN_POSITION;
  Serial.begin(PORT_BOUDRATE);

  // reverse each motor
  setupReverse();

  // set acleration and max speed for each axis
  setupAcceleration();

  // set max speed for each axis
  setupMaxSpeed();

  // Ports settings
  setupPinMode();
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

      if (isCommand('G'))
      {
        //G1X100Y100Z234
        codeNumber = (int)getCommandValue('G');

        setTargetPosition();

        procesGCommand(codeNumber, TARGET);
      }
      else
      {
        // M-commands (M(дополнительные) and G(основные) commands)
        // Settings, pos, temperature
        if (isCommand('M'))
        {
          codeNumber = (int)getCommandValue('M');
          switch (codeNumber)
          {
          case 104: // Set a new target hot end temperature and continue without waiting.
                    // The firmware will continue to try to reach and hold the temperature in the background.
            if (isCommand('S'))
            {
              Temperature.TargetTemperature = getCommandValue('S');
              Temperature.isHeaterOn = true;
            }
            else if (isCommand('F'))
              Temperature.isHeaterOn = false;
            break;
          case 105:
            Serial.print("M105: ");
            Serial.println(Temperature.SlotTemperature);
            break;
          case 109:                   // heat and wait need realized
            if (isCommand('S')) // Set target temperature and wait (if heating up)cНагрев
            {
              Temperature.TargetTemperature = getCommandValue('S');
              Temperature.isHeaterOn = true;
            }
            else if (isCommand('R')) // Set target temperature, wait even if cooling ждать когда охлодится
            {

              Temperature.TargetTemperature = getCommandValue('R');
            }
            else if (isCommand('F')) // Off
              Temperature.isHeaterOn = false;
            break;
          case 114:
            Serial.print("M114: ");
            Serial.print("X:");
            Serial.print(TARGET[X_AXIS]);
            Serial.print("Y:");
            Serial.print(TARGET[Y_AXIS]);
            Serial.print("Z:");
            Serial.println(TARGET[Z_AXIS]);
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
    STATE = State::MOVING;
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
    STATE = State::IDLE;

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
