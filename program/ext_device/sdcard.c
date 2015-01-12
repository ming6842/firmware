#include "sdcard.h"

/* File system obbuffer2_counterect structure (FATFS) */
FATFS     fs;
/* File obbuffer2_counterect structure (FIL) */
FIL       fsrc;
/* File function return code (FRESULT) */
FRESULT   res;
/* File read/write count*/
UINT      bw;

uint8_t buffer1[20000];
uint8_t buffer2[20000]; 
uint8_t buffer_flag = 3;
uint8_t buffer_sync_flag = 0 ;
uint32_t buffer1_counter,buffer2_counter=0;
uint8_t file_name[20]="data.txt";

uint8_t words[20];
uint8_t counter_add;
uint8_t time_stamp=0;

void SD_data_Task(void *pvParameters)
{
	buffer_flag = buffer_2;
	while(1)
	{
		if( xSemaphoreTake(SD_data_trigger, 9) == pdTRUE){
			uint8_t i;	
			time_stamp++;
			if (time_stamp>=10) time_stamp=0;
			if (buffer_flag == buffer_2){
				memset(words,' ',sizeof(words));
				sprintf(words,"%ld,%ld,%ld,%d\r\n",(int32_t)(imu_raw_data.acc[0]*100.0f),(int32_t)(imu_raw_data.acc[1]*100.0f),(int32_t)(imu_raw_data.acc[2]*100.0f),(signed int)time_stamp);
				for(i=0;i<(strlen((char*)words));i++){
					buffer1[buffer1_counter+i]=words[i];
				}
				buffer1_counter += (strlen((char*)words));
			}else if(buffer_flag == buffer_1){
				memset(words,' ',sizeof(words));
				sprintf(words,"%ld,%ld,%ld,%d\r\n",(int32_t)(imu_raw_data.acc[0]*100.0f),(int32_t)(imu_raw_data.acc[1]*100.0f),(int32_t)(imu_raw_data.acc[2]*100.0f),(signed int)time_stamp);
				for(i=0;i<(strlen((char*)words));i++){
					buffer2[buffer2_counter+i]=words[i];
				}
				buffer2_counter += (strlen((char*)words));
			}	

			if(buffer1_counter>=19950){	
				buffer_flag = buffer_1;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
				}else vTaskDelay(400);		
			}else if(buffer2_counter>=19950){
				buffer_flag = buffer_2;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
				}else vTaskDelay(400);
			}
		} 
	}    		
}


void SD_write_Task(void *pvParameters)
{
	memset(buffer1,' ',sizeof(buffer1));
	memset(buffer2,' ',sizeof(buffer2));
	uint8_t i=0;

	uint8_t  retry = 0xFF;
  	do{
    	res = f_mount(&fs,"0:",1);
  	}while(res && --retry);
  	printf("%d\r\n",res);

  	for(i=1;i<27;i++){
  		retry = 0xFF;
  		do{
    		res = f_open(&fsrc,(TCHAR *)file_name,FA_CREATE_NEW);
  		}while(res && --retry);
  		if(res) file_name[3] = 97 + i;
  		else break;
  	}
  	printf("%d\r\n",res);
  
  	
  	retry = 0xFF;
  	do{
    	res = f_open(&fsrc,(TCHAR *)file_name, FA_WRITE );
  	}while(res && --retry);
  	printf("%d\r\n",res);

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
			if(buffer_flag == buffer_1){
				LED_TOGGLE(LED4);	
				buffer1_counter=0;
				buffer_sync_flag = 1;	
				f_lseek(&fsrc,fsrc.fsize);
				do{
          			res = f_write(&fsrc,&buffer1,strlen((char*)buffer1),&bw);
          			if(res){
            		LED_TOGGLE(LED2);
            		break;
          			}
      			}	
      		while (bw < strlen((char*)buffer1));
      		f_sync(&fsrc);
      		buffer_sync_flag = 0;
      		memset(buffer1,' ',sizeof(buffer1));	
			}else if(buffer_flag == buffer_2){		
				LED_TOGGLE(LED3);
				buffer2_counter=0;
				buffer_sync_flag = 1;
				f_lseek(&fsrc,fsrc.fsize); 
				do{
          			res = f_write(&fsrc,&buffer2,strlen((char*)buffer2),&bw);
          			if(res){
            		LED_TOGGLE(LED2);
            		break;
          			}
      			}
      		while (bw < strlen((char*)buffer2));
      		f_sync(&fsrc);
      		buffer_sync_flag = 0;	
      		memset(buffer2,' ',sizeof(buffer2));    		
			}
    	}	
	}
}
