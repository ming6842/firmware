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


uint8_t buffer_flag = buffer_2;

void SD_data_Task(void *pvParameters)
{
	while(1)
	{
		LED_TOGGLE(LED1);
		if(buffer_flag == buffer_1)
		{
			buffer_flag = buffer_2;
		}else if(buffer_flag == buffer_2)
		{
			buffer_flag = buffer_1;
		}
      	xSemaphoreGive(SD_sem);
      	vTaskDelay(4000);
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
