#include <Arduino.h>
#include <GCodeParser.h>
#include "GyverStepper.h"
#include <GyverPID.h>
#include <GyverNTC.h>
#include <GyverTimers.h>

#include "motor.h"
#include "tones_types.h"
#include "config.h"
#include "parser.h"

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

// Cpommands
int procesMCommand(char *str[]);

void setup()
{
  STATE = UNKNOWN_POSITION;
  Serial.begin(PORT_BOUDRATE);

  setupReverse();
  setupAcceleration();
  setupMaxSpeed();
  setupPinMode();

  Serial.println("Setup:OK");
  // slotTemp.setDirection(NORMAL); // Heat
  // slotTemp.setLimits(0, 255); // ограничение шим

  // slotTemp.setpoint = 70;

  // настраиваем прерывания с периодом, при котором
  // система сможет обеспечить максимальную скорость мотора.
  // Для большей плавности лучше лучше взять период чуть меньше, например в два раза
  
}

void loop()
{
  parsing();
}

