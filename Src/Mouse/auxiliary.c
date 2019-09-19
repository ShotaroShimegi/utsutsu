#include "Mouse/global.h"
#include "tim.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//wait
//	WaitMsは重複して使用する(以下参照)ことが出来ない
//	(動作が途中で止まる)。そのため、割り込み関数内では
//	正確な時間は測定できないがwhile文ループを用いて待機する
// 引数1：loop・・・待機するループ数
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void stay(unsigned int loop)
{
	while(loop--);					//loop回while文を回す
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//WaitMs
//	ミリ秒待機する
// 引数1：ms・・・待機時間[ミリ秒]
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void WaitMs(unsigned int ms)
{
	HAL_Delay(ms);
}

/*------------------------------------------------------------
		モードセレクト
------------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//select_mode
//	モードセレクトを行う
// 引数1：mode・・・モード番号を格納する変数のアドレス
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void ModeSelect(uint8_t *mode)
{
	uint16_t encR,encL;
	uint16_t nowR = 0;
	uint16_t nowL = 0;
	uint16_t preR;

	*mode = 0x00;									//変数の初期化
	TIM3->CNT = 0;
	TIM4->CNT = 0;

	//====Show Mode====
	printf(" mode: 0\r");						//モードをUARTで送信

	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);

	//====Mode Select do====
	do{
		preR = nowR;
		//preL = nowL;

		encL = TIM3->CNT;
		encR = TIM4->CNT;

		nowR = (uint16_t)(encR / 4300);
		nowL = (uint16_t)(encL / 30000);

		//WaitMs(50);
		 *mode = nowR;
		//LEDで現在の値を表示
		LedDisplay(mode);			//LEDがActiveLowの場合
		if(nowR - preR != 0){
			printf(" mode:%2d\r\n", *mode);
			Melody(c6 + (60 * *mode),100);
		}
	}while(nowL != 1);

	printf("Finish :  This is mode %2d\r\n", *mode);
	Melody(c6 + + (60 * *mode),500);

	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_ALL);

	TIM3->CNT = 0;
	TIM4->CNT = 0;

}

void MelodySummer(void)
{
	Melody(g6,100);
	Melody(c7,100);
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	HAL_Delay(50);
	Melody(c7,50);
	HAL_Delay(50);
	Melody(c7,200);
}

void MelodyMrLawrence()
{
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(a6,100);
	Melody(d7,100);

	HAL_Delay(400);

	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(e6,100);
	Melody(g7,100);
	Melody(e6,100);
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(c7,100);
	Melody(a6,100);

}

void Melody(uint32_t hz, uint32_t ms)
{
	TIM_OC_InitTypeDef sConfigOC;
//TIM8 Setting
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 840-1;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 100000 / hz;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//Config Setting
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse =  95000 / hz ;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//TIM8 OUTPUT
	if(HAL_TIM_PWM_Init(&htim8) != HAL_OK){
		Error_Handler();
	}
	if(HAL_TIM_PWM_ConfigChannel(&htim8,&sConfigOC,TIM_CHANNEL_4) != HAL_OK){
		Error_Handler();
	}
	if(HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4) != HAL_OK){
		Error_Handler();
	}
	HAL_Delay(ms);

	if(HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_4) != HAL_OK){
		Error_Handler();
	}
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
}

void FirstAction(void)
{
	DisableMotor();
	StartTimer();

	MF.FLAG.CTRL = 0;
	SetMotionDirection(FORWARD);

	AutoCalibration(0.30,0.60);
	time2 = 0;
	DriveAccel(SET_MM * 0.5);
	DriveDecel(SET_MM * 0.5, 1);

	DisableMotor();

}

void SetParams(params *instance)
{
	params_now.vel_max = instance->vel_max;
	params_now.accel = instance->accel;
	params_now.omega_max = instance->omega_max;
	params_now.omega_accel = instance->omega_accel;
}

void SetGain(gain *instance)
{
	gain_now.vel_kpR = instance->vel_kpR;
	gain_now.vel_kiR = instance->vel_kiR;
	gain_now.vel_kpL = instance->vel_kpL;
	gain_now.vel_kiL = instance->vel_kiL;
	gain_now.omega_kp = instance->omega_kp;
	gain_now.omega_ki = instance->omega_ki;
	gain_now.wall_kp = instance->wall_kp;
	gain_now.wall_kd = instance->wall_kd;
}

void AutoCalibration(float constant_l, float constant_r)
{
//	wall_l.threshold = (uint16_t)(wall_l.dif * constant_l);

	wall_l.threshold = WALL_BASE_L;
	wall_ff.threshold = WALL_BASE_F;
	wall_r.threshold = WALL_BASE_R;

//	wall_r.threshold = (uint16_t)(wall_r.dif * constant_r);
	printf("threshold %d, %d :: dif %d, %d\r\n",wall_l.threshold, wall_r.threshold, wall_l.dif, wall_r.dif);

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
		while(1){
		}
	}

	printf("Voltage ALL GREEN\n");

}
