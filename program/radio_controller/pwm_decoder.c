#include "pwm_decoder.h"
#include "bound.h"
extern input_capture_t inc[6];
/*
*/
uint8_t get_pwm_decode_value(radio_controller_t* rc)
{
	uint8_t gotUpdatedFlag=0;

	if (radio_controller_get_current_rc_input_source() == RC_INPUT_SOURCE_PWM_INC ){

		rc->roll_control_input = RC_ROLL_CONTROL_STEP_SIZE  *
				(float)(RC_CHANNEL_1_INPUT_CAPTURE - RC_CHANNEL_1_NEUTRAL);

		rc->pitch_control_input = - RC_PITCH_CONTROL_STEP_SIZE  *
				(float)(RC_CHANNEL_2_INPUT_CAPTURE - RC_CHANNEL_2_NEUTRAL);

		rc->throttle_control_input = RC_THROTTLE_CONTROL_STEP_SIZE *
				(float)(RC_CHANNEL_3_INPUT_CAPTURE - RC_CHANNEL_3_NEUTRAL);

		rc->yaw_rate_control_input = RC_YAW_RATE_CONTROL_STEP_SIZE  *
				(float)(RC_CHANNEL_4_INPUT_CAPTURE - RC_CHANNEL_4_NEUTRAL);

		if ( RC_CHANNEL_5_INPUT_CAPTURE > RC_CHANNEL_5_HALF_SIZE + RC_CHANNEL_5_NEUTRAL) {

			rc->safety = ENGINE_ON;
		} else {

			rc->safety = ENGINE_OFF;
		}

		if ( RC_CHANNEL_6_INPUT_CAPTURE > (RC_CHANNEL_6_HALF_SIZE + RC_CHANNEL_6_NEUTRAL) ) {

			rc->mode = MODE_3;
		} else if( RC_CHANNEL_6_INPUT_CAPTURE < (- RC_CHANNEL_6_HALF_SIZE + RC_CHANNEL_6_NEUTRAL) ){

			rc->mode = MODE_1;
		} else{

			rc->mode = MODE_2;

		}
		gotUpdatedFlag = 1;

	}else if(radio_controller_get_current_rc_input_source() == RC_INPUT_SOURCE_SBUS ){

		if(SBUS_receiver_update_when_available()){


				rc->roll_control_input = SBUS_ROLL_CONTROL_STEP_SIZE   *
						(float)(SBUS_reveiver_get_channel_data(1) - SBUS_CH1_NEUTRAL);

				rc->pitch_control_input = SBUS_PITCH_CONTROL_STEP_SIZE   *
						(float)(SBUS_reveiver_get_channel_data(2) - SBUS_CH2_NEUTRAL);

				rc->throttle_control_input = SBUS_THROTTLE_CONTROL_STEP_SIZE *
						(float)(SBUS_reveiver_get_channel_data(3) - SBUS_CH3_NEUTRAL);

				rc->yaw_rate_control_input = SBUS_YAW_RATE_CONTROL_STEP_SIZE  *
						(float)(SBUS_reveiver_get_channel_data(4) - SBUS_CH4_NEUTRAL);

				if ( SBUS_reveiver_get_channel_data(5) > SBUS_CH5_HALF_SIZE + SBUS_CH5_NEUTRAL) {

					rc->safety = ENGINE_ON;
				} else {

					rc->safety = ENGINE_OFF;
				}


				if ( SBUS_reveiver_get_channel_data(6) > (SBUS_CH6_HALF_SIZE + SBUS_CH6_NEUTRAL) ) {

					rc->mode = MODE_3;
				} else if( SBUS_reveiver_get_channel_data(6)  < (- SBUS_CH6_HALF_SIZE + SBUS_CH6_NEUTRAL) ){

					rc->mode = MODE_1;
				} else{

					rc->mode = MODE_2;

				}

				rc->rcv_status = SBUS_get_rcv_condition();
				rc->fs_status = SBUS_get_failsafe_status();

				gotUpdatedFlag=1;

			}
		}


	bound_float(rc->roll_control_input, ROLL_CONTROL_MIN, ROLL_CONTROL_MAX);
	bound_float(rc->pitch_control_input, PITCH_CONTROL_MIN, PITCH_CONTROL_MAX);
	bound_float(rc->throttle_control_input, THROTTLE_CONTROL_MIN, THROTTLE_CONTROL_MAX);
	bound_float(rc->yaw_rate_control_input, YAW_RATE_CONTROL_MIN, YAW_RATE_CONTROL_MAX);


	return gotUpdatedFlag;
}