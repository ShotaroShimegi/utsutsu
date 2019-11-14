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
	DriveDecel(HALF_MM,1);
}

void GoOneSectionStop()
{
 	HalfSectionAccel(GET_WALL_ON);
	HalfSectionDecel();
	ResetDistance();
	DisableMotor();
}

void GoOneSectionContinuous()
{
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
	DriveAccel(params_search1.TURN90_before);
	MF.FLAG.CTRL = 0;
	DriveSlalom(-90);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_search1.TURN90_after);
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
	DriveAccel(params_now.TURN90_before);
	MF.FLAG.CTRL = 0;
	DriveSlalom(90);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.TURN90_after);
	GetWallData();

}

void BigSlalomR90()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_now.big90_before);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(-90,params_now.big90_omega_max, params_now.big90_omega_accel, big90_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_after);
	GetWallData();
}

void BigSlalomL90()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_now.big90_before);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(90,params_now.big90_omega_max, params_now.big90_omega_accel, big90_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big90_after);
	GetWallData();
}

void BigSlalomR180()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_now.big180_before);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(-180,params_now.big180_omega_max, params_now.big180_omega_accel, big180_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big180_after);
	GetWallData();
}

void BigSlalomL180()
{
	MF.FLAG.CTRL = 1;
	SetMotionDirection(FORWARD);
	DriveAccel(params_now.big180_before);
	MF.FLAG.CTRL = 0;
	DriveSlalomFree(180,params_now.big180_omega_max, params_now.big180_omega_accel, big180_omega_accel_time);
	MF.FLAG.CTRL = 1;
	DriveAccel(params_now.big180_after);
	GetWallData();
}

void GoStraightTrapezoid(uint16_t distance, float vel_max,float accel)
{
	MF.FLAG.CTRL = 1;

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
	MF.FLAG.CTRL = 0;
	SetMotionDirection(BACK);
	WaitMs(200);
	ResetDistance();

	center.velocity_max = 0.20f;
	DriveAccel(-(SET_MM * 0.7f));
	DriveDecel(-(SET_MM * 0.7f),1);
	center.velocity_max = params_now.vel_max;

	center.angle_target = 0.0f;
	center.angle = 0.0f;

	SetMotionDirection(FORWARD);
	MF.FLAG.CTRL = 1;
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

	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	if(dist > 0)	MF.FLAG.ACTRL = 0;
	else 			MF.FLAG.ACTRL = 0;

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

	if(dist > 0)MF.FLAG.ACTRL = 0;
	else MF.FLAG.ACTRL = 0;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	center.velocity_max = params_now.vel_max;
	center.velocity_min = 0.0f;

	offset = 0.5 * params_now.vel_max * accel_time * 1000;
	offset += 8.0f;

	StartMotion();

	//----走行----
	if(dist >= 0.0){
		while((center.distance + offset) < (dist + ics));
	}else{
		while((center.distance + offset) > (dist + ics));
	}
		if(rs){

			MF.FLAG.CTRL = 0;
			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 1;
			while(center.vel_target != 0.0f);
			WaitMs(500);
			MF.FLAG.WCTRL = 0;
			MF.FLAG.VCTRL = 0;

			StopMotion();
			vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
			omega_control.i_out = 0;
		}
}

void DriveSpin(float theta)
{
	float offset;
	float ics = center.angle;
	center.angle_target += theta;

	if(theta > 0){
		center.omega_dir = 1;
	}else if(theta < 0){
		center.omega_dir = -1;
	}

	//----Setting Mouse Flag----
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.ACTRL = 0;

	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;

	ResetDistance();

	offset = (0.5 * omega_accel_time * params_now.omega_max) * KWP;		//減速に必要な角度の絶対値計算
	center.vel_target = 0;
	center.omega_target = 0;
	omega_control.i_out = 0;

	StartMotion();

	if(theta > 0){
		//----Turn Left----
//		offset += 2.0f;
		while(center.angle < ics + theta - offset);				//w-tグラフにおける速度増加部の面積　⇒　現在の回転角度

		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle < ics + theta);

	}else if (theta < 0){
		//----Turn Right----
//		offset += 2.0f;
		while(center.angle > ics + theta + offset);

		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle > ics + theta);

	}

	center.omega_target = 0;
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;
	HAL_Delay(200);


	//----停止許可があれば停止----
	StopMotion();


}

void DriveSlalom(int16_t theta)
{
	float offset = 0;
	float offset_fix = 0;
	float ics = center.angle;
	ResetDistance();

//	center.angle = 0;
	center.omega_target = 0;

	//====Set MF FLAGS====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;

	if(theta > 0)		center.omega_dir = 1;	//Left Turn
	else if(theta < 0)	center.omega_dir = -1;	//Right Turn

	StartMotion();

	offset = (0.5 * omega_accel_time * center.omega_max) * KWP;

	if(center.omega_dir == 1)					//Left Turn
	{
		offset_fix = 0.5f;
		while(center.angle + offset < ics + theta - offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		while(center.angle < ics + theta - offset_fix){
			if(center.omega_target == 0) break;
		}
	}
	else if(center.omega_dir == -1)			//Right Turn
	{
		offset_fix = 0.2f;
		while(center.angle - offset > ics + theta + offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		while(center.angle > ics + theta + offset_fix)
		{
			if(center.omega_target == 0) break;
		}
	}
	center.angle_target += (float)theta;
	omega_control.dir = 0;
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;
}

void DriveSlalomFree(int16_t theta, float omega_max, float omega_accel,float omega_time)
{
	float offset = 0;
	float offset_fix = 0;
	float ics = center.angle;
	ResetDistance();

	//====Change Center Structure====
	center.omega_target = 0.0f;
	center.omega_accel = omega_accel;
	center.omega_max = omega_max;

	//====Set MF FLAG====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.ACTRL = 0;

	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;

	if(theta > 0)		center.omega_dir = 1;	//Left Turn
	else if(theta < 0)	center.omega_dir = -1;	//Right Turn

	StartMotion();
	offset = (0.5 * omega_time * omega_max) * KWP;

	if(center.omega_dir == 1){				//Left Turn
		while(center.angle + offset < ics + theta - offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		while(center.angle < ics + theta - offset_fix){
			if(center.omega_target == 0) break;
		}

	} else if(center.omega_dir == -1){		//Right Turn
		while(center.angle - offset > ics + theta + offset_fix);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		while(center.angle > ics + theta + offset_fix){
			if(center.omega_target == 0) break;
		}
	}

	center.angle_target += theta;
	omega_control.dir = 0;
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega_control.i_out = 0;
}

//---Not Completed Function
void DriveTrapezoid(uint16_t distance,float vel_max,float vel_min, float accel)
{
	float ics = center.distance;
	float vel_dif;
	float offset;

	//Change Max Speed
	center.velocity_max = vel_max;
	center.velocity_min = 0.0f;

	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	if(distance > 0)	MF.FLAG.ACTRL = 0;
	else 				MF.FLAG.ACTRL = 0;

	offset = 0.5f * accel_time * vel_min * 1000;
	center.omega_target = 0;

	StartMotion();
	//----Go Forward----
	if(distance >= 0.0f){
		while(center.distance + offset < ics + distance){
			if(center.vel_target ==  center.velocity_max){
				vel_dif = center.vel_target - vel_min;
				offset = 0.5f * 1000 * vel_dif * vel_dif / accel;
				offset += 1000 * vel_min * vel_dif / accel;
			}
		}
		center.velocity_min = vel_min;
		MF.FLAG.ACCL = 0;
		MF.FLAG.DECL = 1;

		while(center.distance < ics + distance){
			if(center.vel_target == center.velocity_min) break;
		}
	} else{
		while(center.distance > ics + distance);
	}


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

void SetMotionDirection(uint8_t d_dir)
{
	HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin,SET);
	//====Left Motor====
	switch(d_dir & 0x0f){
		//----FORWARD----
		case 0x00:
			vel_ctrl_L.dir= 1;
			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);
			break;
		//----BACK----
		case 0x01:
			vel_ctrl_L.dir = -1;
			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,RESET);
			break;
	}
	//====Right Motor====
	switch(d_dir & 0xf0){
	//----FORWARD----
		case 0x00:
			vel_ctrl_R.dir = 1;
			HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);
			break;
	//----BACK----
		case 0x10:
			vel_ctrl_R.dir = -1;
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

	MF.FLAGS = 0x0000;
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
				StartTimer();
				SetMotionDirection(FORWARD);
				utsutsu_time = 0;
				MF.FLAG.CTRL = 0;
				DriveTrapezoid(540,1.0f,0.5f,4.0f);
				DriveDecel(HALF_MM,1);
				break;

			case 8:
				break;

			default:
				printf("GoodBy, Drive\r\n");
				StopTimer();
				return;
		}
		StopTimer();
		WaitMs(3000);
		printf("Drive Out\n");
	}
	printf("drive_Finish: %d\n",*mode);
}

void PIDStructureInit(pid_control *instance)
{
	instance->dif = 0.0f;
	instance->dir = 0;
	instance->i_out = 0.0f;
	instance->p_out = 0.0f;
	instance->out = 0.0f;
}
