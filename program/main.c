//#define DEBUG
#include "stm32f4xx_conf.h"
#include "../common/delay.h"
#include "gpio.h"
#include "led.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "can.h"
#include "tim.h"
#include "flight_controller.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "global.h"
#include "communication.h"
#include "system_time.h"
#include "lea6h_ubx.h"
#include "simple_navigation.h"

#include "fatfs_sd.h"
#include "ff.h"
#include "integer.h"

#include "sdcard.h"

extern uint8_t estimator_trigger_flag;

/* FreeRTOS */
extern xSemaphoreHandle serial_tx_wait_sem;
extern xQueueHandle serial_rx_queue;
extern xQueueHandle gps_serial_queue;
xTimerHandle xTimers[1];

void vApplicationStackOverflowHook( xTaskHandle xTask, signed char *pcTaskName );
void vApplicationIdleHook(void);
void vApplicationMallocFailedHook(void);
void boot_time_timer(void);
void gpio_rcc_init(void);
void gpio_rcc_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
	RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE,  ENABLE);	
}

void vApplicationStackOverflowHook( xTaskHandle Task __attribute__ ((unused)), signed char *pcTaskName __attribute__ ((unused)))
{
	while(1);

}
void vApplicationIdleHook(void)
{

}
void vApplicationMallocFailedHook(void)
{
	while(1);
}

CanRxMsg MainRxMessage;
int main(void)
{

	/* File system obbuffer2_counterect structure (FATFS) */
	FATFS     fs;
	/* File obbuffer2_counterect structure (FIL) */
	FIL       fsrc;
	/* File function return code (FRESULT) */
	FRESULT   res;
	/* File read/write count*/
	DWORD      bw;


	uint8_t buffer1[20000];
	uint8_t buffer2[20000]; 
	uint8_t buffer_flag = 3;
	uint8_t buffer_sync_flag = 0 ;
	uint32_t buffer1_counter,buffer2_counter=0;
	uint8_t file_name[20]="HAHA.txt";

	uint8_t words[20];
	uint8_t counter_add;
	uint32_t time_stamp=0;

	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(5, sizeof(serial_msg));
	gps_serial_queue = xQueueCreate(5, sizeof(serial_msg));
	vSemaphoreCreateBinary(flight_control_sem);
	// vSemaphoreCreateBinary(SD_data_trigger);
	vSemaphoreCreateBinary(SD_sem);
	/* Global data initialazition */
	init_global_data();

	/* Hardware initialization */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	gpio_rcc_init();
	led_init();
	usart_init();
	spi_init();
	pwm_input_output_init();
	init_pwm_motor();
	i2c_Init();
	usart2_dma_init();

	cycle_led(1);
	//can1_init();
	CAN1_NVIC_Config();
	CAN1_Config();

	CAN1_Transmit();



	memset(buffer1,' ',sizeof(buffer1));
	memset(buffer2,' ',sizeof(buffer2));
	uint8_t i=0;
	uint32_t length=0;
	uint8_t  retry = 0xFF;

	uint32_t multiplied_count = 5000;

	length = sprintf(buffer1, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

  	do{
    	res = f_mount(&fs,"0:",1);
  	}while(res && --retry);
  	printf("%d\r\n",res);


  	for(i=1;i<27;i++){
  		retry = 0xFF;
  		do{
    		res = f_open(&fsrc,&file_name,FA_CREATE_NEW);
  		}while(res && --retry);
  		if(res) file_name[3] = 97 + i;
  		else break;
  	}

  	retry = 0xFF;
  	do{
    	res = f_open(&fsrc,&file_name, FA_WRITE );
  	}while(res && --retry);


	f_lseek(&fsrc,fsrc.fsize);

	while(multiplied_count--){
		do{
         			LED_TOGGLE(TOGGLE_DEBUG);
        		res = f_write(&fsrc,&buffer1,length,&bw);
         			//LED_ON(TOGGLE_DEBUG);
        		if(res){
         		// LED_TOGGLE(LED2);
         		break;
        		}
     		}	
     	while (bw < length);
         			// LED_OFF(TOGGLE_DEBUG);
         			// LED_ON(TOGGLE_DEBUG);
         			// LED_TOGGLE(TOGGLE_DEBUG);
     }
    f_sync(&fsrc);
    //2.46
    while(1){


    	LED_TOGGLE(LED1);
    }

	// GPIO_InitTypeDef GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	// GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	// GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	// GPIO_Init(GPIOC, &GPIO_InitStruct);


	/* Register the FreeRTOS task */
	/* Flight control task */
	xTaskCreate(
		(pdTASK_CODE)SD_write_Task,
		(signed portCHAR*)"SD write Task",
		4096,
		NULL,
		tskIDLE_PRIORITY + 6,
		NULL
	);

	// xTaskCreate(
	// 	(pdTASK_CODE)flight_control_task,
	// 	(signed portCHAR*)"flight control task",
	// 	4096,
	// 	NULL,
	// 	tskIDLE_PRIORITY + 9,
	// 	NULL
	// );

	// /* Navigation task */
	// xTaskCreate(
	// 	(pdTASK_CODE)navigation_task,
	// 	(signed portCHAR*)"navigation task",
	// 	512,
	// 	NULL,
	// 	tskIDLE_PRIORITY + 7,
	// 	NULL
	// );

	// /* Ground station communication task */	
	// xTaskCreate(
	// 	(pdTASK_CODE)ground_station_task,
	// 	(signed portCHAR *)"ground station send task",
	// 	2048,
	// 	NULL,
	// 	tskIDLE_PRIORITY + 5,
	// 	NULL
	// );

	// xTaskCreate(
	// 	(pdTASK_CODE)mavlink_receiver_task,
	// 	(signed portCHAR *) "ground station receive task",
	// 	4096,
	// 	NULL,
	// 	tskIDLE_PRIORITY + 7, NULL
	// );

	// xTaskCreate(
	// 	(pdTASK_CODE)gps_receive_task,
	// 	(signed portCHAR *) "gps receive task",
	// 	2048,
	// 	NULL,
	// 	tskIDLE_PRIORITY + 8, NULL

	// );
	vTaskStartScheduler();

	return 0;
}

