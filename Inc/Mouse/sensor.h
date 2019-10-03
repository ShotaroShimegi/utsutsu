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
		float out;

		uint16_t threshold;
	}wall_sensor;


	//====変数====
	#ifdef MAIN_C_
		/*グローバル変数の定義*/
		//----LEDポート----
		volatile wall_sensor wall_r;
		volatile wall_sensor wall_fr;
		volatile wall_sensor wall_ff;
		volatile wall_sensor wall_fl;
		volatile wall_sensor wall_l;

		//----その他----
		volatile uint8_t tp;
		volatile float gyro_base;

	#else

		//----壁センサ構造体----
		extern volatile wall_sensor wall_r;
		extern volatile wall_sensor wall_fr;
		extern volatile wall_sensor wall_ff;
		extern volatile wall_sensor wall_fl;
		extern volatile wall_sensor wall_l;

		//----その他----
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


#endif /* SENSOR_H_ */
