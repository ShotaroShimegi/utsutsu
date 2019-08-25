#include <Mouse/global.h>
#include "tim.h"
#include "adc.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//get_base
//	制御用の基準値を取得する
// 引数：なし
// 戻り値：理想的な値を取得できたか　1:できた　0:できなかった
//+++++++++++++++++++++++++++++++++++++++++++++++
unsigned char get_base()
{
	unsigned char res = 0;										//理想的な値を取得できたか

	ms_wait(10);
	//----制御用の基準を取得----
//	base_l = ad_l;										//現在の左側のセンサ値で決定
//	base_r = ad_r;										//現在の右側のセンサ値で決定

	//----基準が理想的だとLED点滅----
	if((-50 < (int)(base_l - base_r)) && ((int)(base_l - base_r) < 50)){
		//左右で差が50以下である場合
		res = 1;										//resを1に
	}else{
	}
	printf("base:%d, %d\r\n", base_r, base_l);
	return res;											//理想的な値を取得できたかを返す

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_wall_info
//	壁情報取得を取得する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void get_wall_info()
{
	unsigned char tmp = 0;						//点灯させるLEDの場所
	//----壁情報の初期化----
	wall_info = 0x00;									//壁情報を初期化

	//----Check Front----
	if(wall_ff.dif > wall_ff.threshold){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x88;								//壁情報を更新
		tmp = 0x06;										//1番目と2番目のLEDを点灯させるよう設定
	}
	//----Check Right----
	if(wall_r.dif > wall_r.threshold){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x44;								//壁情報を更新
		tmp |= 0x01;									//0番目のLEDを点灯させるよう設定
	}
	//----Check Left----
	if(wall_l.dif > wall_l.threshold){
		wall_info |= 0x11;								//Apdating Wall Data
		tmp |= 0x08;									//3番目のLEDを点灯させるよう設定
	}

}

void enc_test(){
	reset_distance();
	time = 0;
/*	R_PG_Timer_StartCount_MTU_U0_C1();
	R_PG_Timer_StartCount_MTU_U0_C2();
	R_PG_Timer_StartCount_CMT_U1_C2();
*/
	while(1){
/*
		totalR_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096) / 4096;
		totalL_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096) / 4096;
*/		//printf("R_distance:%4lf L_distance:%4lf\r\n",encoder_r.distance, encoder_l.distance);
		ms_wait(500);
	}

}

void sensor_start(){
/*	R_PG_Timer_StartCount_MTU_U0_C1();	//エンコーダ左右
	R_PG_Timer_StartCount_MTU_U0_C2();

	R_PG_Timer_StartCount_CMT_U0_C1();	//壁センサ用LED起動タイマ
	R_PG_Timer_StartCount_CMT_U1_C2();	//エンコーダ処理，PID計算用タイマ
*/
}
void sensor_stop(){
/*	R_PG_Timer_HaltCount_MTU_U0_C1();
	R_PG_Timer_HaltCount_MTU_U0_C2();

	R_PG_Timer_HaltCount_CMT_U0_C1();
	R_PG_Timer_HaltCount_CMT_U1_C2();

	pin_write(PE0,0);
	pin_write(PE1,0);
	pin_write(PE2,0);
	pin_write(PE3,0);
	pin_write(PE4,0);

	melody(c6,1000);
*/
}
void sensor_check()
{
	uint8_t buff;
	MF.FLAG.CTRL = 1;

//	HAL_TIM_Base_Start_IT(&htim6);

	printf("Timer Start!\n");

//	get_base();
	while(1){
		//printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d \r\n", wall_l.dif, wall_fl.dif, wall_ff.dif, wall_fr.dif, wall_r.dif);
		printf("ad_l: %4d ad_ff:%4d ad_r:%4d \r\n", wall_l.val,wall_ff.val,wall_r.val);
		//----LEDが4つの場合----
		if(wall_fr.dif > wall_fr.threshold){
			buff = buff | 0x10;
		}
		if(wall_r.dif > wall_r.threshold){
			buff = buff | 0x08;
		}
		if(wall_ff.dif > wall_ff.threshold){
			buff = buff | 0x04;
		}
		if(wall_l.dif > wall_l.threshold){
			buff = buff | 0x02;
		}
		if(wall_fl.dif > wall_fl.threshold){
			buff = buff | 0x01;
		}
		LedDisplay(&buff);
		ms_wait(1000);

		}
	MF.FLAG.CTRL = 0;

}

void Tim6WaitUs(uint16_t us){
	uint16_t time = __HAL_TIM_GET_COUNTER(&htim6) + us;
	while(__HAL_TIM_GET_COUNTER(&htim6) < time);

}

int16_t GetEncoderLeft(void){
	int16_t count = 0;
	uint16_t enc_val = TIM3->CNT;
	TIM3->CNT = 0;

	if(enc_val > 32767){
		count = enc_val - 65535;
	}else{
		count = enc_val;
	}

	return count;

}

int16_t GetEncoderRight(void)
{
	int16_t count = 0;
	uint16_t enc_val = TIM4->CNT;
	TIM4->CNT = 0;

	if(enc_val > 32767){
		count = enc_val - 65535;
	}else{
		count = enc_val;
	}

	return count;

}

int get_adc_value(ADC_HandleTypeDef *hadc, uint32_t channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

  HAL_ADC_ConfigChannel(hadc, &sConfig);

  HAL_ADC_Start(hadc);
  HAL_ADC_PollForConversion(hadc, 100);
  return HAL_ADC_GetValue(hadc);
}
