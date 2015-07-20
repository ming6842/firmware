
#ifndef FILE_SBUS_H
#define FILE_SBUS_H
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "led.h"
#include "usart.h"
#include <stdio.h>

typedef enum {
	SBUS_FAILESAFE_NOT_ACTIVE=  0,
	SBUS_FAILESAFE_ACTIVATED,
}SBUS_FS_STATUS;

typedef enum {
	SBUS_LINK_OK= 0,
	SBUS_FRAMELOST,
}SBUS_RCV_CONDITION;
void enable_sbus_usart6(void);
void USART6_IRQHandler(void);


uint8_t SBUS_receiver_update_when_available(void);
int16_t SBUS_reveiver_get_channel_data(uint8_t channel);
uint8_t SBUS_get_rcv_condition(void);
uint8_t SBUS_get_failsafe_status(void);

#endif
