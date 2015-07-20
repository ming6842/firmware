#include "sbus.h"


void enable_sbus_usart6(void)
{
	/* RCC Initialization */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	/* GPIO Initialization */
	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = GPIO_Pin_7 ,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_Speed = GPIO_Speed_50MHz
	};

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USART3 Initialization */
	USART_InitTypeDef USART_InitStruct = {
		.USART_BaudRate = 100000,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_2,
		.USART_Parity = USART_Parity_Even,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx
	};

	USART_Init(USART6, &USART_InitStruct);
	USART_Cmd(USART6, ENABLE);


	USART_ClearFlag(USART6, USART_FLAG_TC);

	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	/* NVIC Initialization */
	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = USART6_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 0,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);
}

static uint8_t sbusFrameIndex=0;
static uint8_t sBUScapturingFlag=0;
static uint8_t receivedFrameFlag=0;
static uint8_t sbusBuffer[] = {0x0F, 0x00, 0x0C, 0x20, 0xA8, 0x01, 0x08, 0x16, 0x50, 0x83, 0x1A, 0x2C, 0xA0, 0x06, 0x35, 0xA8, 0xC1, 0x02, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x14, 0x00};
static uint8_t sbusReceivedBuffer[30];

static int16_t sbus_rc_data[16];
static uint8_t sbus_fs_status;
static uint8_t sbus_rcv_condition;
void USART6_IRQHandler(void)
{
	char c;
	uint8_t i;

	if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) {
		c = USART_ReceiveData(USART6);

		if(c == 0x0F){

			// LED_TOGGLE(LED3);
			sBUScapturingFlag=1;

			sbusBuffer[sbusFrameIndex] = c;
			sbusFrameIndex++;
		}else if((sBUScapturingFlag ==1) && (sbusFrameIndex<24)){


			sbusBuffer[sbusFrameIndex] = c;
			sbusFrameIndex++;

		}

		else if((sBUScapturingFlag ==1) && (sbusFrameIndex==24) && ((c == 0x04) || (c==0x14) || (c==0x24) || (c==0x34))){

				sbusBuffer[sbusFrameIndex] = c;

				for(i=0;i<25;i++){

					sbusReceivedBuffer[i] = sbusBuffer[i];

				}

				receivedFrameFlag=1;
				sbusFrameIndex=0;
				sBUScapturingFlag=0;
		}else{


			sbusFrameIndex=0;
			sBUScapturingFlag=0;
		}

	}


}

uint8_t SBUS_receiver_update_when_available(void){

	if(receivedFrameFlag == 1){


		sbus_rc_data[1] = ((int16_t)sbusReceivedBuffer[1] & 0xFFFF) | ((int16_t) (sbusReceivedBuffer[2] & 0x0007) << 8);
		sbus_rc_data[2] = ((int16_t)sbusReceivedBuffer[2] & 0x00F8 ) >> 3 | ((int16_t) (sbusReceivedBuffer[3] & 0x003F) << 5);
		sbus_rc_data[3] = ((int16_t)sbusReceivedBuffer[3] & 0x00C0 ) >> 6 | ((int16_t) (sbusReceivedBuffer[4]) << 2 ) | ((int16_t) (sbusReceivedBuffer[5] & 0x0001) << 10);
		sbus_rc_data[4] = ((int16_t)sbusReceivedBuffer[5] & 0x00FE ) >> 1 | ((int16_t) (sbusReceivedBuffer[6] & 0x000F) << 7);
		sbus_rc_data[5] = ((int16_t)sbusReceivedBuffer[6] & 0x00F0 ) >> 4 | ((int16_t) (sbusReceivedBuffer[7] & 0x007F) << 4);
		sbus_rc_data[6] = ((int16_t)sbusReceivedBuffer[7] & 0x0080 ) >> 7 | ((int16_t) (sbusReceivedBuffer[8]) << 1 ) | ((int16_t) (sbusReceivedBuffer[9] & 0x0003) << 9);
		sbus_rc_data[7] = ((int16_t)sbusReceivedBuffer[9] & 0x00FC ) >> 2 | ((int16_t) (sbusReceivedBuffer[10] & 0x007F) << 6);
		sbus_rc_data[8] = ((int16_t)sbusReceivedBuffer[10] & 0x00E0 ) >> 3 | ((int16_t) (sbusReceivedBuffer[11] ) << 3);
		
		sbus_rc_data[9] = ((int16_t)sbusReceivedBuffer[12] & 0xFFFF) | ((int16_t) (sbusReceivedBuffer[13] & 0x0007) << 8);
		sbus_rc_data[10] = ((int16_t)sbusReceivedBuffer[13] & 0x00F8 ) >> 3 | ((int16_t) (sbusReceivedBuffer[14] & 0x003F) << 5);
		sbus_rc_data[11] = ((int16_t)sbusReceivedBuffer[14] & 0x00C0 ) >> 6 | ((int16_t) (sbusReceivedBuffer[15]) << 2 ) | ((int16_t) (sbusReceivedBuffer[16] & 0x0001) << 10);
		sbus_rc_data[12] = ((int16_t)sbusReceivedBuffer[16] & 0x00FE ) >> 1 | ((int16_t) (sbusReceivedBuffer[17] & 0x000F) << 7);
		sbus_rc_data[13] = ((int16_t)sbusReceivedBuffer[17] & 0x00F0 ) >> 4 | ((int16_t) (sbusReceivedBuffer[18] & 0x007F) << 4);
		sbus_rc_data[14] = ((int16_t)sbusReceivedBuffer[18] & 0x0080 ) >> 7 | ((int16_t) (sbusReceivedBuffer[19]) << 1 ) | ((int16_t) (sbusReceivedBuffer[20] & 0x0003) << 9);
		sbus_rc_data[15] = ((int16_t)sbusReceivedBuffer[20] & 0x00FC ) >> 2 | ((int16_t) (sbusReceivedBuffer[21] & 0x007F) << 6);
		sbus_rc_data[16] = ((int16_t)sbusReceivedBuffer[21] & 0x00E0 ) >> 3 | ((int16_t) (sbusReceivedBuffer[22] ) << 3);
		
		// LED_TOGGLE(LED2);
		receivedFrameFlag=0;

		return 1;
	}

	return 0;
}

int16_t SBUS_reveiver_get_channel_data(uint8_t channel){

	return sbus_rc_data[channel];
}

uint8_t SBUS_get_rcv_condition(void){
//ch23
	uint8_t tmp = (sbusReceivedBuffer[23] & (0x04) )>>2;
	if(tmp == 0){
		sbus_rcv_condition = SBUS_LINK_OK;

	}else{

		sbus_rcv_condition= SBUS_FRAMELOST;

	}
	return sbus_rcv_condition;
}


uint8_t SBUS_get_failsafe_status(void){
//ch23
	uint8_t tmp = (sbusReceivedBuffer[23] & (0x08) )>>3;
	if(tmp == 0){

		sbus_fs_status = SBUS_FAILESAFE_NOT_ACTIVE;

	}else{

		sbus_fs_status = SBUS_FAILESAFE_ACTIVATED;

	}
	return sbus_fs_status;
}