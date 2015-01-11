#include "sdcard.h"
#include "stm32f4xx_conf.h"

#include "gpio.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "fatfs_sd.h"
#include "ff.h"
#include "integer.h"

#include <string.h>


/* File system object structure (FATFS) */
FATFS     fs;
/* File object structure (FIL) */
FIL       fsrc;
/* File function return code (FRESULT) */
FRESULT   res;
/* File read/write count*/
UINT      bw;

uint8_t buffer1[5000];
uint8_t buffer2[5000]; 

uint8_t buffer_flag = buffer_2;

uint16_t i,j=0;
uint8_t words[20];

void SD_data_Task(void *pvParameters)
{

	while(1)
	{
		LED_TOGGLE(LED1);
		if (buffer_flag == buffer_2){
			buffer1[i]='a';
			i++;
		}else if(buffer_flag == buffer_1){
			buffer2[j]='b';
			j++;
		}

		sprintf((char*)&words,"%d %d\r\n",i,j);
  		uart8_puts(words);

		if(i>=5000){	
			buffer_flag = buffer_1;
			xSemaphoreGive(SD_sem);
			// vTaskDelay(40);
		}else if(j>=5000){
			buffer_flag = buffer_2;
			xSemaphoreGive(SD_sem);
			// vTaskDelay(40);
		} 	
      	
	}
}


void SD_write_Task(void *pvParameters)
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
		tskIDLE_PRIORITY + 6,
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
      	i=0;	
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
      	j=0;
		}

    	}	
	}
}
