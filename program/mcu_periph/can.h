
#ifndef FILE_CAN_H
#define FILE_CAN_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx_can.h"
#include "led.h"

#define CANx                       CAN1
#define CAN_CLK                    RCC_APB1Periph_CAN1
#define CAN_RX_PIN                 GPIO_Pin_0
#define CAN_TX_PIN                 GPIO_Pin_1
#define CAN_GPIO_PORT              GPIOD
#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define CAN_AF_PORT                GPIO_AF_CAN1
#define CAN_RX_SOURCE              GPIO_PinSource0
#define CAN_TX_SOURCE              GPIO_PinSource1     

void can1_init(void);
uint8_t can_loopback_test(void);

void CAN1_Config(void);
void CAN1_Transmit(void);
void CAN1_NVIC_Config(void);
void CAN1_RX0_IRQHandler(void);

void CAN2_Config(void);
void CAN2_Transmit(void);
void CAN2_NVIC_Config(void);
void CAN2_RX0_IRQHandler(void);

#endif
