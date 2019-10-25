#include <Mouse/global.h>
/*============================================================
		初期化関数
============================================================*/
// Variable Initializing
void VariableInit(void){

	float val1 = 0;
	uint16_t i;
	//----壁センサ系----
	tp = 0;
	wall_l.dif = wall_r.dif = wall_fl.dif = wall_fr.dif = wall_ff.dif = 0;
	wall_l.val = wall_r.val = wall_fl.val = wall_fr.val = wall_ff.val = 0;
	wall_l.base = wall_r.base = wall_fl.base = wall_fr.base = wall_ff.base = 0;
	wall_l.threshold = wall_r.threshold = wall_fl.threshold = wall_fr.threshold = wall_ff.threshold = 0;
	utsutsu_time = 0;

	/*** Initialize Encoder Structure ***/
	encoder_r.pulse = 0;
	encoder_r.dif = 0;
	encoder_r.sum = 0;
	encoder_r.distance = 0;
	encoder_r.velocity = 0.0;

	encoder_l.pulse = 0;
	encoder_l.dif = 0;
	encoder_l.sum = 0;
	encoder_l.distance = 0;
	encoder_l.velocity = 0.0;

	/*** Initialize Velocity-Control Structure ***/
	vel_ctrl_R.dif = 0;
	vel_ctrl_R.p_out = 0;
	vel_ctrl_R.i_out = 0;
	vel_ctrl_R.dir = 1;
	vel_ctrl_R.out = 0;

	vel_ctrl_L.dif = 0;
	vel_ctrl_L.p_out = 0;
	vel_ctrl_L.i_out = 0;
	vel_ctrl_L.dir = 1;
	vel_ctrl_L.out = 0;

	/*** Initialize Omega-Control Structure ***/
	omega_control.dif = 0;
	omega_control.p_out = 0;
	omega_control.i_out = 0;
	omega_control.dir = 0;
	omega_control.out = 0;
	gyro_base = 0;

	//parameter 設定
	params_search1.vel_max = 0.40f;						//Unit is [m/s] = [mm/ms]
	params_search1.accel = 4.0f;						//Unit is [m/s/s]

	val1 = HALF_MM / params_search1.vel_max * 0.001; 			//Time of Running 90mm
	params_search1.omega_max = 1.5f * Pi * 0.5f / val1;				//Max Angular-Velocity [rad/s]
	params_search1.omega_accel = 3 * params_search1.omega_max / val1; 	//Angular Acceleration [rad/s/s]

	params_search1.R90_before = 35;		//35
	params_search1.R90_after = 53;
	params_search1.L90_before = 35;
	params_search1.L90_after = 53;

//	params_search1.omega_max = 6.0f;			//Unit is [rad/s]
//	params_search1.omega_accel = 25.0f;		//Unit is [rad/s/s]

	/*** Set Parameter for Search　***/
	gain_search1.vel_kpR = 3.5f;	//3.5f
	gain_search1.vel_kpL = 3.5f;	//3.5f
	gain_search1.vel_kiR = 0.01f;	//0.01f
	gain_search1.vel_kiL = 0.01f;
	gain_search1.omega_kp = 0.06f;	//0.2f
	gain_search1.omega_ki = 0.002f;	//0.01f
	gain_search1.wall_kp = 0.001f;	//0.001f
	gain_search1.wall_kd = 0.00f;
	gain_search1.angle_kp = 0.005f;
	gain_search1.angle_kd = 0.0f;

	SetParams(&params_search1);
	SetGain(&gain_search1);

	/*** Initialize Center Structure ***/
	center.velocity = 0;
	center.velocity_dir = 0;
	center.vel_target = 0;
	center.omega_deg = 0;
	center.omega_target = 0;
	center.pre_omega_deg = 0;
	center.omega_rad = 0;
	center.omega_dir = 0;
	center.distance = 0;
	center.angle = 0;
	center.pre_angle = 0.0f;
	center.angle_target = 0.0f;

	maxindex_w = val1 / 3;					// Time for Rotate

	/* 並進速度，計算処理  */
	maxindex = params_now.vel_max / params_now.accel;	//Time for Accel

	//----Eraze MF FLAGS----
	MF.FLAGS = 0x00;

	//----探索系----
	goal_x = GOAL_X;
	goal_y = GOAL_Y;
	InitializeMap();
	MOUSE.X = 0;
	MOUSE.Y = 0;
	MOUSE.DIR = 0;

	//Gain for Convert AD Value to Voltage
	Kvolt = MASS / 2 * DIA_SPUR_mm / DIA_PINI_mm * RADIUS_WHEEL_mm / Ktolk * Rmotor;
	//Gain for Convert Encoder Pulse to Physical Unit
	Kxr =  RADIUS_WHEEL_mm * DIA_PINI_mm / DIA_SPUR_mm * 2 * Pi / 4096;

	printf("Array Delete Start\n");

	//---Initialize Log Array---
	for(i=0;i<MEMORY;i++){
		test1[i] = 0;
		test2[i] = 0;
		test3[i] = 0;
		test4[i] = 0;
//		printf("%d\n",i);
//		HAL_Delay(1);
	}
	printf("Array Delete Completed\n");


	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);


}



