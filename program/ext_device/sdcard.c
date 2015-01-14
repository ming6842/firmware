#include "sdcard.h"

/* File system obbuffer2_counterect structure (FATFS) */
FATFS     fs;
/* File obbuffer2_counterect structure (FIL) */
FIL       fsrc;
/* File function return code (FRESULT) */
FRESULT   res;
/* File read/write count*/
UINT      bw;

uint8_t buffer1[20480];
uint8_t buffer2[20480]; 
uint8_t buffer_flag = 3;
uint8_t buffer_sync_flag = 0 ;
uint32_t buffer1_counter,buffer2_counter=0;
uint8_t file_name[20]="data.txt";


uint8_t words[40];
uint8_t counter_add;
uint8_t time_stamp=0;
uint8_t time_flag=0;

void SD_data_Task(void *pvParameters)
{
	buffer_flag = buffer_2;
	while(1)
	{
		if( xSemaphoreTake(SD_data_trigger, 9) == pdTRUE && time_flag==4){
			time_flag=0;
			GPIO_ToggleBits(GPIOC,GPIO_Pin_8);
			uint8_t i;	
			if (buffer_flag == buffer_2){
				memset(words,0x00,sizeof(words));
				sprintf(words,"%ld,%ld,%ld,%d\r\n",imu_unscaled_data.acc[0],imu_unscaled_data.acc[1],imu_unscaled_data.acc[2],(signed int)time_stamp);
				for(i=0;i<(strlen((char*)words));i++){
					buffer1[buffer1_counter+i]=words[i];
				}
				buffer1_counter += (strlen((char*)words));
			}else if(buffer_flag == buffer_1){
				memset(words,0x00,sizeof(words));
				sprintf(words,"%ld,%ld,%ld,%d\r\n",imu_unscaled_data.acc[0],imu_unscaled_data.acc[1],imu_unscaled_data.acc[2],(signed int)time_stamp);
				for(i=0;i<(strlen((char*)words));i++){
					buffer2[buffer2_counter+i]=words[i];
				}
				buffer2_counter += (strlen((char*)words));
			}	

			GPIO_ToggleBits(GPIOC,GPIO_Pin_8);

			if(buffer1_counter>=19968){	
				buffer_flag = buffer_1;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
				}else vTaskDelay(40);		
			}else if(buffer2_counter>=19968){
				buffer_flag = buffer_2;
				if(buffer_sync_flag==0){
					xSemaphoreGive(SD_sem);
					LED_TOGGLE(LED1);
				}else vTaskDelay(40);
			}
		} 
	}    		
}


void SD_write_Task(void *pvParameters)
{
	memset(buffer1,0x00,sizeof(buffer1));
	memset(buffer2,0x00,sizeof(buffer2));

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
      		memset(buffer1,0x00,sizeof(buffer1));	
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
      		memset(buffer2,0x00,sizeof(buffer2));    		
			}
    	}	
	}
}


void SD_swritting_once_Task(void *pvParameters){

	uint8_t i=0;
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

  	while(1){
  		/*saftey trigger dection*/
  		if(my_rc.safety==ENGINE_OFF){
  			f_close(&fsrc);
  		}else{
  			if( xSemaphoreTake(SD_data_trigger, 9) == pdTRUE){
  			sprintf(words,"%ld,%ld,%ld\r\n",(int32_t)(imu_raw_data.acc[0]*100.0f),(int32_t)(imu_raw_data.acc[1]*100.0f),(int32_t)(imu_raw_data.acc[2]*100.0f));
  			f_write(&fsrc,&words,strlen((char*)words),&bw);
  			}
  		}
  	}
}