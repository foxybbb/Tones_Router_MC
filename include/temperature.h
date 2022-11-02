#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

struct {
  double SlotTemperature;
  double TargetTemperature;


  bool isHeaterOn = 0;

} Temperature;


#endif // __TEMPERATURE_H__