#ifndef BAROMETER_GROUNDSTATION_H
#define BAROMETER_GROUNDSTATION_H
#include "stm32f4xx_conf.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "led.h" 
#include <string.h>
void baro_groundstation_set_USART_IT(void);
void UART8_IRQHandler(void);
char uart8_read(void);
void baro_reference_receive_task(void);
void clear_rcv_stat(void);
void receive_error_handler(void);
#endif