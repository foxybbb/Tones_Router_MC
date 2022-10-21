#ifndef __CONFIG_H__
#define __CONFIG_H__

    #define DEBUG_MODE 0
#define PORT_BOUDRATE 115200

#define MAX_X_POSITION 250
#define MAX_Y_POSITION 100
#define MAX_Z_POSITION 200

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

// Macros
#define stepsToMM_X(x) ((x)*10)
#define stepsToMM_Y(x) ((x)*15)
#define stepsToMM_Z(x) ((x)*400)

#define DEFAULT_ACELERATION 100
#define Z_DEFAULT_ACCELERATION 800
#define DEFAULT_MAX_SPEED 800
#define Z_DEFAULT_MAX_SPEED 3000

#define LIMSW_X_PIN 9
#define LIMSW_Y_PIN 10
#define LIMSW_Z_PIN 12

#endif // __CONFIG_H__