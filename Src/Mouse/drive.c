#include <Mouse/global.h>
#include "tim.h"

/*==========================================================
		走行系関数
==========================================================*/
/*
		基本仕様として、
		基幹関数		第一arg:走行パルス数
					第二arg:停止許可フラグ

		マウスフラグ(MF)
			7Bit:ストップフラグ
			6Bit:定速フラグ
			5Bit:減速フラグ
			4Bit:加速フラグ
			3Bit:制御フラグ
			2Bit:スラロームフラグ
			1Bit:二次走行フラグ
			0Bit:予約ビット
		上位4Bitは、上位ほど優先度が高い
*/
/*----------------------------------------------------------
		上位関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA
//	半区画分加速しながら走行する
// arg ： なし
// return ： なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA()
{
	MF.FLAG.CTRL = 1;
	DriveAccel(HALF_MM);
	GetWallData();
}

void half_sectionA2()
{
	MF.FLAG.CTRL = 1;										//制御を有効にする
	DriveAccel(HALF_MM);									//半区画のパルス分加速しながら走行。走行後は停止しない
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
//	半区画分減速しながら走行し停止する
// arg ： なし
// return ： なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD()
{
	MF.FLAG.CTRL = 0;
	DriveDecel(HALF_MM,1);									//
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//a_section
//	1区画分進んで停止する
// arg：なし
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void a_section()
{

 	half_sectionA();			//半区画分加速走行
	half_sectionD();

/*	DriveAccel(HALF_MM);
	DriveDecel(HALF_MM,1);
*/
	ResetDistance();
	DisableMotor();
}

void s_section(){
	half_sectionA2();			//半区画分加速走行
	half_sectionA();

}

void SpinR90()
{
	MF.FLAG.CTRL = 0;

	SetMotionDirection(TURN_R);								//右に回転するようモータの回転方向を設定
	DriveSpin(ROT_ANGLE_R90);
	DisableMotor();

	SetMotionDirection(FORWARD);								//前進するようにモータの回転方向を設定
}

void SlalomR90()
{
	MF.FLAG.CTRL = 0;
	SetMotionDirection(FORWARD);								//右に回転するようモータの回転方向を設定
	DriveAccel(params_search1.R90_before);

	time = 0;
	time2 = 0;
	MF.FLAG.CTRL = 0;
	driveW(-90);								//低速で指定パルス分回転。回転後に停止する

	MF.FLAG.CTRL = 0;
	DriveAccel(params_search1.R90_after);

	GetWallData();

}


void SpinL90(void)
{
	MF.FLAG.CTRL = 0;

	SetMotionDirection(TURN_L);									//左に超信地旋回する向きに設定
	DriveSpin(ROT_ANGLE_L90);									//超信地面するわよぉ！
	DisableMotor();
	SetMotionDirection(FORWARD);									//前進するようにモータの回転方向を設定
}

void SlalomL90(void)
{
	//time2 = 0;
	MF.FLAG.CTRL = 0;

	SetMotionDirection(FORWARD);
	DriveAccel(params_search1.L90_before);							//offset　before区間走行

	MF.FLAG.CTRL = 0;
	driveW(90);								//90までスラローム旋回

	MF.FLAG.CTRL = 0;
	DriveAccel(params_search1.L90_after);							//offset　after区間
	omega.p_out= 0;
	omega.i_out = 0;

	GetWallData();

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//Spin180
//	180度回転する
// arg：なし
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void Spin180()
{
	MF.FLAG.CTRL = 0;										//制御を無効にする

	SetMotionDirection(TURN_R);
	DriveSpin(-180);
	DisableMotor();

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
//	機体の尻を壁に当てて場所を区画中央に合わせる
// arg：なし
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position(uint8_t flag)
{
	MF.FLAG.CTRL = 0;

	//制御を無効にする
	SetMotionDirection(BACK);											//後退するようモータの回転方向を設定
	WaitMs(200);
	driveC(500,0);								//尻を当てる程度に後退。回転後に停止する
//	DriveAccel(SET_MM * 0.5);
//	DriveDecel(SET_MM * 0.5,1);
	SetMotionDirection(FORWARD);										//前進するようにモータの回転方向を設定

	MF.FLAG.CTRL =1;
	if(flag == 0){			//スラローム
		DriveAccel(SET_MM);
	}else{
		DriveAccel(SET_MM * 0.5);
		DriveDecel(SET_MM * 0.5,1);

	}

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
//	指定距離分加速走行する
// arg1：dist・・・走行する距離mm
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
//DC用に改造しているdriveA,距離はオッケー
void DriveAccel(float dist) {					//arg　走行距離　停止の有無（1で停止，０で継続走行）,vel0とtimeは触れていない

	float ics = center.distance;
//	uint16_t flag = 0;
	//====走行====
	//----走行開始----
	//MF.FLAGS = 0x00 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	MF.FLAG.ACTRL = 0;
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	MF.FLAG.FFCTRL = 0;

	//走行距離をリセット
	center.omega_target = 0;

	StartMotion();					//走行開始

	//----走行----
	while(center.distance < ics + dist){
//		printf("%lf : %lf : %lf : %lf :\n",center.distance, center.vel_target, center.velocity, out_duty_r);
/*		if(MF.FLAG.WALL && flag == 0){
			encoder_r.distance = (dist + ics - 60) / Kxr;
			encoder_l.distance = (dist + ics - 60) / Kxr;
			MF.FLAG.WALL = 0;
			flag = 1;
		}
*/	}

//	printf("Accel Finish\n");

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
//	指定パルス分減速走行して停止する
// arg1：dist・・・走行する距離
// arg2：rs・・・走行後停止するか　1:する　それ以外:しない
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++


void DriveDecel(uint16_t dist, unsigned char rs) {
	float ics = center.distance;
	float offset;

	//====走行====
	MF.FLAG.CTRL = 0;

	//----走行開始----
	MF.FLAG.ACTRL = 0;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.VCTRL = 1;

	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;

	StartMotion();

	offset = rs * 0.5 * params_now.vel_max * maxindex * 1000;

	//----走行----
	while((center.distance + offset) < (dist + ics)){
//		printf("%lf : %lf : %lf : %lf :\n",center.distance, center.vel_target, center.velocity, out_duty_r);
	}

		if(rs){
			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 1;

			while(center.vel_target > 0.0f){
//				printf("%lf : %lf : %lf : %lf :\n",center.distance, center.vel_target, center.velocity, out_duty_r);
			}

			WaitMs(500);

			MF.FLAG.ACTRL = 0;
			MF.FLAG.WCTRL = 0;
			MF.FLAG.XCTRL = 0;
			MF.FLAG.VCTRL = 0;
		}
//	printf("Deaccel Finish\n");
	//----停止措置----
	StopMotion();											//走行終了、停止許可があれば停止
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega.i_out = 0;


}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveAD
//	指定パルス分加速(or等速)・減速走行して停止する
// arg1：dist・・・走行するパルス
// arg2：rs・・・走行後停止するか　1:する　それ以外:しない
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
//DC用に改造，目的は超信地のみ arg1:
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
	MF.FLAG.ACTRL = 0;
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;

	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;

	MF.FLAG.REVOL = 1;

	ResetDistance();

	offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;	//減速に必要な角度の絶対値計算
	center.vel_target = 0;
	center.omega_target = 0;
	omega.i_out = 0;

	StartMotion();

	if(theta > 0){
		//----走行----
		while(center.angle < ics + theta - offset);				//w-tグラフにおける速度増加部の面積　⇒　現在の回転角度

		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle < ics + theta);

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
	omega.i_out = 0;
	MF.FLAG.REVOL = 0;

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveX
//姿勢制御を加えた減速走行　止まるときに使いたい
// arg1：dist・・・走行する距離[mm]
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveX(uint16_t dist){
	//====走行====
	//----走行開始----
	MF.FLAG.ACTRL = 1;
	MF.FLAG.VCTRL = 0;
	MF.FLAG.WCTRL = 0;
	MF.FLAG.XCTRL = 1;

	MF.FLAG.CTRL = 0;

	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;

	ResetDistance();		//距離を初期化
	center.angle = 0;

	StartMotion();								//走行開始

	WaitMs(100);
	//----走行----

	//----停止許可があれば停止----
	StopMotion();
	center.angle = 0;
	center.vel_target = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
//	指定時間分加速走行する
// arg1： count	走行時間
// arg2： rs		走行後停止するか　1:する　それ以外:しない
// return：null
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t count, unsigned char rs)			//arg　時間　停止許可　＊時間制限でモータ回せる関数
{

	ResetDistance();
	center.vel_target = center.omega_target = 0;
	time = 0;

	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.WCTRL = 1;

	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.WDECL = 0;

	StartMotion();											//走行開始

	//====回転====
	while(time < count * 0.5);			//一定時間経過まで待機
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 1;
	while(time < count);

	if(rs){
		vel_ctrl_R.dir = vel_ctrl_L.dir = 0;
		WaitMs(100);			//速度がゼロに収束するまで待つ
	}
	//----停止許可があれば停止----
	StopMotion();											//走行終了、停止許可があれば停止

	center.vel_target = 0;

	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega.i_out = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveW
//	角速度制御走行
// arg1：theta・・・目標角度
// arg2：rs・・・走行後停止するか　1:する　それ以外:しない
// return：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveW(int16_t theta)			//arg　時間　停止許可
{
	float offset = 0;
	ResetDistance();

	center.angle = 0;
	center.omega_target = 0;

	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.WCTRL = 1;

	if(theta > 0){				//左旋回
		center.omega_dir = 1;
	}else if(theta < 0){			//右旋回
		center.omega_dir = -1;
	}
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;

	StartMotion();

	offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;
	//====回転====
	if(center.omega_dir == 1){				//左旋回
		while(center.angle + offset < theta);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		while(center.angle  < theta){
			if(center.omega_target == 0){
				break;
			}
		}
	} else if(center.omega_dir == -1){			//右旋回

		while(center.angle - offset > theta);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;

		while(center.angle > theta){
			if(center.omega_target == 0){
				break;
			}
		}
	}

	omega.dir = 0;
	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega.i_out = 0;

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
			MF.FLAG.L_DIR = 1;				//左を前進方向に設定  Realy Need?
			vel_ctrl_L.dir= 1;

			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

			break;
		//----逆回転----
		case 0x01:
			MF.FLAG.L_DIR = 0;				//左を後進方向に設定
			vel_ctrl_L.dir = -1;

			HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
			HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,RESET);

			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){									//4~7ビット目を取り出す
		case 0x00:											//0x00の場合
			MF.FLAG.R_DIR = 1;					//右を前進方向に設定
			vel_ctrl_R.dir = 1;

			HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
			HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);

			break;
		case 0x10:
			MF.FLAG.R_DIR = 0;					//右を後進方向に設定
			vel_ctrl_R.dir = -1;

			HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,SET);
			HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,RESET);

			break;
	}
}

void DisableMotor(){
	HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin,RESET);

	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

	HAL_GPIO_WritePin(MOTOR_R_DIR1_GPIO_Port, MOTOR_R_DIR1_Pin,RESET);
	HAL_GPIO_WritePin(MOTOR_R_DIR2_GPIO_Port, MOTOR_R_DIR2_Pin,SET);

	vel_ctrl_R.i_out = vel_ctrl_L.i_out = 0;
	omega.i_out = 0;


}

void DriveTest(uint8_t *mode){

	while(1){
		printf("test_drive:\n");						//UARTで送信
		ModeSelect(mode);								//モード選択をさせる
		WaitMs(50);
		StartWaiting();

		switch(*mode){										//モード番号により分岐
			//----位置の調整----
			case 0:

				SetMotionDirection(FORWARD);

				StartMotion();
				HAL_Delay(1000);
				StopMotion();
				HAL_Delay(1000);

				break;

			//----一区画定速走行----
			case 1:
				StartTimer();
				SetMotionDirection(FORWARD);
				time = 0;
				half_sectionA();
				half_sectionD();

				break;

			//----右90回転----
			case 2:
				StartTimer();

				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();
				SpinR90();

				break;

			//----左90回転----
			case 3:
				StartTimer();

				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();
				SpinL90();

				break;

			//----180回転----
			case 4:
				StartTimer();

				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();
				Spin180();


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

