
#include "setup.h"
#include <GyverTimers.h>
#include <GyverPID.h>
#include "PIDtuner2.h"
#include "GyverNTC.h"
#include "config.h"
#include "thermistor.h"



GyverNTC therm(A3, 100000, 3950);
PIDtuner2 tuner;


ISR(TIMER1_A){
  tuner.setInput(therm.getTempAverage());
  tuner.compute();
  analogWrite(MOS_PIN, tuner.getOutput());

  // выводит в порт текстовые отладочные данные, включая коэффициенты
  tuner.debugText();
}

void setupTimer(){
  Timer1.setFrequency(20); // установка частоты в Герцах и запуск таймера
  Timer1.enableISR();
  Timer1.stop();
  //Timer1.resume();
}

void setupHeter(){
  //  slotHeater.setDirection(NORMAL); // Heat
  ///  slotHeater.setLimits(0, 255); // PWM constrain
  //  slotHeater.setpoint = 0;

    // направление, начальный сигнал, конечный, период плато, точность, время стабилизации, период итерации
  tuner.setParameters(NORMAL, 0, 80, 6000, 0.05, 500);
}
