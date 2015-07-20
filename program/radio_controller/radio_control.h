#ifndef FILE_RADIO_CONTROL_H
#define FILE_RADIO_CONTROL_H
#include "stm32f4xx_conf.h"

typedef enum {
	ENGINE_ON = 0,
	ENGINE_OFF
}SAFETY_CHANNEL;

typedef enum {
	MODE_1= 0,
	MODE_2,
	MODE_3
}MODE_CHANNEL;

typedef enum {
	FAILSAFE_NOT_ACTIVE = 0,
	FAILESAFE_ACTIVATED,
}FS_STATUS;

typedef enum {
	FRAME_RECEIVED= 0,
	FRAMELOST,
}RCV_STATUS;

typedef struct radio_controller{

	volatile float roll_control_input;
	volatile float pitch_control_input;
	volatile float throttle_control_input;
	volatile float yaw_rate_control_input;
	volatile SAFETY_CHANNEL safety;
	volatile MODE_CHANNEL mode;
	volatile RCV_STATUS rcv_status;
	volatile FS_STATUS fs_status;

} radio_controller_t;

uint8_t update_radio_control_input(radio_controller_t *);
void check_rc_safety_init(radio_controller_t * );
uint8_t radio_controller_get_current_rc_input_source(void);

#endif