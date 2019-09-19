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
	float sen_out_right = 0.0;
	float sen_out_left = 0.0;
	float duty_right =0.0;
	float duty_left = 0.0;

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

			//====並進加減速処理====
				//----減速----
			if(MF.FLAG.VCTRL){
				if(MF.FLAG.DECL){
					center.vel_target -= params_now.accel * 0.001;
					if(center.vel_target < 0)center.vel_target = 0.0f;
				}
				//----加速----
				else if(MF.FLAG.ACCL){
					center.vel_target += params_now.accel * 0.001;
					if(center.vel_target > params_now.vel_max)center.vel_target = params_now.vel_max;
				}
			}

			break;

		case 1:
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,SET);
			HAL_GPIO_WritePin(LED_FL_GPIO_Port, LED_FL_Pin,SET);

			Tim6WaitUs(AD_WAIT_US);

			wall_r.val = GetADC(&hadc1,ADC_CHANNEL_1);
			wall_fl.val = GetADC(&hadc1,ADC_CHANNEL_5);

			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,RESET);
			HAL_GPIO_WritePin(LED_FL_GPIO_Port, LED_FL_Pin,RESET);

			//====回転加速処理====
			if(MF.FLAG.WCTRL){
				if(MF.FLAG.WDECL){
					center.omega_target -= params_now.omega_accel * 0.001;
					if(center.omega_target < 0)	center.omega_target = 0.0f;
				}
				else if(MF.FLAG.WACCL){
					center.omega_target += params_now.omega_accel * 0.001;
					if(center.omega_target > params_now.omega_max)	center.omega_target = params_now.omega_max;
				}
			}

			break;
		case 2:
			time++;
			if(time > 2000){
				time = 1999;
			}
			UpdateEncoder();
			UpdateGyro();

			//PID
			if(MF.FLAG.WCTRL){
				//偏差角速度の算出
				omega.dif = (center.omega_dir * center.omega_target) - center.omega_rad;
				omega.p_out = gain_now.omega_kp * omega.dif;
				omega.i_out += gain_now.omega_ki * omega.dif;
				omega.out = omega.p_out + omega.i_out;
			}else{
				omega.out = 0;
			}

			if(MF.FLAG.VCTRL){
				//偏差の計算
				vel_ctrl_R.dif = (center.vel_target * vel_ctrl_R.dir) - encoder_r.velocity;
				vel_ctrl_L.dif = (center.vel_target * vel_ctrl_L.dir) - encoder_l.velocity;
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

			//壁制御フラグアリの場合
			if(MF.FLAG.CTRL && center.vel_target > 0.1){
				wall_l.dif = wall_l.val - wall_l.threshold;
				wall_r.dif = wall_l.val - wall_l.threshold;
				if(SREF_MIN_L < wall_l.dif && wall_l.dif < SREF_MAX_L){
					sen_out_left = gain_search1.wall_kp * wall_l.dif;
				}
				if(SREF_MIN_R < wall_r.dif && wall_r.dif < SREF_MAX_R){
					sen_out_right = gain_search1.wall_kp * wall_r.dif;
				}
			}else {
				sen_out_right = 0;
				sen_out_left = 0;
			}



			if(time < 1999){
//			Wall Sensor
				test1[(uint16_t)(time*0.1)] = center.distance;
				test2[(uint16_t)(time*0.1)] = wall_l.val;
				test3[(uint16_t)(time*0.1)] = wall_r.val;


//			Translation
 /*
  *  			test1[(uint16_t)(time*0.1)] = encoder_r.velocity;
				test2[(uint16_t)(time*0.1)] = encoder_l.velocity;
				test3[(uint16_t)(time*0.1)] = center.vel_target;
*/
//			Revolution
/*				test1[(uint16_t)(time*0.1)] = center.omega_rad;
				test2[(uint16_t)(time*0.1)] = center.omega_dir * center.omega_target ;
				test3[(uint16_t)(time*0.1)] = center.angle;
*/
			}

			break;
		case 3:
			duty_left = vel_ctrl_L.out - omega.out + sen_out_left;
			duty_right = vel_ctrl_R.out + omega.out + sen_out_right;

			if(duty_left < 0){
				duty_left = -duty_left;
				HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
				HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,RESET);
			}else{
				HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
				HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);
			}

			if(duty_right < 0){
				duty_right = -duty_right;
				HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,SET);
				HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,RESET);
			}else{
				HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
				HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);
			}

			if(duty_left > 0.5f) duty_left = 0.5f;
			if(duty_left < 0.01f) duty_left = 0.01f;

			if(duty_right > 0.5f) duty_right = 0.5f;
			if(duty_right < 0.01f) duty_right = 0.01f;

	//Config Setting
			sConfigOC.OCMode = TIM_OCMODE_PWM1;
			sConfigOC.Pulse = (uint16_t)(1000 * duty_right);

			sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
			sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
			sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
			sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

			HAL_TIM_PWM_ConfigChannel(&htim1,&sConfigOC,TIM_CHANNEL_1);

			sConfigOC.Pulse = (uint16_t)(1000 * duty_left);

			HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_2);

			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			break;
		}
		tp = (tp+1) % 4;

	}	//---htim6 End---

	else if(htim->Instance == htim1.Instance)
	{
	}//---htim1 End--

	else if(htim->Instance == htim2.Instance)
	{
	}//---htim2 End--

}





/*------------------------------------------------------------
		センサ系の割り込み
------------------------------------------------------------*/

/*
void Cmt2IntFunc(){



//物理量で扱いやすいm/s = mm/msに変換
	center.vel = (vel_ctrl_R.real + vel_ctrl_L.real) * 0.5;
	center.distance = (encoder_r.distance + encoder_l.distance) * 0.5;

	//PIDしてみる？
	if(MF.FLAG.WCTRL){
		//偏差角速度の算出
		omega.dif = (center.omega_dir * center.omega_target) - center.omega_rad;

//		omega.p_out = gain_now.omega_kp * omega.dif * TREAD_mm * 0.5 * 0.001;
//		omega.i_out += gain_now.omega_ki * omega.dif * TREAD_mm * 0.5 * 0.001;

		omega.p_out = gain_now.omega_kp * omega.dif;
		omega.i_out += gain_now.omega_ki * omega.dif;
		omega.out = omega.p_out + omega.i_out;
	}else{
		omega.out = 0;
	}

	//超信地旋回で無い　かつ　低速域
	if(MF.FLAG.REVOL == 0 && center.vel_target < 0.20f){
		sen_ctrl = 0;
	}


}
*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

}
