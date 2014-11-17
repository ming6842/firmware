/* CAN bus driver */
#include "can.h"

void CAN1_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;


  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* CAN GPIOs configuration **************************************************/

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

  /* Connect CAN pins to AF9 */
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 
  
  /* Configure CAN RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  /* CAN configuration ********************************************************/  
  /* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
  
  /* CAN register init */
  CAN_DeInit(CAN1);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    
  /* CAN Baudrate = 1 MBps (CAN clocked at 30 MHz) */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 20;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
/* USE_CAN1 */
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}


void can1_init(void){

	// CAN_InitTypeDef        CAN_InitStructure;
 //  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
 

	//   /* CANx Periph clock enable */
 //  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

 //  /* CAN register init */
 //  CAN_DeInit(CAN1);
  
 //  /* CAN cell init */
 //  CAN_InitStructure.CAN_TTCM = DISABLE;
 //  CAN_InitStructure.CAN_ABOM = DISABLE;
 //  CAN_InitStructure.CAN_AWUM = DISABLE;
 //  CAN_InitStructure.CAN_NART = DISABLE;
 //  CAN_InitStructure.CAN_RFLM = DISABLE;
 //  CAN_InitStructure.CAN_TXFP = DISABLE;
 //  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
 //  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

 //  /* CAN Baudrate = 175kbps (CAN clocked at 42 MHz) */
 //  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
 //  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
 //  CAN_InitStructure.CAN_Prescaler = 16;
 //  CAN_Init(CAN1, &CAN_InitStructure);


 //  /* CAN filter init */
 //  CAN_FilterInitStructure.CAN_FilterNumber = 0;
 //  /* USE_CAN1 */
 //  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
 //  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
 //  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
 //  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
 //  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
 //  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;  
 //  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

 //  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
 //  CAN_FilterInit(&CAN_FilterInitStructure);
}

#define PASSED 1
#define FAILED 0

uint8_t can_loopback_test(void){

  CanTxMsg TxMessage;
  CanRxMsg RxMessage;
  uint32_t i = 0;
  uint8_t TransmitMailbox = 0;

  /* transmit */
  TxMessage.StdId = 0x11;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 2;
  TxMessage.Data[0] = 0xCA;
  TxMessage.Data[1] = 0xFE;

  TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CAN1, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFFFF))
  {
    i++;
  }
  LED_ON(LED1);

  i = 0;
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i  !=  0xFFFF))
  {
    i++;
  }

  LED_ON(LED2);


  /* receive */
  RxMessage.StdId = 0x00;
  RxMessage.IDE = CAN_ID_STD;
  RxMessage.DLC = 0;
  RxMessage.Data[0] = 0x00;
  RxMessage.Data[1] = 0x00;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  if (RxMessage.StdId != 0x11)
  {
    return FAILED;  
  }

  if (RxMessage.IDE != CAN_ID_STD)
  {
    return FAILED;
  }

  if (RxMessage.DLC != 2)
  {
    return FAILED;  
  }

  if ((RxMessage.Data[0]<<8|RxMessage.Data[1]) != 0xCAFE)
  {
    return FAILED;
  }
  
  return PASSED; /* Test Passed */


}

void CAN1_Transmit(void){

	CanTxMsg TxMessage;

  /* Transmit Structure preparation */
  TxMessage.StdId = 0x321;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 1;
  TxMessage.Data[0] = 64;
  CAN_Transmit(CANx, &TxMessage);

}

void CAN1_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
 /* USE_CAN1 */

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
CanRxMsg RxMessage;

void CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
  
  if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
  {
  		LED_TOGGLE(LED3);
  }
}