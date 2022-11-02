
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

ISR(TIMER1_A)
{

  if (Temperature.isHeaterOn)
    digitalWrite(HEATER_RELAY_PIN, HIGH);
  else
    digitalWrite(HEATER_RELAY_PIN, LOW);
  tuner.setInput(sensors.getTempCByIndex(0));
  tuner.compute();
  analogWrite(MOS_PIN, tuner.getOutput());

  // выводит в порт текстовые отладочные данные, включая коэффициенты
  tuner.debugText();
}

void setupTimer()
{
  Timer1.setFrequency(20); // установка частоты в Герцах и запуск таймера
  Timer1.enableISR();
  Timer1.stop();
  // Timer1.resume();
}

void setupHeter()
{
  //  slotHeater.setDirection(NORMAL); // Heat
  ///  slotHeater.setLimits(0, 255); // PWM constrain
  //  slotHeater.setpoint = 0;
  // направление, начальный сигнал, конечный, период плато, точность, время стабилизации, период итерации
  tuner.setParameters(NORMAL, 30, 80, 6000, 0.5, 500);
}
