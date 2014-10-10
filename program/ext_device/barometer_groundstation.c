// Barometer ground station receiver
#include "barometer_groundstation.h"


#define BARO_REF_DEBUG printf

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
	}

	portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);


}

char uart8_read(void)
{
	serial_msg msg;

	while (!xQueueReceive(baro_ref_serial_queue, &msg, portMAX_DELAY));

	return msg.ch;
}

#define BARO_REF_BUFFER_SIZE 30

static uint16_t dataIndex = 0;
static uint8_t capturingFlag = 0;
static uint8_t bufferFullFlag = 0;
static uint8_t bufferGetStop = 0;
static uint8_t gotStartBitFlag=0;
static uint8_t msgLength = 5;
static uint8_t baroRefBuffer[BARO_REF_BUFFER_SIZE];
static uint8_t chksum_OK_flag=0;

static uint32_t rcv_err_count=0;

float baro_ref_output_cm=0.0f;
float V_Z_reference=0.0f;
int32_t rcv_raw_data=0.0f;

void receive_error_handler(void){

			bufferFullFlag=0;
			capturingFlag=0;
			dataIndex = 0;
			gotStartBitFlag = 0;
			rcv_err_count++;

}

void clear_rcv_stat(void){

			bufferFullFlag=0;
			capturingFlag=0;
			dataIndex = 0;
			gotStartBitFlag = 0;

}

uint8_t buffer[100];

void baro_reference_receive_task(void){

	uint8_t c;
	uint8_t i=0;
	uint8_t chksum_trans=0;
	uint8_t chksum_calc=0;
	uint8_t first_data_flag=1;

	while(1){

		c = uart8_read();
		if((c == 0x73) && (capturingFlag==0)){

			capturingFlag=1;
			dataIndex = 0;
		}else if((capturingFlag == 1 )&& (baroRefBuffer[dataIndex] == 0x73) && (c == 0x33)){

			dataIndex = 1;
			gotStartBitFlag = 1;

		}else if((dataIndex >0)&&(gotStartBitFlag)&&(capturingFlag)){  

			if(dataIndex == 1){

				if(c == 0x05){ // get the correct message type

					dataIndex = 2;

				}else{

					receive_error_handler(); // discard data;
				}

			}else if((dataIndex >1)&&(dataIndex<msgLength+1)){

				dataIndex++;
			}else { // End of data length
				dataIndex++;
				baroRefBuffer[dataIndex] = c;
				bufferGetStop = 1;
 
				/* calculate checksum */
				chksum_calc = 0;
				for (i=0;i<dataIndex;i++){
					chksum_calc+= baroRefBuffer[i];
				}

				chksum_trans = baroRefBuffer[dataIndex];
				if(chksum_calc == chksum_trans){

				chksum_OK_flag = 1;


				memcpy(&rcv_raw_data,&baroRefBuffer[3],sizeof(int32_t));
				baro_ref_output_cm = (float)rcv_raw_data*0.0001f;

				if((float)(fabs(V_Z_reference - baro_ref_output_cm))<500.0f){
				V_Z_reference= lowpass_float(&V_Z_reference,&baro_ref_output_cm,0.05f);
				}else if(first_data_flag){

					first_data_flag=0;
					V_Z_reference = baro_ref_output_cm;

				}
				/* Debug session */



				if (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {

					buffer[5] = 0;buffer[6] = 0;buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;

					/* for doppler PID test */
					// sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld\r\n",
					// 	(int32_t)(pid_nav_info.output_roll* 1.0f),
					// 	(int32_t)(pid_nav_info.output_pitch* 1.0f),
					// 	(int32_t)GPS_velocity_NED.velN,
					// 	(int32_t)GPS_velocity_NED.velE,
			 	// 		(uint32_t)GPS_solution_info.numSV);
				

					sprintf((char *)buffer, "%ld,%ld,%ld\n",
						
			 			(int32_t)(baro_ref_output_cm*10000.0f),
			 			(int32_t)(V_Z_reference*10000.0f),
			 			(uint32_t)(rcv_err_count));

					usart2_dma_send(buffer);
				}	





				/* end of debug session */





				clear_rcv_stat();
				LED_TOGGLE(LED1);

				//BARO_REF_DEBUG("%d \r\n ",(int32_t)(baro_ref_output_cm*10000.0f));


				}else{

					receive_error_handler();
					/* discard data and increase error count */

				}
			}


		}


			// Too big data//
			if(dataIndex == BARO_REF_BUFFER_SIZE-1){ 
			bufferFullFlag=0;
			capturingFlag=0;
			dataIndex = 0;
			receive_error_handler();
			}

			// capture data
			baroRefBuffer[dataIndex] = c;


		//BARO_REF_DEBUG("%c",c);

		//vTaskDelayUntil( &xLastWakeTime, xFrequency );
		LED_TOGGLE(LED2);

	}

}