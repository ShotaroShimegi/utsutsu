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
		int16_t dif;			//
		int16_t pre;			//For D-control
		int16_t diff;			//D-Control

		uint16_t threshold;
	}wall_sensor;

	typedef struct{
		int16_t epsilon;		//壁制御偏差
		int16_t pre_epsilon;		//D制御用のバッファ
		float sen_pid;
	}wall_control;

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
		volatile uint8_t tp;											//タスクポインタ

		volatile uint16_t base_l, base_r;								//基準値を格納
		volatile int16_t dif_l, dif_r;									//AD値と基準との差

		volatile float cont_r,cont_l;									//壁制御の比例定数

		volatile float dif_total;
		volatile float pre_dif_total;									//壁のPD制御用
		volatile float sen_ctrl;

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
		extern volatile uint16_t base_l, base_r;
		extern volatile int16_t dif_l, dif_r;

		extern volatile float cont_r,cont_l;

		extern volatile float pre_dif_total;
		extern volatile float dif_total;
		extern volatile float sen_ctrl;

		extern volatile float gyro_base;

	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====センサ系====
	unsigned char get_base();					//センサ基準値を取得
	void get_wall_info();				//壁情報を読む
	void EncoderGyroTest();
	void sensor_start();
	void sensor_stop();
	void sensor_check();

	void Tim6WaitUs(uint16_t);
	int16_t GetEncoderLeft(void);
	int16_t GetEncoderRight(void);
	void UpdateEncoder(void);
	void UpdateGyro(void);

	int GetADC(ADC_HandleTypeDef *hadc, uint32_t channel);


#endif /* SENSOR_H_ */
