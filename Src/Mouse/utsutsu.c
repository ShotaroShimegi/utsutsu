#include <Mouse/global.h>

void UtsutsuSystem()
{
	uint8_t mode = 0;
	uint16_t i = 0;

	DisableMotor();
	MelodySummer();
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
	utsutsu_time = 0;
	BigSlalomL180();
	HalfSectionDecel();
*/
	StopTimer();
	DisableMotor();

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
			SearchOneSection(GOAL_LENGTH);
			goal_x = goal_y = 0;
			WaitMs(100);
			SearchOneSection(1);

			goal_x = GOAL_X;
			goal_y = GOAL_Y;
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
			SearchSlalom(RETURN_GOAL_LENGTH);
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);
			StopTimer();

			break;
			//////////////////////////////////

		case 5:
			break;

		case 6:
			SetMotionDirection(FORWARD);
			StartTimer();
			utsutsu_time = 0;
			HalfSectionAccel(GET_WALL_ON);
			HalfSectionDecel();
			StopTimer();
			break;

		case 10:
			printf("Wall Data Output\n");
			ConvertMapIntoWall();
			PrintWallData();
			printf("Output Finish\n");
			break;

		case 11:
		    DriveTest(&mode);
			WaitMs(100);
			break;

		case 12:
			StartWaiting();
			GyroInit();
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

		case 14:
			MF.FLAG.SEARCH = 0;
			MF.FLAG.SCND = 1;
			LoadMapFromEeprom();
			MelodyUrara();
			break;

		default:
			HAL_Delay(100);
			VariableInit();
			CheckSensor();
			WaitMs(100);
			break;
	  	  }
	  }
}
