#include "parser.h"

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
