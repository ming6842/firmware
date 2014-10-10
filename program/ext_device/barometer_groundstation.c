// Barometer ground station receiver
#include "barometer_groundstation.h"

void baro_groundstation_set_USART_IT(void){


	USART_ITConfig(UART8, USART_IT_TXE, DISABLE);
	USART_ITConfig(UART8, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = UART8_IRQn,
		.NVIC_IRQChannelPreemptionPriority =  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);
}


extern xQueueHandle baro_ref_serial_queue ;
void UART8_IRQHandler(void)
{

	long lHigherPriorityTaskWoken = pdFALSE;

	serial_msg rx_msg;


	if (USART_GetITStatus(UART8, USART_IT_RXNE) != RESET) {
		rx_msg.ch = USART_ReceiveData(UART8);

		if (!xQueueSendToBackFromISR(baro_ref_serial_queue, &rx_msg, &lHigherPriorityTaskWoken))
			portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
		LED_TOGGLE(LED1);
	}

	portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);


}

char uart8_read(void)
{
	serial_msg msg;

	while (!xQueueReceive(baro_ref_serial_queue, &msg, portMAX_DELAY));

	return msg.ch;
}

void baro_reference_receive_task(void){

 	/* Generate  vTaskDelayUntil parameters */
	portTickType xLastWakeTime;
	const portTickType xFrequency = (uint32_t)100/(1000.0 / configTICK_RATE_HZ);

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

	while(1){



		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		LED_TOGGLE(LED2);

	}

}