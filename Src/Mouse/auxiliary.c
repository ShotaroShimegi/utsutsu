#include "Mouse/global.h"
#include "tim.h"

void WaitMs(unsigned int ms)
{
	HAL_Delay(ms);
}

void ModeSelect(uint8_t *mode)
{
	uint16_t encR,encL;
	uint16_t nowR = 0;
	uint16_t nowL = 0;
	uint16_t preR;

	*mode = 0x00;
	TIM3->CNT = 0;
	TIM4->CNT = 0;

	//====Show Mode====
	printf(" mode: 0\r");

	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);

	//====Mode Select do====
	do{
		preR = nowR;

		encL = TIM3->CNT;
		encR = TIM4->CNT;

		nowR = (uint16_t)(encR / 4300);
		nowL = (uint16_t)(encL / 30000);

		//WaitMs(50);
		 *mode = nowR;
		//LEDで現在の値を表示
		LedDisplay(mode);
		if(nowR - preR != 0){
			printf(" mode:%2d\n", *mode);
			Melody(c6 + (60 * *mode),100);
		}
	}while(nowL != 1);

	printf("Finish :  This is mode %2d\n", *mode);
	Melody(c6 + + (60 * *mode),500);

	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_ALL);

	TIM3->CNT = 0;
	TIM4->CNT = 0;

}

void StartWaiting(void)
{
	MF.FLAG.VCTRL = 0;
	MF.FLAG.WCTRL = 0;

	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	HAL_TIM_Base_Start(&htim6);

	printf("Ready???\r\n");

	while(1){
		if(wall_ff.val > WALL_START){
			Melody(e6,300);
			Melody(f6,300);
			Melody(g6,300);
			WaitMs(1000);
			break;
		}
	}

	center.angle = 0.0f;
	center.angle_target = 0.0f;
}

void FirstAction(void)
{
	DisableMotor();
	StartTimer();

	MF.FLAG.CTRL = 0;
	GetControlBaseValue();
	AutoCalibration(0.50,0.50);

	SetMotionDirection(FORWARD);
	DisableMotor();

}

void SetParams(params *instance)
{
	//====Params Structure====
	params_now.vel_max = instance->vel_max;
	params_now.big_vel_max = instance->big_vel_max;
	params_now.accel = instance->accel;
	params_now.omega_max = instance->omega_max;
	params_now.omega_accel = instance->omega_accel;
	params_now.big90_omega_max = instance->big90_omega_max;
	params_now.big90_omega_accel = instance->big90_omega_accel;
	params_now.big180_omega_max = instance->big180_omega_max;
	params_now.big180_omega_accel = instance->big180_omega_accel;

	params_now.TURN90_before = instance->TURN90_before;
	params_now.TURN90_after = instance->TURN90_after;
	params_now.big90_before = instance->big90_before;
	params_now.big90_after = instance->big90_after;
	params_now.big180_before = instance->big180_before;
	params_now.big180_after = instance->big180_after;

	center.velocity_max = instance->vel_max;
	center.omega_max = instance->omega_max;
	center.velocity_min = 0.0f;

	accel_time = params_now.vel_max / params_now.accel;		//Time for Straight Accel
//	omega_accel_time = accel_time / 3;

}

void SetGain(gain *instance)
{
	gain_now.vel_kp = instance->vel_kp;
	gain_now.vel_ki = instance->vel_ki;
	gain_now.omega_kp = instance->omega_kp;
	gain_now.omega_ki = instance->omega_ki;
	gain_now.wall_kp = instance->wall_kp;
	gain_now.wall_kd = instance->wall_kd;
	gain_now.angle_kp = instance->angle_kp;
	gain_now.angle_kd = instance->angle_kd;
}

void AutoCalibration(float constant_l, float constant_r)
{
//	wall_l.threshold = (uint16_t)(wall_l.dif * constant_l);
//	wall_r.threshold = (uint16_t)(wall_r.dif * constant_r);
//	printf("threshold %d, %d :: dif %d, %d\r\n",wall_l.threshold, wall_r.threshold, wall_l.dif, wall_r.dif);

	wall_l.threshold = WALL_BASE_L;
	wall_ff.threshold = WALL_BASE_F;
	wall_r.threshold = WALL_BASE_R;

}
void LedDisplay(uint8_t *led)
{
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, *led&0x01);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, *led&0x02);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, *led&0x04);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, *led&0x08);
	HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, *led&0x10);

}

void ResetDistance()
{
	/* Initialize Distance */
	encoder_r.distance = 0;
	encoder_l.distance = 0;
	center.distance = 0;

	/* Initialize Integral Variable  */
	encoder_r.sum = 0;
	encoder_l.sum = 0;
}

void CheckBattery(void)
{
	if(HAL_GPIO_ReadPin(BATTERY_GPIO_Port,BATTERY_Pin) == 0)
	{
		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim11,TIM_CHANNEL_1);

		HAL_TIM_Base_Stop(&htim6);
		DisableMotor();

		printf("Voltage Out!\n");
		MelodyMrLawrence();
		while(1);

	}
	printf("Voltage ALL GREEN\n");
}

void CalculateNormalParams(params *instance,float velocity_max, float velocity,float accel)
{
	float time_90mm = HALF_MM / velocity * 0.001;
	omega_accel_time = time_90mm / 3.0f;

	instance->vel_max = velocity;									//Unit is [m/s] = [mm/ms]
	instance->big_vel_max = velocity_max;
	instance->accel = accel;										//Unit is [m/s/s]
	instance->omega_max = 3.0f / (1.0f+3.0f) * 2.0f * (Pi * 0.5f) / time_90mm;	//Max Angular-Velocity [rad/s]
	instance->omega_accel = 3.0f * instance->omega_max / time_90mm; 	//Angular Acceleration [rad/s/s]

}

void CalculateBigParams(params *instance,float velocity,float accel)
{
	float time_180mm = ONE_MM / velocity * 0.001;
	float time_360mm = 2 * ONE_MM / velocity * 0.001;				//Unit is [ms]

	big90_omega_accel_time = time_180mm / 2.0f;						//Unit is [ms]
	big180_omega_accel_time = time_360mm / 2.5f;

	instance->big90_omega_max = 2.0f /(1.0f + 2.0f) * 2.0f * Pi * 0.5f /time_180mm;
	instance->big90_omega_accel = 2.0f * instance->big90_omega_max / time_180mm;
	instance->big180_omega_max = 2.5f / (1.0f + 2.5f) * 2.0f * Pi / time_360mm;
	instance->big180_omega_accel =2.0f * instance->big180_omega_max / time_360mm;

}

void ApplyOffsetParams(params *instance, uint8_t turn90_before,uint8_t turn90_after,uint8_t big90_before,uint8_t big90_after,uint8_t big180_before,uint8_t big180_after)
{
	instance->TURN90_before = turn90_before;
	instance->TURN90_after = turn90_after;
	instance->big90_before = big90_before;
	instance->big90_after = big90_after;
	instance->big180_before = big180_before;
	instance->big180_after = big180_after;
}

void ApplyGain(gain *instance, float vel_kp,float vel_ki, float omega_kp,float omega_ki,float wall_kp,float wall_kd, float angle_kp, float angle_kd){

	instance->vel_kp = vel_kp;
	instance->vel_ki = vel_ki;
	instance->omega_kp = omega_kp;
	instance->omega_ki = omega_ki;
	instance->wall_kp = wall_kp;
	instance->wall_kd = wall_kd;
	instance->angle_kp = 0.005f;
	instance->angle_kd = 0.0f;

}

void FailSafe(void)
{
	DisableMotor();
	StopTimer();
	WaitMs(500);
	MelodyMrLawrence();
	while(1){
//		printf("angle: %lf, target: %lf\n",center.angle,center.angle_target);
		WaitMs(100);
	}

}

void FailCheck(void)
{
	float dif_angle;

	dif_angle = center.angle - center.angle_target;

	if((dif_angle > 20.0f) || (dif_angle < -20.0f)){
		MelodySummer();
		FailSafe();
	}else if((center.accel > 14.0f) && (MF.FLAG.SCND == 1)){
		FailSafe();
	}


}

