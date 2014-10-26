//#define DEBUG
#include "stm32f4xx_conf.h"
#include "../common/delay.h"
#include "gpio.h"
#include "led.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include <stdio.h>
#include "attitude_estimator.h"
#include "vertical_estimator.h"
#include "estimator.h"
#include "controller.h"
#include "pwm.h"
#include "radio_control.h"
#include "test_common.h"
#include "hmc5983.h"
#include "lea6h_ubx.h"

extern uint8_t estimator_trigger_flag;

#define OUTPUT_ALL 0
#define OUTPUT_MAG 1
#define OUTPUT_IMU 2
#define OUTPUT_BARO 3
#define OUTPUT_GPS 4
#define OUTPUT_RC 5
#define OUTPUT_YU_CHI 6

/* temporary use */
motor_output_t motor_for_yu_chi_data_out;

void gpio_rcc_init(void);


void gpio_rcc_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
		RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE,  ENABLE);	
}

int main(void)
{
	uint8_t buffer[200];
	uint32_t transmit_delay_count=0;
	uint8_t output_mode = OUTPUT_YU_CHI;
	/* State estimator initialization */
	imu_unscaled_data_t imu_unscaled_data;
	imu_data_t imu_raw_data;
	imu_data_t imu_filtered_data;
	imu_calibrated_offset_t imu_offset;
	attitude_t attitude;
	vector3d_f_t predicted_g_data;
	euler_trigonometry_t negative_euler;
	vertical_data vertical_raw_data;
	vertical_data vertical_filtered_data;

	/* GPS localizer initialization */
	UBXvelned_t GPS_velocity_NED;
	UBXsol_t GPS_solution_info;
	UBXposLLH_t GPS_position_LLH;

	/* Radio controller initialization */
	radio_controller_t my_rc;

	/* PID controller initialization */
	attitude_stablizer_pid_t pid_roll_info;
	attitude_stablizer_pid_t pid_pitch_info;
	attitude_stablizer_pid_t pid_yaw_rate_info;
	attitude_stablizer_pid_t pid_heading_info;
	vertical_pid_t pid_Zd_info;
	vertical_pid_t pid_Z_info;
	nav_pid_t pid_nav_info;


	PID_init(&pid_roll_info,&pid_pitch_info ,&pid_yaw_rate_info ,&pid_heading_info,&pid_Z_info ,&pid_Zd_info,&pid_nav_info);

	attitude_estimator_init(&attitude,&imu_raw_data, &imu_filtered_data,&predicted_g_data);
	vertical_estimator_init(&vertical_raw_data,&vertical_filtered_data);

	gpio_rcc_init();
	led_init();
	usart_init();
	spi_init();
	pwm_input_output_init();
	//calibrate_ESC();
	init_pwm_motor();
	i2c_Init();
	usart2_dma_init();

	cycle_led(5);
	magnetometer_initialize(&imu_offset);

	imu_initialize(&imu_offset,30000);

	check_rc_safety_init(&my_rc);
 	barometer_initialize();
	lea6h_set_USART_IT();

	while (1) {

		LED_OFF(LED4);
		

	// if(GPS_solution_info.updatedFlag){
	// 	if (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {

	// 		buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;

	// 		/* for doppler PID test */
	// 		// sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld\r\n",
	// 		// 	(int32_t)(pid_nav_info.output_roll* 1.0f),
	// 		// 	(int32_t)(pid_nav_info.output_pitch* 1.0f),
	// 		// 	(int32_t)GPS_velocity_NED.velN,
	// 		// 	(int32_t)GPS_velocity_NED.velE,
	//  	// 		(uint32_t)GPS_solution_info.numSV);
		

	// 		sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
	// 			(int32_t)(attitude.yaw * 1.0f),
	// 			(int32_t)(vertical_filtered_data.Z* 1.0f),
	// 			(int32_t)(vertical_filtered_data.Zd* 1.0f),
	// 			(int32_t)(pid_nav_info.output_roll* 1.0f),
	// 			(int32_t)(pid_nav_info.output_pitch* 1.0f),
	// 			(int32_t)GPS_velocity_NED.velE,

	//  			(uint32_t)GPS_solution_info.pAcc,
	//  			(uint32_t)GPS_solution_info.numSV);

	// 		usart2_dma_send(buffer);
	// 	}	
	//  	GPS_solution_info.updatedFlag=0;
	// }

		if(output_mode == OUTPUT_ALL){

			transmit_delay_count++;
			if ((DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) && (transmit_delay_count >= 100)) {

				uint8_t iii=0;

				for(iii=0;iii<190;iii++){

					buffer[iii]=0;

				};
				buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;

				/* for doppler PID test */
				// sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld\r\n",
				// 	(int32_t)(pid_nav_info.output_roll* 1.0f),
				// 	(int32_t)(pid_nav_info.output_pitch* 1.0f),
				// 	(int32_t)GPS_velocity_NED.velN,
				// 	(int32_t)GPS_velocity_NED.velE,
		 	// 		(uint32_t)GPS_solution_info.numSV);
			
				sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
					(int32_t)(attitude.yaw 						* 100.0f),
					(int32_t)(imu_filtered_data.acc[0] 			* 100.0f),
					(int32_t)(imu_filtered_data.acc[1] 			* 100.0f),
					(int32_t)(imu_filtered_data.acc[2] 			* 100.0f),
					(int32_t)(imu_filtered_data.gyro[0] 		* 100.0f),
					(int32_t)(imu_filtered_data.gyro[1] 		* 100.0f),
					(int32_t)(imu_filtered_data.gyro[2] 		* 100.0f),
					(int32_t)(vertical_filtered_data.Z			* 100.0f),
					(int32_t)(vertical_filtered_data.Zd 		* 100.0f),
					(int32_t)(my_rc.roll_control_input  		* 100.0f),
					(int32_t)(my_rc.pitch_control_input  		* 100.0f),
					(int32_t)(my_rc.throttle_control_input  	* 100.0f),
					(int32_t)(my_rc.yaw_rate_control_input  	* 100.0f),
					(int32_t)(motor_for_yu_chi_data_out.m1  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m2  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m3  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m4  	* 10.0f));

				usart2_dma_send(buffer);
				transmit_delay_count = 0;
				LED_TOGGLE(LED1);
			}	

		}else if(output_mode == OUTPUT_MAG){

			transmit_delay_count++;
			if ((DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) && (transmit_delay_count >= 100)) {

				uint8_t iii=0;

				for(iii=0;iii<190;iii++){

					buffer[iii]=0;

				};
				buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;


				sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,\r\n",
					(int32_t)(attitude.yaw 						* 1.0f),
					(int32_t)(imu_unscaled_data.mag[0] 			* 1),
					(int32_t)(imu_unscaled_data.mag[1] 			* 1),
					(int32_t)(imu_unscaled_data.mag[2] 			* 1),
					(int32_t)(imu_filtered_data.mag[0] 			* 100.0f),
					(int32_t)(imu_filtered_data.mag[1] 			* 100.0f),
					(int32_t)(imu_filtered_data.mag[2] 			* 100.0f));


				usart2_dma_send(buffer);
				transmit_delay_count = 0;
				LED_TOGGLE(LED1);
			}	

		}else if(output_mode == OUTPUT_RC){

			transmit_delay_count++;
			if ((DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) && (transmit_delay_count >= 100)) {

				uint8_t iii=0;

				for(iii=0;iii<190;iii++){

					buffer[iii]=0;

				};
				buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;


				sprintf((char *)buffer, "%ld,%ld,%ld,%ld,\r\n",
					(int32_t)(my_rc.roll_control_input  		* 100.0f),
					(int32_t)(my_rc.pitch_control_input  		* 100.0f),
					(int32_t)(my_rc.throttle_control_input  	* 100.0f),
					(int32_t)(my_rc.yaw_rate_control_input  	* 100.0f));


				usart2_dma_send(buffer);
				transmit_delay_count = 0;
				LED_TOGGLE(LED1);
			}	

		}else if(output_mode == OUTPUT_YU_CHI){

			transmit_delay_count++;
			if ((DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) && (transmit_delay_count >= 40)){

				uint8_t iii=0;

				for(iii=0;iii<190;iii++){

					buffer[iii]=0;

				};
				buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;


				sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,\r\n",

					(int32_t)(imu_filtered_data.gyro[0] 		* 100.0f),
					(int32_t)(imu_filtered_data.gyro[1] 		* 100.0f),
					(int32_t)(imu_filtered_data.gyro[2] 		* 100.0f),
					(int32_t)(motor_for_yu_chi_data_out.m1  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m2  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m3  	* 10.0f),
					(int32_t)(motor_for_yu_chi_data_out.m4  	* 10.0f));

				usart2_dma_send(buffer);
				transmit_delay_count = 0;
				LED_TOGGLE(LED1);
			}	

		}




		attitude_update(&attitude,&imu_filtered_data, &predicted_g_data,&imu_unscaled_data,&imu_raw_data,&imu_offset);
		inverse_rotation_trigonometry_precal(&attitude,&negative_euler);
		vertical_sense(&vertical_filtered_data,&vertical_raw_data, &imu_raw_data,&negative_euler);
		
		heading_sense(&attitude,&imu_raw_data,&negative_euler);

		lea6h_ubx_get_updated_data(&GPS_velocity_NED,&GPS_solution_info,&GPS_position_LLH);



		PID_attitude_heading(&pid_heading_info,&attitude);
		/* bind heading to rate */
		pid_yaw_rate_info.setpoint = pid_heading_info.output;
		PID_attitude_yaw_rate  (&pid_yaw_rate_info,&imu_filtered_data);

		PID_vertical_Z(&pid_Z_info,&vertical_filtered_data);
		/* bind Zd controller to Z */
		pid_Zd_info.setpoint = pid_Z_info.output;
		PID_vertical_Zd(&pid_Zd_info,&vertical_filtered_data);

		PID_Nav(&pid_nav_info,&attitude,&GPS_velocity_NED, &GPS_position_LLH);

		PID_attitude_roll (&pid_roll_info,&imu_filtered_data,&attitude);
		PID_attitude_pitch(&pid_pitch_info,&imu_filtered_data,&attitude);

		PID_output(&my_rc,&pid_roll_info,&pid_pitch_info,&pid_yaw_rate_info,&pid_Zd_info);

		update_radio_control_input(&my_rc);
		PID_rc_pass_command(&attitude,&pid_roll_info,&pid_pitch_info,&pid_heading_info,&pid_Z_info,&pid_Zd_info,&pid_nav_info,&my_rc);

		LED_ON(LED4);

		while(estimator_trigger_flag==0);
		estimator_trigger_flag=0;


#ifdef DEBUG
		test_bound();
#endif
	}

	return 0;
}

