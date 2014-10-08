#ifndef BAROMETER_GROUNDSTATION_H
#define BAROMETER_GROUNDSTATION_H
#include "stm32f4xx_conf.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "led.h" 
void baro_groundstation_set_USART_IT(void);
void USART2_IRQHandler(void);

#endif