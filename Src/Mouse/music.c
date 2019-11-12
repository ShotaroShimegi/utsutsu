
#include"tim.h"
#include"Mouse/global.h"

void MelodySummer(void)
{
	Melody(g6,100);
	Melody(c7,100);
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	WaitMs(50);
	Melody(c7,50);
	WaitMs(50);
	Melody(c7,200);
}

void MelodyRayearth(void){
	Melody(f5,100);
	WaitMs(10);
	Melody(f5,600);
	Melody(c6,400);
	Melody(d6,200);
	Melody(a5h,600);
}

void MelodyGoal(void)
{
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);
}

void MelodyUrara(void){
	Melody(d7,100);
	Melody(a6,100);
	WaitMs(10);
	Melody(a6,200);
	WaitMs(50);

	Melody(d7,100);
	Melody(a6,100);
	WaitMs(10);
	Melody(a6,200);
	WaitMs(50);

	Melody(d7,100);
	Melody(c7,100);
	Melody(d7,100);
	Melody(c7,100);
	Melody(a6,200);
	WaitMs(50);

}

void MelodyMrLawrence()
{
	Melody(d7,100);
	Melody(e7,100);
	Melody(d7,100);
	Melody(a6,100);
	Melody(d7,100);

	WaitMs(400);

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
	WaitMs(ms);

	if(HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_4) != HAL_OK){
		Error_Handler();
	}
}
