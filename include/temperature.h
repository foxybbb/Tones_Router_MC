#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

struct
{
  double SlotTemperature;
  double TargetTemperature;

  bool isHeaterOn = false;
  bool isCompleted = false;
} Temperature;

#endif // __TEMPERATURE_H__