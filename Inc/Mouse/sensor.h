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

	#ifdef MAIN_C_
		wall_sensor wall_r;
		wall_sensor wall_fr;
		wall_sensor wall_ff;
		wall_sensor wall_fl;
		wall_sensor wall_l;

		volatile uint8_t tp;
		volatile float gyro_base;

	#else

		extern wall_sensor wall_r;
		extern wall_sensor wall_fr;
		extern wall_sensor wall_ff;
		extern wall_sensor wall_fl;
		extern wall_sensor wall_l;

		extern volatile uint8_t tp;
		extern volatile float gyro_base;

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


#endif /* SENSOR_H_ */
