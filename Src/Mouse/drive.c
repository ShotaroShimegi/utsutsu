#include <Mouse/global.h>
#include "tim.h"

/*==========================================================
		走行系関数
==========================================================*/
/*
		基本仕様として、
		ペリフェラル関数
			***Motion		ペリフェラルが絡む関数，マイコンごとに要変更
		基幹関数
			Drive*** 		速度制御をしながら走行，引数で目標速度or目標角度を指示
		走行関数
			HalfSection*** 	半区画走行，主にSlalom時の加速，減速用に利用
			GoOneSection*** 一区画動け
			Spin***			超信地旋回
			Slalom***		スラローム
			Fix				尻当て

*/

void HalfSectionAccel(uint8_t wall_read)
{
	MF.FLAG.CTRL = 1;
	DriveAccel(HALF_MM);
	if(wall_read) {
		GetWallData();
	}
}

void HalfSectionDecel()
{
	MF.FLAG.CTRL = 1;
	DriveDecel(HALF_MM,1);									//
}


void GoOneSectionStop()
{
 	HalfSectionAccel(GET_WALL_ON);
	HalfSectionDecel();

	ResetDistance();
	DisableMotor();
}

void GoOneSectionContinuous(){
	HalfSectionAccel(GET_WALL_OFF);
	HalfSectionAccel(GET_WALL_ON);
}

void SpinR90()
{
	MF.FLAG.CTRL = 0;

	SetMotionDirection(TURN_R);
	DriveSpin(ROT_ANGLE_R90);
	DisableMotor();

	SetMotionDirection(FORWARD);
}

void SlalomR90()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_search1.R90_before);

	MF.FLAG.CTRL = 0;
	DriveSlalom(-90);

	MF.FLAG.CTRL = 1;
	DriveAccel(params_search1.R90_after);

	GetWallData();

}


void SpinL90(void)
{
	MF.FLAG.CTRL = 0;

	SetMotionDirection(TURN_L);
	DriveSpin(ROT_ANGLE_L90);
	DisableMotor();
	SetMotionDirection(FORWARD);
}

void SlalomL90(void)
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_search1.L90_before);

	MF.FLAG.CTRL = 0;
	DriveSlalom(90);

	MF.FLAG.CTRL = 1;
	DriveAccel(params_search1.L90_after);

	GetWallData();

}

void Spin180()
{
	MF.FLAG.CTRL = 0;

	SetMotionDirection(TURN_R);
	DriveSpin(-180);
	DisableMotor();

}

void FixPosition(uint8_t flag)
{
	float save_params = params_now.vel_max;

	MF.FLAG.CTRL = 0;

	SetMotionDirection(BACK);
	WaitMs(200);

	ResetDistance();

	params_now.vel_max = 0.20f;

	DriveAccel(-(SET_MM * 0.5f));
	DriveDecel(-(SET_MM * 0.6f),1);

	params_now.vel_max = save_params;

	SetMotionDirection(FORWARD);				//前進するようにモータの回転方向を設定

	MF.FLAG.CTRL =1;
	if(flag == 0){
		DriveAccel(SET_MM);
	}else{
		DriveAccel(SET_MM * 0.5f);
		DriveDecel(SET_MM * 0.5f,1);
	}

}

void DriveAccel(float dist)
{
	float ics = center.distance;

	//====走行====
	//----走行開始----
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	center.omega_target = 0;

	StartMotion();

	//----走行----
	if(dist >= 0.0f){
		while(center.distance < ics + dist){
/*		if(MF.FLAG.WALL && flag == 0){
			encoder_r.distance = (dist + ics - 60) / Kxr;
			encoder_l.distance = (dist + ics - 60) / Kxr;
			MF.FLAG.WALL = 0;
			flag = 1;
		}
*/		}
	} else{
		while(center.distance > ics + dist);
	}


}

void DriveDecel(float dist, unsigned char rs)
{
	float ics = center.distance;
	float offset;

	//----走行開始----
	MF.FLAG.WCTRL = 1;
	MF.FLAG.VCTRL = 1;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	StartMotion();

	offset = rs * 0.5 * params_now.vel_max * maxindex * 1000;
	offset += 8.0f;

	//----走行----
	if(dist >= 0.0){
		while((center.distance + offset) < (dist + ics));
	}else{
		while((center.distance + offset) > (dist + ics));
	}
		if(rs){
			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 1;
			while(center.vel_target != 0.0f);
			WaitMs(500);
			MF.FLAG.WCTRL = 0;
			MF.FLAG.VCTRL = 0;

			//----停止措置----
			StopMotion();
			vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
			omega_control.i_out = 0;

		}
}

void DriveSpin(float theta)
{
	float offset;
	float ics = center.angle;

	if(theta > 0){
		center.omega_dir = 1;
	}else if(theta < 0){
		center.omega_dir = -1;
	}

	//----Setting Mouse Flag----
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;

	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;

	ResetDistance();

	offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;		//減速に必要な角度の絶対値計算
	center.vel_target = 0;
	center.omega_target = 0;
	omega_control.i_out = 0;

	StartMotion();

	if(theta > 0){
		//----走行----
		while(center.angle < ics + theta - offset);				//w-tグラフにおける速度増加部の面積　⇒　現在の回転角度

		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle < ics + theta);
		center.omega_target = 0;
		HAL_Delay(200);

	}else if (theta < 0){
		while(center.angle > ics + theta + offset);

		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle > ics + theta);
		center.omega_target = 0;
		HAL_Delay(200);

	}

	center.omega_target = 0;
	HAL_Delay(200);

	//----停止許可があれば停止----
	StopMotion();

	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;

}

void DriveSlalom(int16_t theta)
{
	float offset = 0;
	float offset_fix = 0;
	ResetDistance();

	center.angle = 0;
	center.omega_target = 0;

	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;

	if(theta > 0){					//Left Turn
		center.omega_dir = 1;
	}else if(theta < 0){			//Right Turn
		center.omega_dir = -1;
	}
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;

	StartMotion();

	offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;

	//====回転====
	if(center.omega_dir == 1){				//Left Turn
		offset_fix = 0.5f;

		while(center.angle + offset < theta - offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle  < theta - offset_fix){
			if(center.omega_target == 0) break;
		}
	} else if(center.omega_dir == -1){		//Right Turn
		offset_fix = 0.2f;

		while(center.angle - offset > theta + offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle > theta + offset_fix){
			if(center.omega_target == 0) break;

		}
	}

	omega_control.dir = 0;
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;
}

void StartMotion(void){
	TIM_OC_InitTypeDef sConfigOC;
//Config Setting
	  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	  sConfigOC.Pulse = 10;
	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
//OUTPUT
	if(HAL_TIM_PWM_ConfigChannel(&htim1,&sConfigOC,TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
	if(HAL_TIM_PWM_ConfigChannel(&htim2,&sConfigOC,TIM_CHANNEL_2) != HAL_OK){
		Error_Handler();
	}

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

	StartTimer();

}

void StopMotion(void)
{
	MF.FLAGS = 0x00;

	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);

	HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,RESET);

	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,RESET);

	DisableMotor();

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SetMotionDirection
//	進行方向を設定する
// arg1：d_dir・・・どの方向に進行するか  0桁目で左、1桁目で右の方向設定
// return：なし

//+++++++++++++++++++++++++++++++++++++++++++++++
void SetMotionDirection(uint8_t d_dir)
{
	HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin,SET);

	//====左モータ====
	switch(d_dir & 0x0f){									//0~3ビット目を取り出す
		//----正回転----
		case 0x00:
			vel_ctrl_L.dir= 1;				//左モータを正方向に
			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

			break;
		//----逆回転----
		case 0x01:
			vel_ctrl_L.dir = -1;			//左モータを逆方向に
			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,RESET);
			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){					//4~7ビット目を取り出す
		case 0x00:
			vel_ctrl_R.dir = 1;				//右を前進方向に設定
			HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);
			break;

		case 0x10:
			vel_ctrl_R.dir = -1;				//右を後進方向に設定
			HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,SET);
			HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,RESET);
			break;
	}
}

void DisableMotor(void)
{
	HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin,RESET);

	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

	HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);

	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;

	MF.FLAG.VCTRL = 0;
	MF.FLAG.WCTRL = 0;

	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.WDECL = 0;

}

void DriveTest(uint8_t *mode)
{
	uint8_t i = 0;

	while(1){
		printf("test_drive:\n");
		ModeSelect(mode);
		WaitMs(50);
		StartWaiting();

		switch(*mode){
			//----位置の調整----
			case 0:

				SetMotionDirection(BACK);
				FixPosition(1);
				HAL_Delay(1000);

				break;

			//----一区画定速走行----
			case 1:
				StartTimer();
				SetMotionDirection(FORWARD);
				utsutsu_time = 0;
				HalfSectionAccel(GET_WALL_OFF);
				HalfSectionDecel();

				break;

			//----右90回転----
			case 2:
				StartTimer();

				for(i=0;i<16;i++){
					SpinR90();
				}

				break;

			//----左90回転----
			case 3:
				StartTimer();
				for(i=0;i<16;i++){
					SpinL90();
				}
				break;

			//----180回転----
			case 4:
				StartTimer();
				for(i=0;i<16;i++){
					Spin180();
				}
				break;

			case 5:
				StartTimer();
				GyroInit();
				SetMotionDirection(FORWARD);
				HalfSectionAccel(GET_WALL_OFF);
				for(i=0;i<16;i++){
					SlalomR90();
					GoOneSectionContinuous(GET_WALL_OFF);
				}
				HalfSectionDecel();
				break;

			case 6:
				StartTimer();
				GyroInit();
				SetMotionDirection(FORWARD);
				HalfSectionAccel(GET_WALL_OFF);
				for(i=0;i<16;i++){
					SlalomL90();
					GoOneSectionContinuous(GET_WALL_OFF);
				}
				HalfSectionDecel();
				break;

			case 7:
				MF.FLAG.CTRL = 1;
				break;

			case 8:
				break;

			default:
				printf("GoodBy, Drive\r\n");

				return;
		}
		StopTimer();
		DisableMotor();
		printf("Drive Out\n");
	}
	printf("drive_Finish: %d\n",*mode);
}

