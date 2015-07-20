
/* Define board selection */
/* Currently support 

	configBOARD_VERTIGO_V20 : Vertigo Control V2.0
	configBOARD_VERTIGO_V21 : Vertigo Control V2.1
	*/
#define configBOARD_VERTIGO_V21


#ifdef configBOARD_VERTIGO_V20
	//Vertigo v2.0 board setting
	#define USE_FUTABA 
	#define USE_IMU_MPU9250
	#define USE_MAG_HMC5983
	#define USE_CAN_MAGNETOMETER
	#define USE_ADS1246_MPX6115A
	//#define USE_CAN_ADS1246_MPX6115A 
	#define STM32F427X
	/*choose the external device*/
	#define IS_USE_MAGNETOMETER 1
	#define IS_USE_BAROMETER 1
	#define IS_USE_GPS 1
	/*Set magnetometer*/
	#if IS_USE_MAGNETOMETER

	#endif
	/*Set barometer*/
	#if IS_USE_BAROMETER

	#endif

	/*Set GPS*/
	#if IS_USE_GPS

	#endif

	// Controller define

	#define CONTROL_f (4000.0f)
	#define CONTROL_DT 0.00025f
	#define USE_MAGNETIC_HEADING
	//#define USE_GYROSCOPE_HEADING
#endif

#ifdef configBOARD_VERTIGO_V21
	//Vertigo v2.0 board setting
	#define USE_FUTABA 
	#define USE_IMU_MPU9250
	#define USE_MAG_HMC5983
	#define USE_CAN_MAGNETOMETER
	#define USE_ADS1246_MPX6115A
	//#define USE_CAN_ADS1246_MPX6115A 
	#define STM32F427X
	/*choose the external device*/
	#define IS_USE_MAGNETOMETER 1
	#define IS_USE_BAROMETER 1
	#define IS_USE_GPS 1
	/*Set magnetometer*/
	#if IS_USE_MAGNETOMETER

	#endif
	/*Set barometer*/
	#if IS_USE_BAROMETER

	#endif

	/*Set GPS*/
	#if IS_USE_GPS

	#endif

	// Controller define

	#define CONTROL_f (4000.0f)
	#define CONTROL_DT 0.00025f
	#define USE_MAGNETIC_HEADING
	//#define USE_GYROSCOPE_HEADING
#endif
