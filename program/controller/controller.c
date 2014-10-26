#include "controller.h"

int32_t altitude_setpoint_accumulator=0;

#define RC_ALTITUDE_STICK_SENSITIVITY  2.0f
#define Z_SETPOINT_DISTANCE_LIMIT 100.0f //CM unit

void PID_rc_pass_command(attitude_t* attitude,vertical_data_t* vertical_filtered_data,attitude_stablizer_pid_t* PID_roll,attitude_stablizer_pid_t* PID_pitch,attitude_stablizer_pid_t* PID_heading,vertical_pid_t* PID_Z,vertical_pid_t* PID_Zd,nav_pid_t* PID_nav,radio_controller_t* rc_command){

	int32_t altitude_accomulator_precal = 0;

	PID_roll -> setpoint = (rc_command -> roll_control_input) + (PID_nav -> output_roll);
	PID_pitch -> setpoint = (rc_command -> pitch_control_input) + (PID_nav -> output_pitch);

	/* pre-calculate current accumulator */
	altitude_accomulator_precal = (int32_t)(gap_float_middle(rc_command -> throttle_control_input-50.0f, -7.0f, 7.0f) * RC_ALTITUDE_STICK_SENSITIVITY);

	/* check if setpoint is too far already */

	if(fabsf( PID_Z -> setpoint - vertical_filtered_data -> Z ) > Z_SETPOINT_DISTANCE_LIMIT){

		if(altitude_accomulator_precal>0){ // command to going higher case

			if(( PID_Z -> setpoint -  vertical_filtered_data -> Z ) > 0){ // check if setpoint is already too high

				/* need failsafe mechanism here */

			}else{ // setpoint is not too high, may be it's too low

				/* Accumulating is ok */
				altitude_setpoint_accumulator +=  altitude_accomulator_precal;

			}

		}else{  // commanding to go lower (altitude_accomulator_precal<0)

			if(( PID_Z -> setpoint -  vertical_filtered_data -> Z ) < 0){  // check if setpoint is already too low

				/* need failsafe mechanism here */
				/*FIXME */

			}else{ // setpoint it not too low

				/* Substracting accomulation is ok */
				altitude_setpoint_accumulator +=  altitude_accomulator_precal;

			}
		}



	}else{  // No problem , accumulate it up


	altitude_setpoint_accumulator +=  altitude_accomulator_precal;

	}

	PID_Z -> setpoint = (float)altitude_setpoint_accumulator/4000.0f;


	if( rc_command -> safety == ENGINE_ON) {


		/* PID_Heading engage */
		PID_heading -> setpoint = (PID_heading -> setpoint)+ (rc_command -> yaw_rate_control_input)*CONTROL_DT;

		if((PID_heading -> setpoint ) > 360.0f){

			PID_heading -> setpoint = PID_heading -> setpoint - 360.0f;
		}else if((PID_heading -> setpoint ) <0.0f){

			PID_heading -> setpoint = PID_heading -> setpoint + 360.0f;

		}


		if((rc_command -> mode) == MODE_3){

			PID_Z -> controller_status = CONTROLLER_ENABLE ;
			PID_Zd -> controller_status = CONTROLLER_ENABLE ;
			PID_nav -> controller_status = CONTROLLER_ENABLE;

		}else if((rc_command -> mode) == MODE_2){

			PID_Z -> controller_status = CONTROLLER_ENABLE ;
			PID_Zd -> controller_status = CONTROLLER_ENABLE ;
			PID_nav -> controller_status = CONTROLLER_DISABLE;

		}else{ // MODE_1

			/* altitude controller is now enabled at all time */
			PID_Z -> controller_status = CONTROLLER_ENABLE ;
			PID_Zd -> controller_status = CONTROLLER_ENABLE ;
			// PID_Z -> controller_status = CONTROLLER_DISABLE ;
			// PID_Zd -> controller_status = CONTROLLER_DISABLE ;
			PID_nav -> controller_status = CONTROLLER_DISABLE;

		}


	}else{

		/* PID_Heading Suppressed */
		PID_heading -> setpoint = attitude -> yaw;
		/* PID_Zd Integral Suppressed */
	}



}


void PID_init(attitude_stablizer_pid_t* PID_roll,attitude_stablizer_pid_t* PID_pitch,attitude_stablizer_pid_t* PID_yaw_rate,attitude_stablizer_pid_t* PID_heading,vertical_pid_t* PID_Z,vertical_pid_t* PID_Zd,nav_pid_t* PID_nav){


	PID_roll -> kp =0.20f;
	PID_roll -> kd =0.07f;
	PID_roll -> ki =0.0;
	PID_roll -> setpoint =0.0;

	PID_pitch -> kp =0.20f;
	PID_pitch -> kd =0.07f;
	PID_pitch -> ki =0.0;
	PID_pitch -> setpoint =0.0;

	PID_yaw_rate -> kp =0.65f;
	PID_yaw_rate -> kd =0.0f;
	PID_yaw_rate -> ki =0.0;
	PID_yaw_rate -> setpoint =0.0;
	PID_yaw_rate -> out_max = 30.0f;
	PID_yaw_rate -> out_min = -30.0f;

	PID_heading -> kp = 2.5f;
	PID_heading -> kd = 0.0f;
	PID_heading -> ki = 0.0;
	PID_heading -> out_max = 50.0f;
	PID_heading -> out_min = -50.0f;
	PID_heading -> setpoint = 0.0;

	PID_Zd -> kp =0.3f;
	PID_Zd -> kd =0.0;
	PID_Zd -> ki =0.05;
	PID_Zd -> out_max = +20.0f;
	PID_Zd -> out_min = -20.0f;
	PID_Zd -> setpoint =0.0;

	PID_Z -> kp =1.8f;//1.8f;
	PID_Z -> kd =0.0;
	PID_Z -> ki =0.0;
	PID_Z -> out_max = +50.0f;
	PID_Z -> out_min = -50.0f;
	PID_Z -> setpoint =0.0;


	PID_nav -> kp =0.045f;//0.045f;
	PID_nav -> kd =0.06f;//0.06;
	PID_nav -> ki =0.0001f;
	PID_nav -> out_max = +25.0f;
	PID_nav -> out_min = -25.0f;

}

#define IDLE_THROTTLE 25.0f 

void PID_output(radio_controller_t* rc_command,attitude_stablizer_pid_t* PID_roll,attitude_stablizer_pid_t* PID_pitch,attitude_stablizer_pid_t* PID_yaw_rate,vertical_pid_t* PID_Zd){

motor_output_t motor;

	motor. m1 =0.0;
	motor. m2 =0.0;
	motor. m3 =0.0;
	motor. m4 =0.0;
	motor. m5 =0.0;
	motor. m6 =0.0;
	motor. m7 =0.0;
	motor. m8 =0.0;
	motor. m9 =0.0;
	motor. m10 =0.0;
	motor. m11 =0.0;
	motor. m12 =0.0;
	if( rc_command -> safety == ENGINE_ON) {

	motor . m1 = IDLE_THROTTLE - (PID_roll->output) + (PID_pitch -> output) - (PID_yaw_rate -> output) + (PID_Zd -> output);
	motor . m2 = IDLE_THROTTLE + (PID_roll->output) + (PID_pitch -> output) + (PID_yaw_rate -> output) + (PID_Zd -> output);
	motor . m3 = IDLE_THROTTLE + (PID_roll->output) - (PID_pitch -> output) - (PID_yaw_rate -> output) + (PID_Zd -> output);
	motor . m4 = IDLE_THROTTLE - (PID_roll->output) - (PID_pitch -> output) + (PID_yaw_rate -> output) + (PID_Zd -> output);
	set_pwm_motor(&motor);

	LED_ON(LED3);

	}else{

	motor. m1 =0.0;
	motor. m2 =0.0;
	motor. m3 =0.0;
	motor. m4 =0.0;
	set_pwm_motor(&motor);

	LED_OFF(LED3);
	}
}
