#ifndef FILE_PWM_DECODER_H
#define FILE_PWM_DECODER_H
#include "input_capture.h"
#include "rc_config.h"
#include "pwm.h"
#include "radio_control.h"
#include "sbus.h"

#define RC_ROLL_CONTROL_STEP_SIZE  ( (float)ROLL_CONTROL_MAX /(float)(RC_CHANNEL_1_MAX- RC_CHANNEL_1_NEUTRAL) )
#define RC_PITCH_CONTROL_STEP_SIZE  ( (float)PITCH_CONTROL_MAX / (float)(RC_CHANNEL_2_MAX- RC_CHANNEL_2_NEUTRAL) )
#define RC_THROTTLE_CONTROL_STEP_SIZE \
( ( (float)THROTTLE_CONTROL_MAX - (float)THROTTLE_CONTROL_MIN )/ (float) (RC_CHANNEL_3_MAX- RC_CHANNEL_3_NEUTRAL) )
#define RC_YAW_RATE_CONTROL_STEP_SIZE  ( (float)YAW_RATE_CONTROL_MAX / (float)(RC_CHANNEL_4_MAX- RC_CHANNEL_4_NEUTRAL) )

#define SBUS_ROLL_CONTROL_STEP_SIZE  ( (float)(ROLL_CONTROL_MAX - ROLL_CONTROL_MIN) /(float)(SBUS_CH1_MAX - SBUS_CH1_MIN) )
#define SBUS_PITCH_CONTROL_STEP_SIZE  ( (float)(PITCH_CONTROL_MAX - PITCH_CONTROL_MIN) / (float)(SBUS_CH2_MAX - SBUS_CH2_MIN) )
#define SBUS_THROTTLE_CONTROL_STEP_SIZE \
( ( (float)THROTTLE_CONTROL_MAX - (float)THROTTLE_CONTROL_MIN )/ (float) (SBUS_CH3_MAX - SBUS_CH3_MIN) )
#define SBUS_YAW_RATE_CONTROL_STEP_SIZE  ( (float)(YAW_RATE_CONTROL_MAX - YAW_RATE_CONTROL_MIN) / (float)(SBUS_CH4_MAX - SBUS_CH4_MIN) )



uint8_t get_pwm_decode_value(radio_controller_t* );
#endif 