#include <stdint.h>
#include "adc.h"

#ifndef SENSOR_H_
	#define SENSOR_H_

/*============================================================
		各種定数･変数宣言
============================================================*/

	typedef struct{
		uint32_t val;			//Raw Value
		int16_t base;			//
		int16_t dif;			// = (Raw -base)
		int16_t pre;			//For D-control
		int16_t diff;			//D-Control
		float out;				//output for Duty
		uint16_t threshold;		//For Wall Check
	}wall_sensor;

	typedef struct{
		float velocity;			//Unit is [m/s]
		float vel_target;
		float accel;
		float velocity_max;
		float velocity_min;
		int8_t velocity_dir;

		float omega_rad;		//Unit is [rad/s]
		float omega_deg;		//Unit is [deg/s]
		float omega_target;		//Unit is [rad/s]
		float omega_accel;
		float omega_max;
		float pre_omega_deg;	//Unit is [rad/s]
		int8_t omega_dir;

		float distance;			//Unit is [mm]
		float angle;			//Unit is [deg]
		float pre_angle;
		float angle_target;

	} gravity;


	typedef struct{
		int16_t pulse;			//Raw Value
		int16_t  dif;
		int64_t sum; 			//Raw Integral
		float distance;			//Converted Value [mm]
		float velocity;			//unit m/s
	} encoder;

	#ifdef MAIN_C_
		wall_sensor wall_r;
		wall_sensor wall_fr;
		wall_sensor wall_ff;
		wall_sensor wall_fl;
		wall_sensor wall_l;

		/*** Structure [Gravity-Center] [encoder] ***/
		encoder encoder_r;
		encoder encoder_l;
		gravity center;

		volatile uint8_t tp;
		volatile float gyro_omega_base,gyro_accel_base;


	#else

		extern wall_sensor wall_r;
		extern wall_sensor wall_fr;
		extern wall_sensor wall_ff;
		extern wall_sensor wall_fl;
		extern wall_sensor wall_l;

		extern encoder encoder_r;
		extern encoder encoder_l;
		extern gravity center;

		extern volatile uint8_t tp;
		extern volatile float gyro_omega_base,gyro_accel_base;

	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====センサ系====
	void GetWallData(void);
	void EncoderGyroTest(void);
	void StartTimer(void);
	void StopTimer(void);
	void CheckSensor(void);
	void GetControlBaseValue(void);

	void Tim6WaitUs(uint16_t);
	int16_t GetEncoderLeft(void);
	int16_t GetEncoderRight(void);
	void UpdateEncoder(void);
	void UpdateGyro(void);

	int GetADC(ADC_HandleTypeDef *hadc, uint32_t channel);
	void WallStructureInit(wall_sensor*);
	void CenterStructureInit(gravity*);
	void EncoderStructureInit(encoder*);


#endif /* SENSOR_H_ */
