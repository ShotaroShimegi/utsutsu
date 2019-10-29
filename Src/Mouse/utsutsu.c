#include <Mouse/global.h>

void UtsutsuSystem()
{
	uint8_t mode = 0;
	uint16_t i = 0;

	DisableMotor();
//	MelodySummer();
	MelodyRayearth();
//	StartWaiting();

	GyroInit();
	VariableInit();
	CheckBattery();
	MF.FLAG.SEARCH = 1;

/*	StartTimer();
	FirstAction();
	SetMotionDirection(FORWARD);
	center.angle_target = 0.0f;
	utsutsu_time = 0;

	HalfSectionAccel(GET_WALL_OFF);
	SlalomL90();
	GoOneSectionContinuous();
	utsutsu_time = 0;

	SlalomL90();
	HalfSectionDecel();

*/	StopTimer();
	DisableMotor();

	printf("----Start Utsutsu System----\n");

	SaveMapInEeprom();
	LoadMapFromEeprom();

	for(i=0;i<16;i++){
		printf("MAP %d is 0x%x\n",i,map[i][16]);
		HAL_Delay(100);
	}

	while(1){

	  ModeSelect(&mode);

	  WaitMs(500);
	  //----mode action----
	  switch(mode)
	  {
	  	  //----log Transmit----
	  	  case 0:
			printf("START\r\n");
			for(i=0;i<MEMORY;i++){
				printf("%4lf, %4lf, %4lf, %4lf\n",test1[i],test2[i],test3[i],test4[i]);
			}
			printf("ALL\r\n");

			break;

		  //----Search Mode for One Section Running----
	  	  case 1:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

//			GetWallData();
			SearchOneSection(GOAL_LENGTH);
			goal_x = goal_y = 0;
			WaitMs(100);
			SearchOneSection(1);

			goal_x = GOAL_X;
			goal_y = GOAL_Y;						//ゴール座標設定
			break;

			//----Search Mode for Continuous Running----
	  	  case 2:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			SearchContinuous(GOAL_LENGTH);
			goal_x = goal_y = 0;
			SearchContinuous(1);
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();
			StopTimer();
			UpdateDirection(DIR_SPIN_180);
			break;

	  	  case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			goal_x = goal_y = 0;

			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);
			break;


	  	  case 4:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			SearchSlalom(GOAL_LENGTH);
			goal_x = goal_y = 0;
			SearchSlalom(1);
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);

			StopTimer();

			break;
			//////////////////////////////////

		case 5:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			goal_x = goal_y = 0;

			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();
			UpdateDirection(DIR_SPIN_180);
			break;
		case 6:
			SetMotionDirection(FORWARD);
			StartTimer();
			utsutsu_time = 0;
			HalfSectionAccel(GET_WALL_ON);
			HalfSectionDecel();
			StopTimer();

			break;

			//----走行テスト----
		case 11:
		    DriveTest(&mode);
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
			MF.FLAG.VCTRL = 1;
			MF.FLAG.WCTRL = 1;

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
