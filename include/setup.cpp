
#include "setup.h"
#include <GyverTimers.h>
#include <GyverPID.h>
#include "PIDtuner2.h"
#include <OneWire.h>
#include "config.h"
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

PIDtuner2 tuner;
GyverPID slotHeater(10.43, 123, 12);

ISR(TIMER1_A)
{
  sensors.requestTemperatures();
  Temperature.SlotTemperature = sensors.getTempCByIndex(0);
  slotHeater.input = Temperature.SlotTemperature;

  analogWrite(MOS_PIN, slotHeater.getResult());
  // analogWrite(MOS_PIN, pwm);

  if (Temperature.SlotTemperature > (Temperature.TargetTemperature - 1) && Temperature.SlotTemperature < (Temperature.TargetTemperature + 1))
  {
    Temperature.isCompleted = true;
  }
  else
  {
    Temperature.isCompleted = false;
  }
}

void setupTimer()
{
  Timer1.setFrequency(1); // установка частоты в Герцах и запуск таймера
  Timer1.enableISR();
  Timer1.stop();

  // Timer1.resume();
}

void setupHeter()
{
  slotHeater.setDirection(NORMAL); // Heat
  slotHeater.setLimits(0, 255);    // PWM constrain
  sensors.begin();
  slotHeater.setpoint = 0;
  // направление, начальный сигнал, конечный, период плато, точность, время стабилизации, период итерации
}
