#include <Mouse/global.h>
#include "tim.h"
#include "adc.h"
#include "Mouse/sensor.h"
#include "tim.h"

#define AD_WAIT_US 15

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TIM_OC_InitTypeDef sConfigOC;
	uint8_t buff = 0;
	float wall_gain_fix_r = 1.0f;
	float wall_gain_fix_l = 1.0f;
	float duty_right = 0.0f;
	float duty_left = 0.0f;
	float angle_out;

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
			if(wall_fr.val > wall_fr.threshold)	buff = buff | 0x10;
			if(wall_ff.val > wall_ff.threshold)	buff = buff | 0x04;
			if(wall_fl.val > wall_fl.threshold)	buff = buff | 0x01;

			if(wall_r.val > wall_r.threshold)	{
				buff = buff | 0x08;
				wall_gain_fix_r = 0.8f;
			}else{
				wall_gain_fix_r = 0.8f;
			}
			if(wall_l.val > wall_l.threshold)	{
				buff = buff | 0x02;
				wall_gain_fix_l = 0.8f;
			}else{
				wall_gain_fix_l = 0.80f;
			}

			LedDisplay(&buff);
			UpdateEncoder();
			UpdateGyro();

			//PID
			if(MF.FLAG.WCTRL){
				//偏差角速度の算出
				omega_control.dif = (center.omega_dir * center.omega_target) - center.omega_rad;
				omega_control.p_out = gain_now.omega_kp * omega_control.dif;
				if((omega_control.i_out >= 0.1f) && (omega_control.dif > 0) ){
					omega_control.i_out = 0.1f;
				}else{
					omega_control.i_out += gain_now.omega_ki * omega_control.dif;
				}

				omega_control.out = omega_control.p_out + omega_control.i_out;
			}else{
				omega_control.out = 0;
			}

			if(MF.FLAG.VCTRL){
				//偏差の計算
				vel_ctrl_R.dif = (center.vel_target * vel_ctrl_R.dir) - center.velocity;
				vel_ctrl_L.dif = (center.vel_target * vel_ctrl_L.dir) - center.velocity;

				//偏差のP制御
				vel_ctrl_R.p_out = gain_now.vel_kpR * vel_ctrl_R.dif;
				vel_ctrl_L.p_out = gain_now.vel_kpL * vel_ctrl_L.dif;
				//偏差のI制御
				if((vel_ctrl_R.i_out >= 0.1f) && (vel_ctrl_R.dif > 0) ){
					vel_ctrl_R.i_out = 0.1f;
				}else{
					vel_ctrl_R.i_out += gain_now.vel_kiR * vel_ctrl_R.dif;
				}
				if((vel_ctrl_L.i_out >= 0.1f) && (vel_ctrl_L.dif > 0) ){
					vel_ctrl_L.i_out = 0.1f;
				}else{
					vel_ctrl_L.i_out += gain_now.vel_kiL * vel_ctrl_L.dif;
				}

//				vel_ctrl_R.i_out += gain_now.vel_kiR * vel_ctrl_R.dif;
//				vel_ctrl_L.i_out += gain_now.vel_kiL * vel_ctrl_L.dif;

				//PID制御値を統合
				vel_ctrl_R.out = vel_ctrl_R.p_out + vel_ctrl_R.i_out;
				vel_ctrl_L.out = vel_ctrl_L.p_out + vel_ctrl_L.i_out;

			}else{
				vel_ctrl_R.out = 0;
				vel_ctrl_L.out = 0;
			}

			if(MF.FLAG.CTRL && (center.vel_target > 0.1f)){
				wall_l.dif = wall_l.val - wall_l.base;
				wall_r.dif = wall_r.val - wall_r.base;

				wall_l.diff = wall_l.dif - wall_l.pre;
				wall_r.diff = wall_r.dif - wall_r.pre;

				if(SREF_MIN_L < wall_l.dif && wall_l.dif < SREF_MAX_L){
//					wall_l.out = 1.20f * wall_gain_fix_l * gain_search1.wall_kp * wall_l.dif;
					wall_l.out = wall_gain_fix_l * gain_search1.wall_kp * wall_l.dif;

					wall_l.out += gain_search1.wall_kd * wall_l.diff;
				}
				if(SREF_MIN_R < wall_r.dif && wall_r.dif < SREF_MAX_R){
					wall_r.out = wall_gain_fix_r * gain_search1.wall_kp * wall_r.dif;
					wall_r.out += gain_search1.wall_kd * wall_r.diff;
				}

				if(wall_r.out > 0.1f) wall_r.out = 0.1f;
				if(wall_l.out > 0.1f) wall_l.out = 0.1f;


				wall_l.pre = wall_l.dif;
				wall_r.pre = wall_r.dif;

			}else {
				wall_r.out = 0;
				wall_l.out = 0;
			}

			utsutsu_time++;

			break;
		case 3:
			if(MF.FLAG.ACTRL && (center.vel_target > 0.1f)){
				angle_out = (center.angle_target - center.angle) * gain_now.angle_kp + (center.angle - center.pre_angle) * gain_now.angle_kd;
				center.pre_angle = center.angle;
			}else{
				angle_out = 0.0f;
			}


			duty_left = vel_ctrl_L.out - omega_control.out + wall_l.out - angle_out;
			duty_right = vel_ctrl_R.out + omega_control.out + wall_r.out + angle_out;

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
			sConfigOC.Pulse = (uint16_t)(1000 * duty_left);

			sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
			sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
			sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
			sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

			HAL_TIM_PWM_ConfigChannel(&htim1,&sConfigOC,TIM_CHANNEL_1);

			sConfigOC.Pulse = (uint16_t)(1000 * duty_right);

			HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_2);

			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);

			if(utsutsu_time >= MEMORY){
				utsutsu_time = MEMORY - 1;
			}else{
//				Wall Sensor
/*				test1[utsutsu_time] = wall_r.val;
				test2[utsutsu_time] = wall_r.out;
				test3[utsutsu_time] = wall_l.out;
				test4[utsutsu_time] = wall_l.val;
*/
//				Translation

/*				test1[utsutsu_time] = encoder_r.velocity;
				test2[utsutsu_time] = encoder_l.velocity;
				test1[utsutsu_time] = center.angle;
				test2[utsutsu_time] = angle_out;
				test3[utsutsu_time] = center.vel_target;
				test4[utsutsu_time] = center.velocity;
*/
//				Revolution

/*				test1[utsutsu_time] = center.omega_rad;
				test2[utsutsu_time] = center.omega_dir * center.omega_target;
				test3[utsutsu_time] = center.angle;
				test4[utsutsu_time] = center.distance;
*/
//				Slalom
/*				test1[utsutsu_time] = center.omega_dir * center.omega_target;
				test2[utsutsu_time] = center.omega_rad;;
				test3[utsutsu_time] = center.vel_target;
				test4[utsutsu_time] = center.velocity;
*/
			}

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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {

}
