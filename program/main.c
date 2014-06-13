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
void gpio_rcc_init(void);


void gpio_rcc_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
		RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE,  ENABLE);	
}


int main(void)
{
	uint8_t buffer[100];

	/* State estimator initialization */
	imu_unscaled_data_t imu_unscaled_data;
	imu_data_t imu_raw_data;
	imu_data_t imu_filtered_data;
	imu_calibrated_offset_t imu_offset;
	attitude_t attitude;
	vector3d_t predicted_g_data;
	euler_trigonometry_t negative_euler;
	vertical_data vertical_raw_data;
	vertical_data vertical_filtered_data;

	/* GPS localizer initialization */
	UBXvelned_t UBXvelned;
	UBXsol_t UBXsol;
	UBXposLLH_t UBXposLLH;

	/* Radio controller initialization */
	radio_controller_t my_rc;

	/* PID controller initialization */
	attitude_stablizer_pid_t pid_roll_info;
	attitude_stablizer_pid_t pid_pitch_info;
	attitude_stablizer_pid_t pid_yaw_rate_info;
	attitude_stablizer_pid_t pid_heading_info;
	vertical_pid_t pid_Zd_info;
	vertical_pid_t pid_Z_info;


	PID_init(&pid_roll_info,&pid_pitch_info ,&pid_yaw_rate_info ,&pid_heading_info,&pid_Z_info ,&pid_Zd_info);

	attitude_estimator_init(&attitude,&imu_raw_data, &imu_filtered_data,&predicted_g_data);
	vertical_estimator_init(&vertical_raw_data,&vertical_filtered_data);

	gpio_rcc_init();
	led_init();
	usart_init();
	spi_init();
	pwm_input_output_init();
	init_pwm_motor();
	i2c_Init();
	usart2_dma_init();

	cycle_led(5);
	magnetometer_initialize(&imu_offset);

	// lea6h_set_USART_IT();
	// while(1){
	// 	Delay_1us(80);

	// 	LED_TOGGLE(LED4);

	// 	lea6h_ubx_get_updated_data(&UBXvelned,&UBXsol,&UBXposLLH);

	// 	if(UBXsol.updatedFlag){




	// 	if (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {

	// 		buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;


	// 		// sprintf((char *)buffer, "%ld,%ld,%ld\r\n",
	// 		// 	(int32_t)(imu_raw_data.mag[0]),
	// 		// 	(int32_t)(imu_raw_data.mag[1]),
	// 		// 	(int32_t)(imu_raw_data.mag[2]));


	// 		sprintf((char *)buffer, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,\r\n",


	// 				(uint32_t)UBXvelned.itow,
	// 				(int32_t)UBXvelned.velN,
	// 				(int32_t)UBXvelned.velE,
	// 				(uint32_t)UBXsol.pAcc,
	// 				(uint32_t)UBXvelned.speedAccu,
	// 				(uint32_t)UBXsol.pDOP,
	// 				(uint32_t)UBXsol.numSV);
	// 		usart2_dma_send(buffer);

	// 	}	
	// 	UBXsol.updatedFlag=0;
	// }
	// }


	imu_initialize(&imu_offset,30000);

	check_rc_safety_init(&my_rc);
 	barometer_initialize();
	lea6h_set_USART_IT();


	while (1) {

		LED_OFF(LED4);
		if (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {

			buffer[7] = 0;buffer[8] = 0;buffer[9] = 0;buffer[10] = 0;buffer[11] = 0;buffer[12] = 0;	buffer[13] = 0;


			sprintf((char *)buffer, "%d,%d,%d,%d,%d,%d\r\n",
				(int16_t)(pid_heading_info.setpoint* 1.0f),
				(int16_t)(attitude.yaw *1.0f),
				(int16_t)(pid_heading_info.error* 1.0f),
				(int16_t)(pid_heading_info.output *1.0f),
				(int16_t)(vertical_filtered_data.Z * 1.0f),
				(int16_t)(vertical_filtered_data.Zd  * 1.0f));

			usart2_dma_send(buffer);

		}	

		/* yay */
		attitude_update(&attitude,&imu_filtered_data, &predicted_g_data,&imu_unscaled_data,&imu_raw_data,&imu_offset);
		inverse_rotation_trigonometry_precal(&attitude,&negative_euler);
		vertical_sense(&vertical_filtered_data,&vertical_raw_data, &imu_raw_data,&negative_euler);
		
		heading_sense(&attitude,&imu_raw_data,&negative_euler);

		lea6h_ubx_get_updated_data(&UBXvelned,&UBXsol,&UBXposLLH);

		PID_attitude_roll (&pid_roll_info,&imu_filtered_data,&attitude);
		PID_attitude_pitch(&pid_pitch_info,&imu_filtered_data,&attitude);

		PID_attitude_heading(&pid_heading_info,&attitude);
		PID_attitude_yaw_rate  (&pid_yaw_rate_info,&imu_filtered_data);

		PID_vertical_Z(&pid_Z_info,&vertical_filtered_data);
		/* bind Zd controller to Z */
		pid_Zd_info.setpoint = pid_Z_info.output;
		PID_vertical_Zd(&pid_Zd_info,&vertical_filtered_data);

		PID_output(&my_rc,&pid_roll_info,&pid_pitch_info,&pid_yaw_rate_info,&pid_Zd_info);



		update_radio_control_input(&my_rc);
		PID_rc_pass_command(&attitude,&pid_roll_info,&pid_pitch_info,&pid_heading_info,&pid_Z_info,&pid_Zd_info,&my_rc);

		LED_ON(LED4);

		while(estimator_trigger_flag==0);
		estimator_trigger_flag=0;


#ifdef DEBUG
		test_bound();
#endif
	}

	return 0;
}
