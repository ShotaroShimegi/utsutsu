/*
割込関係の処理
左右モータへのPWM
左右のエンコーダ処理
壁センサ処理
ジャイロ処理
制御値確定などなど

*/
#include <Mouse/global.h>
#include "tim.h"
#include "adc.h"
#include "Mouse/sensor.h"
#include "tim.h"

#define AD_WAIT_US 15

uint32_t buff_array[6];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TIM_OC_InitTypeDef sConfigOC;

	if(htim->Instance == htim6.Instance){
		switch(tp){
		case 0:
			HAL_GPIO_WritePin(LED_L_GPIO_Port, LED_L_Pin,SET);
			HAL_GPIO_WritePin(LED_FF_GPIO_Port, LED_FF_Pin,SET);
			HAL_GPIO_WritePin(LED_FR_GPIO_Port, LED_FR_Pin,SET);

			Tim6WaitUs(AD_WAIT_US);

			wall_l.val = GetADC(&hadc1,ADC_CHANNEL_4);
			wall_ff.val = GetADC(&hadc1,ADC_CHANNEL_2);
			wall_fr.val = GetADC(&hadc1,ADC_CHANNEL_0);


			HAL_GPIO_WritePin(LED_L_GPIO_Port, LED_L_Pin,RESET);
			HAL_GPIO_WritePin(LED_FF_GPIO_Port, LED_FF_Pin,RESET);
			HAL_GPIO_WritePin(LED_FR_GPIO_Port, LED_FR_Pin,RESET);

			break;

		case 1:
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,SET);
			HAL_GPIO_WritePin(LED_FL_GPIO_Port, LED_FL_Pin,SET);

			Tim6WaitUs(AD_WAIT_US);

			wall_r.val = GetADC(&hadc1,ADC_CHANNEL_1);
			wall_fl.val = GetADC(&hadc1,ADC_CHANNEL_5);

			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,RESET);
			HAL_GPIO_WritePin(LED_FL_GPIO_Port, LED_FL_Pin,RESET);

			break;
		case 2:
			UpdateEncoder();

			break;
		case 3:
			break;
		}
		tp = (tp+1) % 4;

		UpdateGyro();

	}	//---htim6 End---

	else if(htim->Instance == htim1.Instance)
	{
//Config Setting
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 100;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		HAL_TIM_PWM_ConfigChannel(&htim1,&sConfigOC,TIM_CHANNEL_1);

	}
	else if(htim->Instance == htim2.Instance)
	{
		//Config Setting
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 100;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_2);
	}

}



//+++++++++++++++++++++++++++++++++++++++++++++++
//Mtu3IcCmDIntFunction
//左モータの制御関数
// PWMを１パルス送るごとに呼び出される
// RX631では1msよりは短くなることが多いから計算量に注意
//+++++++++++++++++++++++++++++++++++++++++++++++
/*
void Mtu3IcCmDIntFunc(void){		//パルス一周期でやると呼び出される左モータの制御関数
	volatile uint16_t paraL0 = 0;
	volatile uint16_t paraL1 = 1000;
	volatile float output = 0.0f;

//	pin_write(P54,0);
//	pin_write(P55,0);

	//duty_l = Kvolt * accel_l / VOLT_BAT + kpvL + kpdL;
	duty_l = vel_ctrl_L.duty_out - omega.duty_out;
	apid_G = duty_l;

	output = vel_ctrl_L.out - omega.out;

	if(output < 0){
		MF.FLAG.L_DIR = 0;
		output = - output;
	}else if(output > 0){
		MF.FLAG.L_DIR = 1;
	}
	if(output > 0.70f) {
		output = 0.70f;
	}else if(output < 0.001f ){
		output = 0.001f;
	}

		paraL0 = (uint16_t)(float)((1 - output) * paraL1);					//デューティ逆計算

//		R_PG_Timer_SetTGR_C_MTU_U0_C3(paraL0);
//		R_PG_Timer_SetTGR_D_MTU_U0_C3(paraL1);

}
*/
/*
void Mtu4IcCmDIntFunc(void){			//右モータ制御関数
	volatile uint16_t paraR0 = 0;
	volatile uint16_t paraR1 = 1000;
	volatile float output = 0.0f;

//	pin_write(PA4,0);
//	pin_write(PA6,0);

	//duty_r = Kvolt * accel_r / VOLT_BAT + kpvR + kpdR;
	duty_r = vel_ctrl_R.duty_out + omega.duty_out;

	output = vel_ctrl_R.out + omega.out;

	if(output< 0.0f){
		MF.FLAG.R_DIR = 0;
		output= - output;
	}else if(output > 0.0f){
		MF.FLAG.R_DIR = 1;
	}

	number = output;

	if(output > 1.0f){
		output = 0.70f;
	}else if(output < 0.001f){
		output = 0.001f;
	}

	paraR0 = (uint16_t)(float)((1 - output) * paraR1);

//	R_PG_Timer_SetTGR_C_MTU_U0_C4(paraR0);
//	R_PG_Timer_SetTGR_D_MTU_U0_C4(paraR1);

}
*/

/*------------------------------------------------------------
		センサ系の割り込み
------------------------------------------------------------*/
/*
void Cmt1IntFunc(void){
//	int r_threshold = 0;
//	int l_threshold = 0;

	switch(tp){
	//タイマ内での分割処理
	case 0:
break;

	case 1:

		break;

	case 2:

		centor.omega_deg = GYRO_read();
		centor.omega_rad = centor.omega_deg * KW;
		centor.angle += (centor.omega_deg + centor.pre_omega_deg) * 0.5 * 0.001;
		centor.pre_omega_deg = centor.omega_deg;

		break;
		//----制御計算----
	case 3:
		//====加減速処理====
			//----減速----
		if(MF.FLAG.VCTRL){
			if(MF.FLAG.DECL){
				centor.vel_target -= params_now.accel * 0.001;
				if(centor.vel_target < 0){
					centor.vel_target = 0.0f;
				}
			}
			//----加速----
			else if(MF.FLAG.ACCL){
				centor.vel_target += params_now.accel * 0.001;
				if(centor.vel_target > params_now.vel_max){
					centor.vel_target = params_now.vel_max;
				}
			}
		}

		if(MF.FLAG.WCTRL){
			if(MF.FLAG.WDECL){
				omega.target -= params_now.omega_accel * 0.001;
				if(omega.target < 0.0f){
					omega.target = 0.0f;
				}
			}
			else if(MF.FLAG.WACCL){
				omega.target += params_now.omega_accel * 0.001;
				if(omega.target > params_now.omega_max){
					omega.target = params_now.omega_max;
				}
			}
		}

		//壁制御フラグアリの場合
		if(MF.FLAG.CTRL){
			//閾値の設定
			if(abs(wall_r.diff) > 8){
				r_threshold = wall_r.threshold + 10;
			}else{
				r_threshold = wall_r.threshold;
			}
			if(abs(wall_l.diff) > 8){
				l_threshold = wall_l.threshold + 10;
			}else{
				l_threshold = wall_l.threshold;
			}

			//偏差を統合
			if((wall_r.val > r_threshold) && (wall_l.val > l_threshold)){			//両方向に壁がある
				dif_total = wall_l.dif - wall_r.dif;
			}else if((wall_r.val < r_threshold) && (wall_l.val < l_threshold)){		//両方向に壁無し
				dif_total = 0;
			}else if(wall_r.val > r_threshold){						//右だけ壁アリ
				dif_total = -2 * wall_r.dif;
			}else{											//左だけ壁アリ
				dif_total = 2 * wall_l.dif;
			}

			//偏差
			sen_ctrl = gain_search1.wall_kp * dif_total + gain_search1.wall_kd * (pre_dif_total - dif_total);
			pre_dif_total = dif_total;

			if(targ_vel < 0.2){
				sen_ctrl = 0;
			}
			sen_ctrl = 0;
		}else {
			sen_ctrl = 0;
		}


	}

	//====タスクポインタで分割処理====
	tp = (tp+1) % 4;
}
*/
/*
void Cmt2IntFunc(){

	time++;
	time2++;
	if(time2 > 2000) time2 = 2000;

	//エンコーダの値取得
//	R_PG_Timer_GetCounterValue_MTU_U0_C1(&encoder_l.pulse);
//	R_PG_Timer_GetCounterValue_MTU_U0_C2(&encoder_r.pulse);

//	R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
//	R_PG_Timer_SetCounterValue_MTU_U0_C2(0);

	encoder_r.dif = encoder_r.pulse + (65536 * encoder_r.over_flag);
	encoder_l.dif = encoder_l.pulse + (65536 * encoder_l.over_flag);

	encoder_r.over_flag = 0;
	encoder_r.over_flag = 0;

	encoder_r.sum += encoder_r.dif;
	encoder_l.sum += encoder_l.dif;

	vel_ctrl_R.real = Kxr * (float)encoder_r.dif;
	vel_ctrl_L.real = Kxr * (float)encoder_l.dif;

	encoder_r.distance = Kxr * (float)encoder_r.sum;
	encoder_l.distance = Kxr * (float)encoder_l.sum;

//物理量計算
//物理量で扱いやすいm/s = mm/msに変換
	centor.vel = (vel_ctrl_R.real + vel_ctrl_L.real) * 0.5;
	centor.distance = (encoder_r.distance + encoder_l.distance) * 0.5;

//センサログ用
		log.test1[time2] = totalG_mm;//angle_G, sen_dr;
		log.test2[time2] = wall_l.val;//dif_angle, sen_dl;

		log.test3[time2] = wall_l.base;//angle_G;//ad_r, kvpR
		log.test4[time2] = wall_r.val;//omega_G;//ad_l, kvdR

		log.test5[time2] = wall_r.base;
		log.test6[time2] = dif_total;

//並進速度ログ用
		log.test1[time2] = centor.vel_target;
		log.test2[time2] = omega.out;

		log.test3[time2] = number;
		log.test4[time2] = vel_ctrl_R.out;

		log.test5[time2] = vel_ctrl_L.real;
		log.test6[time2] = vel_ctrl_R.real;

//回転速度ログ用
		log.test1[time2] = omega_direction * targ_omega;	//angle_G, sen_dr;
		log.test2[time2] = omega_G_rad;				//dif_angle, sen_dl;

		log.test3[time2] = vel_omega;				//angle_G;//ad_r, kvpR
		log.test4[time2] = angle_G;				//omega_G;//ad_l, kvdR

		log.test5[time2] = vel_R;
		log.test6[time2] = vel_L;

//スラローム
		log.test1[time2] = omega_direction * targ_omega;//angle_G, sen_dr;
		log.test2[time2] = omega_G_rad;//dif_angle, sen_dl;

		log.test3[time2] = vel_R;//angle_G;//ad_r, kvpR
		log.test4[time2] = vel_L;//omega_G;//ad_l, kvdR

		log.test5[time2] = totalG_mm;
		log.test6[time2] = angle_G;


	//PIDしてみる？
	if(MF.FLAG.WCTRL){
		//偏差角速度の算出
		omega.dif = (centor.omega_dir * omega.target) - centor.omega_rad;

//		omega.p_out = gain_now.omega_kp * omega.dif * TREAD_mm * 0.5 * 0.001;
//		omega.i_out += gain_now.omega_ki * omega.dif * TREAD_mm * 0.5 * 0.001;

		omega.p_out = gain_now.omega_kp * omega.dif;
		omega.i_out += gain_now.omega_ki * omega.dif;
		omega.out = omega.p_out + omega.i_out;
	}else{
		omega.out = 0;
	}

	//超信地旋回で無い　かつ　低速域
	if(MF.FLAG.REVOL == 0 && centor.vel_target < 0.20f){
		sen_ctrl = 0;
	}

	if(MF.FLAG.VCTRL){
		//偏差の計算
		vel_ctrl_R.dif = (centor.vel_target * vel_ctrl_R.dir) - vel_ctrl_R.real;
		vel_ctrl_L.dif = (centor.vel_target * vel_ctrl_L.dir) - vel_ctrl_L.real;
		//偏差のP制御
		vel_ctrl_R.p_out = gain_now.vel_kpR * vel_ctrl_R.dif;
		vel_ctrl_L.p_out = gain_now.vel_kpL * vel_ctrl_L.dif;

		//偏差のI制御
		vel_ctrl_R.i_out += gain_now.vel_kiR * vel_ctrl_R.dif;
		vel_ctrl_L.i_out += gain_now.vel_kiL * vel_ctrl_L.dif;

		//PID制御値を統合
		vel_ctrl_R.out = vel_ctrl_R.p_out + vel_ctrl_R.i_out;
		vel_ctrl_L.out = vel_ctrl_L.p_out + vel_ctrl_L.i_out;


	}else{
		vel_ctrl_R.out = 0;
		vel_ctrl_L.out = 0;
	}

}
*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

}
