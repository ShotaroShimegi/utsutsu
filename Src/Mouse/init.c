#include <Mouse/global.h>

void VariableInit(void)
{
	uint16_t i;

	tp = 0;
	WallStructureInit(&wall_fl);
	WallStructureInit(&wall_l);
	WallStructureInit(&wall_ff);
	WallStructureInit(&wall_r);
	WallStructureInit(&wall_fr);

	PIDStructureInit(&vel_ctrl_R);
	PIDStructureInit(&vel_ctrl_L);
	PIDStructureInit(&omega_control);

	CenterStructureInit(&center);
	EncoderStructureInit(&encoder_r);
	EncoderStructureInit(&encoder_l);

	//parameter 設定
//----	CalculateNormalParams(&params_Structure,velocity,accel)
	CalculateNormalParams(&params_search1, 0.50f, 4.0f);
	CalculateBigParams(&params_search1,0.50f,4.0f);

//	AssignOffsetParams(&params_search1,turn90_before,turn90_after,big90_before,big90_after,big180_before,big180_after);
	AssignOffsetParams(&params_search1,35,53,43,46,30,31);

/*	params_search1.TURN90_before = 35;
	params_search1.TURN90_after = 53;
	params_search1.big90_before = 43;
	params_search1.big90_after = 46;
	params_search1.big180_before = 30;
	params_search1.big180_after = 31;
*/

	/*** Set Parameter for Search　***/
	gain_search1.vel_kp = 3.5f;		//3.5f
	gain_search1.vel_ki = 0.01f;	//0.01f
	gain_search1.omega_kp = 0.06f;	//0.2f
	gain_search1.omega_ki = 0.002f;	//0.01f
	gain_search1.wall_kp = 0.006f;	//0.004f
	gain_search1.wall_kd = 0.00f;
	gain_search1.angle_kp = 0.005f;
	gain_search1.angle_kd = 0.0f;

	SetParams(&params_search1);
	SetGain(&gain_search1);

	//----Eraze MF FLAGS----
	MF.FLAGS = 0x00;
	goal_x = GOAL_X;
	goal_y = GOAL_Y;
	InitializeMap();
	MOUSE.X = 0;
	MOUSE.Y = 0;
	MOUSE.DIR = 0;
	utsutsu_time = 0;

	//Gain for Convert AD Value to Voltage
	Kvolt = MASS / 2 * DIA_SPUR_mm / DIA_PINI_mm * RADIUS_WHEEL_mm / Ktolk * Rmotor;
	//Gain for Convert Encoder Pulse to Physical Unit
	Kxr =  RADIUS_WHEEL_mm * DIA_PINI_mm / DIA_SPUR_mm * 2 * Pi / 4096;

	printf("Array Delete Start\n");
	for(i=0;i<MEMORY;i++){
		test1[i] = 0;
		test2[i] = 0;
		test3[i] = 0;
		test4[i] = 0;
	}
	printf("Array Delete Completed\n");

	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

}
