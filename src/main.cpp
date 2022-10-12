#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include "config.h"
#include "func.h"

SemaphoreHandle_t XMutex;




void setup() {
 Serial.begin(PORT_BOUDRATE);

 XMutex = xSemaphoreCreateMutex();

 xTaskCreate(TaskSerialParser,"Parser",128,NULL,2,NULL);
 xTaskCreate(TaskMoveMotors,"MotorMove",128,NULL,2,NULL);


}

void loop() {
  // put your main code here, to run repeatedly:
}

void TaskMoveMotors(void *pvParameters){
xSemaphoreTake(XMutex, portMAX_DELAY);

xSemaphoreGive(XMutex); // release mutex
}