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
#include <string.h>

extern uint8_t estimator_trigger_flag;

/* FreeRTOS */
extern xSemaphoreHandle serial_tx_wait_sem;
extern xQueueHandle serial_rx_queue;
extern xQueueHandle gps_serial_queue;
xTimerHandle xTimers[1];

xSemaphoreHandle SD_sem;

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

/* File system object structure (FATFS) */
FATFS     fs;
/* File object structure (FIL) */
FIL       fsrc;
/* File function return code (FRESULT) */
FRESULT   res;
/* File read/write count*/
UINT      bw;

uint8_t buffer1[5000]="1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
1FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n";                                                         

uint8_t buffer2[5000]="FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n\
FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n";   

uint8_t buffer_flag=1;
static void SD_data_Task(void *pvParameters)
{
	while(1)
	{
		LED_TOGGLE(LED1);
		if(buffer_flag==1)
		{
			buffer_flag=2;
		}else if(buffer_flag==2)
		{
			buffer_flag=1;
		}
      	xSemaphoreGive(SD_sem);
      	vTaskDelay(4000);
	}
}


static void SD_write_Task(void *pvParameters)
{

	uint8_t words[20];
	memset(words,' ',sizeof(words));
	uint8_t  retry = 0xFF;
  	do{
    	res = f_mount(&fs,"0:",1);
  	}while(res && --retry);
  	sprintf((char*)&words,"%d\r\n",res);
  	uart8_puts(words);
  	retry = 0xFF;
  	do{
    	res = f_open(&fsrc, "data.txt", FA_CREATE_ALWAYS);
  	}while(res && --retry);
  	sprintf((char*)&words,"%d\r\n",res);
  	uart8_puts(words);
  	retry = 0xFF;
  	do{
    	res = f_open(&fsrc, "data.txt", FA_WRITE );
  	}while(res && --retry);
  	sprintf((char*)&words,"%d\r\n",res);
  	uart8_puts(words);

	xTaskCreate(
		(pdTASK_CODE)SD_data_Task,
		(signed portCHAR*)"SD_data_Task",
		4096,
		NULL,
		tskIDLE_PRIORITY + 7,
		NULL
	);

	while(1){
		if(xSemaphoreTake(SD_sem,0)){

		if(buffer_flag==1){
			GPIO_ToggleBits(GPIOC,GPIO_Pin_9);
			LED_TOGGLE(LED4);
			do{
          		res = f_write(&fsrc,&buffer1,strlen((char*)&buffer1),&bw);
          		if(res){
            	LED_TOGGLE(LED2);
            	break;
          		}
      		}
      	while (bw < strlen((char*)&buffer1));
      	f_sync(&fsrc);
		}else if(buffer_flag==2){
			LED_TOGGLE(LED3);
			GPIO_ToggleBits(GPIOC,GPIO_Pin_8); 
			do{
          		res = f_write(&fsrc,&buffer2,strlen((char*)&buffer2),&bw);
          		if(res){
            	LED_TOGGLE(LED2);
            	break;
          		}
      		}
      	while (bw < strlen((char*)&buffer2));
      	f_sync(&fsrc);
		}

    	}	
	}
}


CanRxMsg MainRxMessage;
int main(void)
{
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(5, sizeof(serial_msg));
	gps_serial_queue = xQueueCreate(5, sizeof(serial_msg));
	vSemaphoreCreateBinary(flight_control_sem);
	vSemaphoreCreateBinary(SD_sem);
	/* Global data initialazition */
	init_global_data();

	 // Hardware initialization 
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

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);


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

