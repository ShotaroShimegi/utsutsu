#include <Mouse/global.h>

/*===========================================================
		探索系関数
===========================================================*/
/*-----------------------------------------------------------
		足立法探索走行α(一区画走行)
-----------------------------------------------------------*/
void SearchOneSection(uint8_t goal_length)
{
	InitializeMap();
	GyroInit();

	//====歩数等初期化====
	m_step = r_cnt = 0;									//歩数と経路カウンタの初期化
	GetWallData();
	WriteMap();											//地図の初期化
	MakeStepMap(goal_length);								//歩数図の初期化
	MakeRoute_NESW();									//最短経路探索(route配列に動作が格納される)

	SetMotionDirection(FORWARD);
	StartTimer();

	//====探索走行====
	do{

		//----進行----
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める

			case STRAIGHT:
				SetMotionDirection(FORWARD);
				break;

			case TURN_RIGHT:
				SpinR90();
				UpdateDirection(DIR_SPIN_R90);				//マイクロマウス内部位置情報でも右回転処理
				HAL_Delay(100);								//安定するまで待機
				SetMotionDirection(FORWARD);
				break;

			case TURN_BACK:
				Spin180();
				UpdateDirection(DIR_SPIN_180);				//マイクロマウス内部位置情報でも180度回転処理
				HAL_Delay(100);
				SetMotionDirection(FORWARD);
				break;

			case TURN_LEFT:
				SpinL90();
				UpdateDirection(DIR_SPIN_L90);				//マイクロマウス内部位置情報でも左回転処理
				HAL_Delay(100);								//安定するまで待機
				SetMotionDirection(FORWARD);
				break;

		}

		GoOneSectionStop();
		UpdatePosition();									//マイクロマウス内部位置情報でも前進処理
		ConfRoute_NESW(goal_length);						//最短経路で進行可能か判定

//	}while((MOUSE.X != goal_x) || (MOUSE.Y != goal_y));
	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length));

	WaitMs(200);											//スタートでは***2秒以上***停止しなくてはならない
	MelodyGoal();
	Spin180();												//180度回転
	UpdateDirection(DIR_SPIN_180);							//マイクロマウス内部位置情報でも180度回転処理
	StopTimer();
	DisableMotor();
}

void SearchContinuous(uint8_t goal_length)
{
	uint8_t fix_flag = 0;

	if(MF.FLAG.SEARCH)	InitializeMap();
	if(MF.FLAG.SCND)	LoadMapFromEeprom();

	GyroInit();

	//====歩数等初期化====
	m_step = r_cnt = 0;									//歩数と経路カウンタの初期化
	GetWallData();
	WriteMap();											//地図の初期化
	MakeStepMap(goal_length);								//歩数図の初期化
	MakeRoute_NESW();									//最短経路探索(route配列に動作が格納される)

	SetMotionDirection(FORWARD);
	StartTimer();

	if(wall_ff.val > WALL_TURN_VALUE ){
		Spin180();
		HAL_Delay(100);
		UpdateDirection(DIR_SPIN_180);
		if(wall_ff.val > wall_ff.threshold){
			FixPosition(0);
		}

	}
	if(goal_x == 0 && goal_y == 0){

	}else{
		DriveAccel(SET_MM,0);
	}
	SetMotionDirection(FORWARD);
	HalfSectionAccel(GET_WALL_ON);;
	UpdatePosition();
	r_cnt++;
	ConfRoute_NESW(goal_length);

	//====探索走行====
	do{
		switch(route[r_cnt++]){

			case STRAIGHT:
				GoOneSectionContinuous();
				break;

			case TURN_RIGHT:
				HalfSectionDecel();
				SpinR90();
				HAL_Delay(100);

				UpdateDirection(DIR_SPIN_R90);
				SetMotionDirection(FORWARD);
				HalfSectionAccel(GET_WALL_ON);;
				break;

			case TURN_BACK:
				HalfSectionDecel();
				if(wall_ff.val > WALL_TURN_VALUE){
					fix_flag = 1;
				}
				Spin180();
//				HAL_Delay(100);
				UpdateDirection(DIR_SPIN_180);
				SetMotionDirection(FORWARD);

				if(fix_flag == 1){
					FixPosition(0);
					fix_flag = 0;
				}
				DriveAccel(HALF_MM,0);
				GetWallData();
				break;

			case TURN_LEFT:
				HalfSectionDecel();

				SpinL90();
				HAL_Delay(100);

				UpdateDirection(DIR_SPIN_L90);
				SetMotionDirection(FORWARD);

				HalfSectionAccel(GET_WALL_ON);;
				break;
		}
		UpdatePosition();
		ConfRoute_NESW(goal_length);
	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length));

	HalfSectionDecel();
	StopTimer();
	WaitMs(100);
	if(MF.FLAG.SCND == 0)   SaveMapInEeprom();

	StartTimer();
	WaitMs(100);
	MelodyGoal();
}

void SearchSlalom(uint8_t goal_length)
{
	uint8_t fix_flag = 0;
//	uint8_t buff = 0x00;

	if(MF.FLAG.SEARCH == 1)	InitializeMap();

	GyroInit();

	//====歩数等初期化====
	m_step = r_cnt = 0;									//歩数と経路カウンタの初期化
	GetWallData();
	WriteMap();											//地図の初期化
	MakeStepMap(goal_length);							//歩数図の初期化
	MakeRoute_NESW();									//最短経路探索(route配列に動作が格納される)

	SetMotionDirection(FORWARD);
	StartTimer();

	if(wall_ff.val > WALL_TURN_VALUE){
		Spin180();
		HAL_Delay(100);
		UpdateDirection(DIR_SPIN_180);
		if(wall_ff.val > wall_ff.threshold){
			FixPosition(0);
		}
	}

	utsutsu_time = 0;
	MF.FLAG.CTRL = 0;

	if(goal_x == 0 && goal_y == 0){

	}else{
		DriveAccel(SET_MM,0);
	}
	SetMotionDirection(FORWARD);
	HalfSectionAccel(GET_WALL_ON);
	UpdatePosition();
	r_cnt++;
	ConfRoute_NESW(goal_length);

	//====探索走行====
	do{
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			case STRAIGHT:
//				buff = 0x00;
//				LedDisplay(&buff);

				GoOneSectionContinuous();                                //このプログラムには無い関数、他のプログラムと比べて類似の関数を探してピッタリなのを作れ　標
				break;

			case TURN_RIGHT:
//				buff = 0x10;
//				LedDisplay(&buff);

				SlalomR90();
				UpdateDirection(DIR_SPIN_R90);
				SetMotionDirection(FORWARD);
				break;

			case TURN_BACK:
//			buff = 0x11;
//				LedDisplay(&buff);

				if(wall_ff.val > WALL_TURN_VALUE){
					fix_flag = 1;
				}
				HalfSectionDecel();

				Spin180();
				HAL_Delay(100);
				UpdateDirection(DIR_SPIN_180);

				if(fix_flag == 1){
					FixPosition(0);
					fix_flag = 0;
				}
				SetMotionDirection(FORWARD);
				HalfSectionAccel(GET_WALL_ON);
				break;

			case TURN_LEFT:
//				buff = 0x01;
//				LedDisplay(&buff);

				SlalomL90();
				UpdateDirection(DIR_SPIN_L90);
				SetMotionDirection(FORWARD);

				break;
		}

		UpdatePosition();
		ConfRoute_NESW(goal_length);

	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length) != GOAL_OK);

	HalfSectionDecel();
	StopTimer();
	WaitMs(100);
	if(MF.FLAG.SCND == 0)  SaveMapInEeprom();
	WaitMs(100);
	MelodyGoal();
	StartTimer();
}

uint8_t CheckGoal(uint8_t x, uint8_t y, uint8_t length)
{
	uint8_t goal_check_x = GOAL_FAIL;
	uint8_t goal_check_y = GOAL_FAIL;
	uint8_t i = 0;
	uint8_t x_flag = 0;
	uint8_t y_flag = 0;
	uint8_t new_goal_x = 0;
	uint8_t new_goal_y = 0;

	for(i=0;i<length;i++){
		new_goal_x = goal_x + i;
		new_goal_y = goal_y + i;

		if((x == new_goal_x)&& (x_flag == 0)){
			goal_check_x = GOAL_OK;
			x_flag = 1;
		}else if(x_flag == 0){
			goal_check_x = GOAL_FAIL;
		}
		if((y == new_goal_y) && (y_flag == 0)){
			goal_check_y = GOAL_OK;
			y_flag = 1;
		}else if(y_flag == 0){
			goal_check_y = GOAL_FAIL;
		}
	}

	if((goal_check_x == GOAL_OK) && (goal_check_y == GOAL_OK)) {
		MF.FLAG.SEARCH = 0;
		return GOAL_OK;
	}
	else{
		return GOAL_FAIL;
	}

}

void MakePass(void)
{
	uint16_t j = 0;
	uint16_t pass_count = 0;
	uint16_t route_count = 1;
//	uint16_t show_counter;
	uint16_t big_turn_flag = 0;
	uint16_t normal_turn_flag = 0;
	uint16_t return_flag = 0;
	int8_t half_straight_count = 0;				//First Action is Always STRAIGHT

	for(pass_count=0;pass_count<1024;pass_count++){
		pass[pass_count] = 100;
	}
	half_straight_count = 1;
	pass_count = 0;

	j = route_count;
	while(route[j] == STRAIGHT){
		half_straight_count += 2;
		j++;
	}
	pass[pass_count] = half_straight_count;
	pass_count++;

	half_straight_count = 0;
	if((j-1) > 0)	route_count = j-1;			//Next is Straight
	else			route_count = j;

	normal_turn_flag = 1;

	if(route[route_count+1] == 0xff)	return_flag = 1;

	while(1){

/*		printf("route_count : %d\n",route_count);
		printf("half_straight_count : %d\n",half_straight_count);

		for(show_counter=0;show_counter<255;show_counter++){
			if(route[show_counter] == 0xff) break;
			printf("route[%d] = %#x\n",show_counter,route[show_counter]);
		}
		for(show_counter=0;show_counter<1024;show_counter++){
				if(pass[show_counter] == 100) break;
				printf("pass[%d] = %d\n",show_counter,pass[show_counter]);
		}
		printf("\n");
*/
		switch(route[route_count]){
			case STRAIGHT:

				if((route[route_count+1] == TURN_RIGHT) && (route[route_count+2] == STRAIGHT)){

					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}

					pass[pass_count] = BIG_R90;
					pass_count++;
					route_count += 2;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_RIGHT) && (route[route_count+2] == TURN_RIGHT) && (route[route_count+3] == STRAIGHT)){

					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}

					pass[pass_count] = BIG_R180;

					pass_count++;
					route_count += 3;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_LEFT) && (route[route_count+2] == STRAIGHT)){
					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}
					pass[pass_count] = BIG_L90;
					pass_count++;
					route_count += 2;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if((route[route_count+1] == TURN_LEFT) && (route[route_count+2] == TURN_LEFT) && (route[route_count+3] == STRAIGHT)){
					if((pass_count > 0) && (pass[pass_count-1] > 0))	pass[pass_count-1] -= 1;
					else if(pass[pass_count-1] == -1 || pass[pass_count-1] == -2){
						pass[pass_count] = 1;
						pass_count++;
					}
					pass[pass_count] = BIG_L180;
					pass_count++;
					route_count += 3;
					big_turn_flag = 1;
					normal_turn_flag = 0;
					break;
				}else if(normal_turn_flag == 1){
					route_count++;
					normal_turn_flag = 0;
					break;
				}

				j = route_count;
				while(route[j] == STRAIGHT){
					if(big_turn_flag == 1){
						half_straight_count  = 1;
						big_turn_flag = 0;
					}else{
						half_straight_count += 2;
					}
					j++;
				}
				pass[pass_count] = half_straight_count;
				pass_count++;

				half_straight_count = 0;
				route_count = j - 1;
				normal_turn_flag = 1;

				if(route[route_count+1] == 0xff)	return_flag = 1;

				break;

			case TURN_RIGHT:
				normal_turn_flag = 0;
				half_straight_count = 0;
				pass[pass_count] = R90;
				pass_count++;
				route_count++;
				break;

			case TURN_LEFT:
				normal_turn_flag = 0;
				half_straight_count = 0;
				pass[pass_count] = L90;
				pass_count++;
				route_count++;
				break;

			default:
				pass_end_count = pass_count;
				printf("pass_end_count : %d\n",pass_end_count);
				return;
				break;
		}
		if(return_flag == 1)	break;
	}
	pass_end_count = pass_count;
	printf("pass_end_count : %d\n",pass_end_count);
	return;
}

void ReadPass(void)
{
	uint16_t i = 0;
	uint8_t buff;
	utsutsu_time = 0;

	MF.FLAG.CTRL = 1;
	DriveAccel(SET_MM,FRONT_CONTROL_FLAG);

	while(i != pass_end_count){
		if(pass[i] > 0)	{
			buff = 0x00;
			LedDisplay(&buff);
			MF.FLAG.CTRL = 1;
			DriveTrapezoid(HALF_MM *(float)pass[i],params_now.big_vel_max,params_now.vel_max,params_now.accel);
		}else{
			switch(pass[i]){
				case R90:
					buff = 0x10;
					LedDisplay(&buff);
					SlalomR90();
					break;
				case L90:
					buff = 0x01;
					LedDisplay(&buff);
					SlalomL90();
					break;
				case BIG_R90:
					buff = 0x08;
					LedDisplay(&buff);

					BigSlalomR90();
					break;
				case BIG_L90:
					buff = 0x02;
					LedDisplay(&buff);

					BigSlalomL90();
					break;
				case BIG_R180:
					buff = 0x18;
					LedDisplay(&buff);
					BigSlalomR180();
					break;
				case BIG_L180:
					buff = 0x03;
					LedDisplay(&buff);

					BigSlalomL180();
					break;
				default:
					FailSafe();
					break;
			}
		}
		i++;
	}

	HalfSectionDecel();
	printf("PASS END\n");
	DisableMotor();
	StopTimer();

	MelodyGoal();

}

void ShowPass(void){
	uint16_t i;

	for(i=0;i<256;i++){
		if(route[i] == 0xff) break;
		printf("route[%d] : %x\n",i,route[i]);
	}
	printf("\n");

	for(i=0;i<1024;i++){
		if(pass[i] == 100) break;
		printf("pass[%d] : %d\n",i,pass[i]);
	}
	printf("End Print\n");
}
