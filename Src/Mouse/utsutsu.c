#include <Mouse/global.h>

void UtsutsuSystem(){
	uint8_t mode = 0;
	uint8_t i = 0;

	DisableMotor();

	MelodySummer();

//	StartWaiting();
//	StopTimer();

	GyroInit();
	VariableInit();
	CheckBattery();

/*	SetMotionDirection(BACK);
	sensor_start();
	time = 0;
	set_position(1);
//	a_section();
	StopTimer();
*/
	printf("----Start Utsutsu System----\n");

	while(1){

	  ModeSelect(&mode);

	  WaitMs(500);
	  //----mode action----
	  switch(mode)
	  {
	  	  //----log Transmit----
	  	  case 0:
			printf("START\r\n");
			for(i=0;i<200;i++){
				printf("%4lf, %4lf, %4lf\n",test1[i],test2[i],test3[i]);
				WaitMs(1);
			}
			printf("ALL\r\n");

			break;

		  //----Search Mode for One Section Running----
	  	  case 1:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			GetWallData();
			SearchOneSection();
/*			goal_x = goal_y = 0;
			WaitMs(100);
			searchA();
*/
			goal_x = GOAL_X;
			goal_y = GOAL_Y;						//ゴール座標設定
			break;

			//----Search Mode for Continuous Running----
	  	  case 2:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			SearchContinuous();
			goal_x = goal_y = 0;
			SearchContinuous();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();
			StopTimer();
			UpdateDirection(DIR_SPIN_180);
			break;

			/////////////////////////////////　　↓の二次探索走行とスラローム走行は未実装
			//----二次高速走行----
	  	  case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			searchSA_ESNW();
			goal_x = goal_y = 0;
			searchSA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);
			break;

			//----スラローム走行----
	  	  case 4:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			SearchSlalom();
			goal_x = goal_y = 0;
			SearchSlalom();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);

			break;
			//////////////////////////////////

		case 5:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			searchSLA_ESNW();
			goal_x = goal_y = 0;
			searchSLA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);
			break;
		case 6:
			SetMotionDirection(FORWARD);

/*			StartMotion();
			HAL_Delay(1000);
			StopMotion();
			HAL_Delay(1000);
*/
			SetMotionDirection(FORWARD);
			StartTimer();
			time = 0;
			half_sectionA();
			half_sectionD();
			StopTimer();

			break;

			//----走行テスト----
		case 11:
		    DriveTest(&mode);									//test_drive()はdrive.cに関数定義あり
			WaitMs(100);
			break;

			//----エンコーダテスト----
		case 12:
			StartWaiting();
			EncoderGyroTest();
			WaitMs(100);
			break;

		case 13:
			StartWaiting();

			/*Stone Mode*/
			MF.FLAG.ACTRL = 0;
			MF.FLAG.VCTRL = 1;
			MF.FLAG.WCTRL = 1;
			MF.FLAG.XCTRL = 0;

			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 0;
			MF.FLAG.WACCL = 0;
			MF.FLAG.WDECL = 0;

			center.vel_target = 0;
			center.omega_target = 0;
			SetMotionDirection(FORWARD);
			StartMotion();
			while(1){
				WaitMs(100);
			}
			break;

			//----センサ値, 差を確認----
		default:
			HAL_Delay(100);
			VariableInit();
//			StartWaiting();
			CheckSensor();
			WaitMs(100);
			break;
	  	  }
	  }
}
