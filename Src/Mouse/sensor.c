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
	base_l = wall_l.val;										//現在の左側のセンサ値で決定
	base_r = wall_r.val;										//現在の右側のセンサ値で決定

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
	if(wall_ff.val > wall_ff.threshold){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x88;								//壁情報を更新
		tmp = 0x06;										//1番目と2番目のLEDを点灯させるよう設定
	}
	//----Check Right----
	if(wall_r.val > wall_r.threshold){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x44;								//壁情報を更新
		tmp |= 0x01;									//0番目のLEDを点灯させるよう設定
	}
	//----Check Left----
	if(wall_l.val > wall_l.threshold){
		wall_info |= 0x11;								//Apdating Wall Data
		tmp |= 0x08;									//3番目のLEDを点灯させるよう設定
	}

//	printf("L:%d, F:%d R:%d\n",wall_l.val, wall_ff.val, wall_r.val);

}

void EncoderGyroTest()
{
	reset_distance();
	time = 0;
	centor.angle = 0;

	MF.FLAG.WCTRL = 0;
	MF.FLAG.VCTRL = 0;
	SetMotionDirection(FORWARD);
	StartMotion();

	sensor_start();

	while(1){
//		totalR_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096) / 4096;
//		totalL_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096) / 4096;

		printf("R_dist:%4lf L_dist%4lf Gyro:%4lf \n",encoder_r.distance,encoder_l.distance,centor.angle);

//		printf("R_dist:%4lf R_vel:%4lf L_dist%4lf L_vel:%4lf Gyro:%4lf \n",encoder_r.distance,encoder_r.velocity, encoder_l.distance, encoder_l.velocity, centor.angle);
//		printf("R_vel:%4lf L_vel:%4lf Gyro:%4lf \n",encoder_r.velocity,encoder_l.velocity,centor.angle);

		ms_wait(500);
	}

}

void sensor_start(){

	reset_distance();

	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);

	/*Basic Timer Start*/
	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start(&htim6);


}
void sensor_stop(){
	HAL_TIM_Base_Stop_IT(&htim6);
	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_ALL);
}
void sensor_check()
{
	uint8_t buff;

	printf("Timer Start!\n");

	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start(&htim6);

//	HAL_TIM_Base_Start_IT(&htim6);	<- 何故かこれだと上手くいかない

	while(1){
		//printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d \r\n", wall_l.dif, wall_fl.dif, wall_ff.dif, wall_fr.dif, wall_r.dif);
		printf("ad_l: %4d ad_ff:%4d ad_r:%4d \n", wall_l.val,wall_ff.val,wall_r.val);
		//----LEDが4つの場合----
		if(wall_fr.val > wall_fr.threshold){
			buff = buff | 0x10;
		}
		if(wall_r.val > wall_r.threshold){
			buff = buff | 0x08;
		}
		if(wall_ff.val > wall_ff.threshold){
			buff = buff | 0x04;
		}
		if(wall_l.val > wall_l.threshold){
			buff = buff | 0x02;
		}
		if(wall_fl.val > wall_fl.threshold){
			buff = buff | 0x01;
		}
		LedDisplay(&buff);
		ms_wait(1000);

		}

}

void Tim6WaitUs(uint16_t us){
	uint16_t delay = __HAL_TIM_GET_COUNTER(&htim6) + us;
	while(__HAL_TIM_GET_COUNTER(&htim6) < delay);

}

int16_t GetEncoderLeft(void){
	int16_t count = 0;
	uint16_t enc_val = TIM3->CNT;
	TIM3->CNT = 0;

	if(enc_val > 32767){
		count = (int16_t)(65536 - enc_val);
	}else{
		count = -(int16_t)enc_val;
	}

	return count;

}

int16_t GetEncoderRight(void)
{
	int16_t count = 0;
	uint16_t enc_val = TIM4->CNT;
	TIM4->CNT = 0;

	if(enc_val > 32767){
		count = -(int16_t)(65536 - enc_val);
	}else{
		count = (int16_t)enc_val;
	}

	return count;

}

void UpdateGyro(void)
{
	centor.omega_deg = GyroRead() - gyro_base;
	centor.omega_rad = centor.omega_deg * KW;
	centor.angle += (centor.omega_deg + centor.pre_omega_deg) * 0.5 * 0.001;
	centor.pre_omega_deg = centor.omega_deg;

}
void UpdateEncoder(void)
{
	encoder_r.pulse = GetEncoderRight();
	encoder_l.pulse = GetEncoderLeft();

	encoder_r.sum += encoder_r.pulse;
	encoder_l.sum += encoder_l.pulse;
	encoder_r.distance = Kxr * encoder_r.sum;
	encoder_l.distance = Kxr * encoder_l.sum;

	encoder_r.velocity = Kxr * (float)encoder_r.pulse;
	encoder_l.velocity = Kxr * (float)encoder_l.pulse;

	centor.distance = (encoder_r.distance + encoder_l.distance) * 0.5;
	centor.velocity = (encoder_r.velocity + encoder_l.velocity) * 0.5;

}
int GetADC(ADC_HandleTypeDef *hadc, uint32_t channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

  HAL_ADC_ConfigChannel(hadc, &sConfig);

  HAL_ADC_Start(hadc);
  HAL_ADC_PollForConversion(hadc, 100);
  return HAL_ADC_GetValue(hadc);
}
