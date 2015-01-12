#include "sdcard.h"

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
uint8_t file_name[20]="data.txt";

uint8_t words[20];
uint8_t counter_add;
uint32_t time_stamp=0;

void SD_data_Task(void *pvParameters)
{
	buffer_flag = buffer_2;
	while(1)
	{
		if( xSemaphoreTake(flight_control_sem, 9) == pdTRUE){
			time_stamp++;
			if (buffer_flag == buffer_2){
				// memset(words,' ',sizeof(words));
				// sprintf(words,"%ld,%ld,%ld,%ld\r\n",(int32_t)(imu_raw_data.acc[0]*100.0f),(int32_t)(imu_raw_data.acc[1]*100.0f),(int32_t)(imu_raw_data.acc[2]*100.0f),time_stamp);
				// printf("%s",words);
				// printf("%d\r\n",strlen(words));
				// printf("%d\r\n",buffer1_counter);
				// strcat(buffer1,words);
				// buffer1_counter += strlen(words);
				buffer1[buffer1_counter]='1';
				buffer1[buffer1_counter+1]='2';
				buffer1[buffer1_counter+2]='3';
				buffer1[buffer1_counter+3]='4';
				buffer1[buffer1_counter+4]='5';
				buffer1[buffer1_counter+5]='6';
				buffer1[buffer1_counter+6]='7';
				buffer1[buffer1_counter+7]='8';
				buffer1[buffer1_counter+8]='9';
				buffer1[buffer1_counter+9]='0';
				buffer1[buffer1_counter+10]='0';
				buffer1[buffer1_counter+11]='0';
				buffer1[buffer1_counter+12]='\r';
				buffer1[buffer1_counter+13]='\n';
				buffer1_counter+=14;
			}else if(buffer_flag == buffer_1){
				// memset(words,' ',sizeof(words));
				// sprintf(words,"%ld,%ld,%ld,%ld\r\n",(int32_t)(imu_raw_data.acc[0]*100.0f),(int32_t)(imu_raw_data.acc[1]*100.0f),(int32_t)(imu_raw_data.acc[2]*100.0f),time_stamp);
				// strcat(buffer2,words);
				// buffer2_counter += strlen(words);
				buffer2[buffer2_counter]='a';
				buffer2[buffer2_counter+1]='b';
				buffer2[buffer2_counter+2]='c';
				buffer2[buffer2_counter+3]='d';
				buffer2[buffer2_counter+4]='e';
				buffer2[buffer2_counter+5]='f';
				buffer2[buffer2_counter+6]='g';
				buffer2[buffer2_counter+7]='h'; 
				buffer2[buffer2_counter+8]='i';
				buffer2[buffer2_counter+9]='j';
				buffer2[buffer2_counter+10]='k';
				buffer2[buffer2_counter+11]='l';
				buffer2[buffer2_counter+12]='\r';
				buffer2[buffer2_counter+13]='\n';
				buffer2_counter+=14;
			}	

			if(buffer1_counter>=19950){	
				buffer_flag = buffer_1;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
					// GPIO_ToggleBits(GPIOC,GPIO_Pin_7);
				}else vTaskDelay(400);		
			}else if(buffer2_counter>=19950){
				buffer_flag = buffer_2;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
					// GPIO_ToggleBits(GPIOC,GPIO_Pin_7);
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
	uint32_t length=0;

	uint8_t  retry = 0xFF;
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
  	printf("%d\r\n",res);
  
  	
  	retry = 0xFF;
  	do{
    	res = f_open(&fsrc,&file_name, FA_WRITE );
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
				// GPIO_ToggleBits(GPIOC,GPIO_Pin_9);
				LED_TOGGLE(LED4);
				length = buffer1_counter;
				printf("%d\r\n",length);	
				buffer1_counter=0;
				buffer_sync_flag = 1;	
				f_lseek(&fsrc,fsrc.fsize);
				do{
          			res = f_write(&fsrc,&buffer1,length,&bw);
          			if(res){
            		LED_TOGGLE(LED2);
            		break;
          			}
      			}	
      		while (bw < length);
      		f_sync(&fsrc);
      		buffer_sync_flag = 0;
      		memset(buffer1,' ',sizeof(buffer1));	
			}else if(buffer_flag == buffer_2){		
				LED_TOGGLE(LED3);
				// GPIO_ToggleBits(GPIOC,GPIO_Pin_8);
				length = buffer2_counter;
				buffer2_counter=0;
				buffer_sync_flag = 1;
				f_lseek(&fsrc,fsrc.fsize); 
				do{
          			res = f_write(&fsrc,&buffer2,length,&bw);
          			if(res){
            		LED_TOGGLE(LED2);
            		break;
          			}
      			}
      		while (bw < length);
      		f_sync(&fsrc);
      		buffer_sync_flag = 0;	
      		memset(buffer2,' ',sizeof(buffer2));    		
			}
    	}	
	}
}
