#ifndef __SDCARD_H
#define __SDCARD_H
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "gpio.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "fatfs_sd.h"
#include "ff.h"
#include "integer.h"

#include "imu.h"
#include "flight_controller.h"


xSemaphoreHandle SD_sem;

#define buffer_1 1
#define buffer_2 2

void SD_data_Task(void *pvParameters);
void SD_write_Task(void *pvParameters);

// extern uint8_t buffer1[10000];
// extern uint8_t buffer2[10000];   





#endif