#include <Mouse/global.h>

void UtsutsuSystem(){
	uint8_t mode = 0;
	uint8_t i = 0;

	HAL_GPIO_WritePin(MOTOR_L_DIR1_GPIO_Port, MOTOR_L_DIR1_Pin,SET);
	HAL_GPIO_WritePin(MOTOR_L_DIR2_GPIO_Port, MOTOR_L_DIR2_Pin,SET);

	MelodySummer();

//	StartWaiting();
//	sensor_stop();

	GyroInit();
	VariableInit();
	CheckBattery();

/*	StartWaiting();
	EncoderGyroTest();
	ms_wait(100);
*/

/*	time = 0;
	turn_180();
	HAL_Delay(100);
	sensor_stop();
	MotorDisable();
*/

/*	SetMotionDirection(BACK);
	sensor_start();
	time = 0;
	set_position(1);
//	a_section();
	sensor_stop();
*/
	printf("----Start Utsutsu System----\n");

	while(1){

	  ModeSelect(&mode);

	  ms_wait(100);
	  //----mode action----
	  switch(mode){
	  	  case 0:	//
			//----log trans----
			ms_wait(500);
			printf("START\r\n");

//			printf("base:%d, %d\n", wall_l.threshold, wall_r.threshold);

			for(i=0;i<200;i++){
				printf("%4lf, %4lf, %4lf\n",test1[i],test2[i],test3[i]);
				ms_wait(1);
			}
			printf("ALL\r\n");
			break;

	  	  case 1:	//----一次探索走行----
			goal_x = GOAL_X;									//ゴール座標を設定　　GOAL_Xはglobal.hにマクロ定義あり
			goal_y = GOAL_Y;									//ゴール座標を設定　　GOAL_Yはglobal.hにマクロ定義あり

			StartWaiting();
			start_ready();

			get_wall_info();									//壁情報の初期化     get_wall_info()はsensor.cに関数定義あり
			searchA();										//ゴール区画まで探索，進行する　searchA()はsearch.cに関数定義あり
/*			goal_x = goal_y = 0;								//ゴール座標をスタート区画に設定
			ms_wait(100);										//待機
			searchA();											//戻ってくる
*/
			goal_x = GOAL_X;									//ゴール座標設定
			goal_y = GOAL_Y;									//ゴール座標設定
			break;

			//----連続探索走行----
	  	  case 2:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			start_ready();

			searchSA();
			goal_x = goal_y = 0;
			searchSA();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			sensor_stop();
			turn_dir(DIR_TURN_180);
			break;

			/////////////////////////////////　　↓の二次探索走行とスラローム走行は未実装
			//----二次高速走行----
	  	  case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			start_ready();

			searchSA_ESNW();
			goal_x = goal_y = 0;
			searchSA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);
			break;

			//----スラローム走行----
	  	  case 4:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			start_ready();

			searchSLA();
			goal_x = goal_y = 0;
			searchSLA();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);

			break;
			//////////////////////////////////

		case 5:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			start_ready();

			searchSLA_ESNW();
			goal_x = goal_y = 0;
			searchSLA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);
			break;
		case 6:
			SetMotionDirection(FORWARD);

/*			StartMotion();
			HAL_Delay(1000);
			StopMotion();
			HAL_Delay(1000);
*/
			SetMotionDirection(FORWARD);
			sensor_start();
			time = 0;
			half_sectionA();
			half_sectionD();
			sensor_stop();

			break;

			//----走行テスト----
		case 11:
			HAL_Delay(100);
		    DriveTest(&mode);									//test_drive()はdrive.cに関数定義あり
			ms_wait(100);
			break;

			//----エンコーダテスト----
		case 12:
			HAL_Delay(100);
			StartWaiting();
			EncoderGyroTest();
			ms_wait(100);
			break;

		case 13:
			HAL_Delay(100);
			StartWaiting();

			/*Mode Stone*/
			MF.FLAG.ACTRL = 0;
			MF.FLAG.VCTRL = 1;
			MF.FLAG.WCTRL = 1;
			MF.FLAG.XCTRL = 0;

			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 0;
			MF.FLAG.WACCL = 0;
			MF.FLAG.WDECL = 0;

			centor.vel_target = 0;
			omega.target = 0;

			StartMotion();
			while(1){
//				uart_printf("angle : %lf dif_angle : %lf tpid_G : %lf\r\n");
				ms_wait(100);
			}
			break;

			//----センサ値, 差を確認----
		default:
			HAL_Delay(100);
			VariableInit();
//			StartWaiting();
			sensor_check();
			ms_wait(100);
			break;
	  	  }

	  }
}
