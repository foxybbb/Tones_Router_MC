#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

struct
{
  double SlotTemperature;
  double TargetTemperature;

  bool isHeaterOn = false;
  bool isCompleted = false;
  
  double maxTemperature = 0;
  double minTemperature = 0;
  int temperatureCount = 0;
} Temperature;

#endif // __TEMPERATURE_H__