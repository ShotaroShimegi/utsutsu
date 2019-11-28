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
	HalfSectionAccel(GET_WALL_OFF);
	DriveAccel(SET_MM+ONE_MM);
	utsutsu_time = 0;
	BigSlalomR90();
	HalfSectionDecel();

*/	StopTimer();
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
			SearchOneSection(RETURN_GOAL_LENGTH);

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
			SearchContinuous(RETURN_GOAL_LENGTH);
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();
			StopTimer();
			UpdateDirection(DIR_SPIN_180);
			break;

	  	  case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

	  		MakeStepMap(GOAL_LENGTH);
	  		MakeRoute_NESW();

	  		MakePass();
	  		ShowPass();

	  		break;

	  	  case 4:
	  		MelodyKurenai();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			StartWaiting();
			FirstAction();

			SearchSlalom(GOAL_LENGTH);
			goal_x = 0;
			goal_y = 0;
			SearchSlalom(RETURN_GOAL_LENGTH);
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			Spin180();									//180度回転
			UpdateDirection(DIR_SPIN_180);
			StopTimer();

			break;
			//////////////////////////////////

		case 5:
			MelodyNatsumatsuri();

			//***Make Parameter***//
			//----	CalculateNormalParams(&params_Structure,big_velocity,velocity,accel)
			CalculateNormalParams(&params_short1,1.0f,0.40f,4.0f);
			CalculateBigParams(&params_short1, 0.40f, 4.0f);
			//----  AssignOffsetParams(&params_search1, turn90_before, turn90_after, big90_before, big90_after, big180_before, big180_after);
			ApplyOffsetParams(&params_short1,30,50,47,68,23,70);
			//---- ApplyGain(gain,vel_kp,vel_ki,omega_kp,omega_ki,wall_kp,wall_kd, angle_kp,angle_kd)
			ApplyGain(&gain_short1,3.5f,0.01f,0.06f,0.002f,0.003f,0.00f,0.005f,0.0f);

			SetParams(&params_short1);
			SetGain(&gain_short1);

			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			MakeStepMap(GOAL_LENGTH);						//歩数図の初期化
			MakeRoute_NESW();
			MakePass();
//			ShowPass();

			StartWaiting();
			FirstAction();
			GyroInit();
			SetMotionDirection(FORWARD);
			StartTimer();
			ReadPass();
			break;

		case 6:
			MelodyYamato();
			//***Make Parameter***//
			//----	CalculateNormalParams(&params_Structure,big_velocity,velocity,accel)
			CalculateNormalParams(&params_short2,2.0f,0.80f, 4.0f);
			CalculateBigParams(&params_short2, 0.80f, 4.0f);
			//----  AssignOffsetParams(&params_search1, turn90_before, turn90_after, big90_before, big90_after, big180_before, big180_after);
			ApplyOffsetParams(&params_short2,25,40,43,60,30,90);
			//---- ApplyGain(gain,vel_kp,vel_ki,omega_kp,omega_ki,wall_kp,wall_kd, angle_kp,angle_kd)
			ApplyGain(&gain_short2,3.5f,0.01f,0.06f,0.002f,0.001f,0.00f,0.005f,0.0f);

			SetParams(&params_short2);
			SetGain(&gain_short2);

			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			MakeStepMap(GOAL_LENGTH);						//歩数図の初期化
			MakeRoute_NESW();
			MakePass();

			StartWaiting();
			FirstAction();
			GyroInit();
			SetMotionDirection(FORWARD);
			StartTimer();

			ReadPass();

			break;

		case 7:
			break;


		case 10:
			printf("Wall Data Output\n");
			ConvertMapIntoWall();
			PrintWallData();
			printf("Output Finish\n");
			break;

		case 11:
			MelodyRayearth();

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
