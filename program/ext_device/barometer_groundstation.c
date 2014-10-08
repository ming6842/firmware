// Barometer ground station receiver
#include "barometer_groundstation.h"

void baro_groundstation_set_USART_IT(void){


	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = USART1_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 1,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);
}


void USART1_IRQHandler(void)
{

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
	while(1)
	LED_ON(LED1);
	}

}