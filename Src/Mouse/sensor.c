#include <Mouse/global.h>
#include "tim.h"
#include "adc.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//GetWallData
//	壁情報取得を取得する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void GetWallData()
{
	//----壁情報の初期化----
	wall_info = 0x00;									//壁情報を初期化

	//----Check Front----
	if(wall_ff.val > wall_ff.threshold){
		wall_info |= 0x88;								//壁情報を更新
	}
	//----Check Right----
	if(wall_r.val > wall_r.threshold){
		wall_info |= 0x44;								//壁情報を更新
	}
	//----Check Left----
	if(wall_l.val > wall_l.threshold){
		wall_info |= 0x11;								//Apdating Wall Data
	}

//	printf("L:%d, F:%d R:%d\n",wall_l.val, wall_ff.val, wall_r.val);

}

void EncoderGyroTest()
{
	ResetDistance();
	utsutsu_time = 0;
	center.angle = 0;

	MF.FLAG.WCTRL = 0;
	MF.FLAG.VCTRL = 0;

	DisableMotor();
	StartTimer();

	while(1){
//		totalR_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096) / 4096;
//		totalL_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096) / 4096;

		printf("R_dist:%4lf L_dist%4lf Gyro:%4lf \n",encoder_r.distance,encoder_l.distance,center.angle);
		WaitMs(500);
	}

}

void StartTimer()
{
//	ResetDistance();

	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);

	/*Basic Timer Start*/
	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start(&htim6);

}

//静止している時のみ使うこと

void StopTimer()
{
	HAL_TIM_Base_Stop_IT(&htim6);
	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_ALL);

	ResetDistance();
}

void CheckSensor()
{
	uint8_t buff;

	printf("Timer Start!\n");
	GetControlBaseValue();
	printf("base_r : %d, base_l : %d\n",wall_r.base,wall_l.base);

	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start(&htim6);

//	HAL_TIM_Base_Start_IT(&htim6);	<- Do not Use

	while(1){
		printf("ad_l: %4d ad_ff:%4d ad_r:%4d \n", wall_l.val,wall_ff.val,wall_r.val);
		if(wall_fr.val > wall_fr.threshold)	buff = buff | 0x10;
		if(wall_r.val > wall_r.threshold)	buff = buff | 0x08;
		if(wall_ff.val > wall_ff.threshold)	buff = buff | 0x04;
		if(wall_l.val > wall_l.threshold)	buff = buff | 0x02;
		if(wall_fl.val > wall_fl.threshold)	buff = buff | 0x01;

		LedDisplay(&buff);
		WaitMs(1000);
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
	center.omega_deg = ReadGyro() - gyro_base;
	center.omega_rad = center.omega_deg * KW;
	center.angle += (center.omega_deg + center.pre_omega_deg) * 0.5 * 0.001;
	center.pre_omega_deg = center.omega_deg;

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

	center.distance = (encoder_r.distance + encoder_l.distance) * 0.5;
	center.velocity = (encoder_r.velocity + encoder_l.velocity) * 0.5;

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

void GetControlBaseValue(){
	DisableMotor();
	StartTimer();
	HAL_Delay(100);
	wall_r.base = wall_r.val;
	wall_l.base = wall_l.val;
}
