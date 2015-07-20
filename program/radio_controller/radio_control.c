#include "radio_control.h"
#include "pwm_decoder.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "led.h"
//#define DEBUG_RADIO_CONTROLLER
static radio_controller_t radio_controller = {
	.roll_control_input = 0.0f,
	.pitch_control_input = 0.0f,
	.throttle_control_input = 0.0f,
	.yaw_rate_control_input = 0.0f,
	.safety = ENGINE_ON,
	.mode = MODE_1,
	.rcv_status = FRAMELOST,
	.fs_status = FAILSAFE_NOT_ACTIVE 
	 
};
uint8_t update_radio_control_input(radio_controller_t *rc_data)
{
	uint8_t gotUpdatedFlag = get_pwm_decode_value(&radio_controller);
	memcpy(rc_data, &radio_controller, sizeof(radio_controller_t));

#ifdef DEBUG_RADIO_CONTROLLER
	printf("%d,%d,%d,%d,",
		(int16_t) (rc_data->roll_control_input*100),	
		(int16_t) (rc_data->pitch_control_input*100),
		(int16_t) (rc_data->throttle_control_input),
		(int16_t) (rc_data->yaw_rate_control_input*100));
	if (rc_data->safety == ENGINE_ON) {

			printf("ENGINE_ON,");
	} else {

			printf("ENGINE_OFF");

	}
	printf("\r\n");
	Delay_1us(100);
#endif	

	return gotUpdatedFlag;
}

static uint8_t rc_input_source = RC_INPUT_SOURCE_DEFAULT;


uint8_t radio_controller_get_current_rc_input_source(void){

	return rc_input_source;

}



void check_rc_safety_init(radio_controller_t *rc_controller_data)
{

	uint8_t safe_flag=0;
	uint32_t count_to_byebye = 0;
	uint32_t safe_count = 0;

	/* initialize SBUS if selected */

	if(radio_controller_get_current_rc_input_source() == RC_INPUT_SOURCE_SBUS){

		enable_sbus_usart6();

	}

	while(safe_flag==0){

		if(radio_controller_get_current_rc_input_source() == RC_INPUT_SOURCE_PWM_INC){
			count_to_byebye = 10000;
			safe_count = 0;

			while(count_to_byebye--){
				update_radio_control_input(rc_controller_data);
				if(((rc_controller_data->throttle_control_input)<5.0f)&&((rc_controller_data -> safety) == ENGINE_OFF)){

					safe_count++;

				}else{

				}

			}
			LED_TOGGLE(LED1);

			if(safe_count >= (10000-500)){
				safe_flag = 1;
				LED_OFF(LED1);
			}

		}else if(radio_controller_get_current_rc_input_source() == RC_INPUT_SOURCE_SBUS){


			if(update_radio_control_input(rc_controller_data)){

				if(((rc_controller_data->throttle_control_input)<5.0f)&&((rc_controller_data -> safety) == ENGINE_OFF)&&(rc_controller_data->rcv_status == FRAME_RECEIVED) && (rc_controller_data-> fs_status == FAILSAFE_NOT_ACTIVE)){


					safe_flag = 1;
					LED_OFF(LED1);
				}

			}

			if(!safe_flag){


				count_to_byebye = 100000;
				while(count_to_byebye--);
					LED_TOGGLE(LED1);
			}

		}



	}


}








