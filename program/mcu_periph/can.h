
#ifndef FILE_CAN_H
#define FILE_CAN_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx_can.h"
#include "led.h"

void can1_init(void);

uint8_t can_loopback_test(void);

#endif
