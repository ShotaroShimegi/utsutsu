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

		GoOneSectionStop();									//前進する
		UpdatePosition();									//マイクロマウス内部位置情報でも前進処理
		ConfRoute_NESW();									//最短経路で進行可能か判定

//	}while((MOUSE.X != goal_x) || (MOUSE.Y != goal_y));
	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length));
															//現在座標とgoal座標が等しくなるまで実行
	WaitMs(2000);											//スタートでは***2秒以上***停止しなくてはならない
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

	MelodyRayearth();

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
		DriveAccel(SET_MM);
	}
	SetMotionDirection(FORWARD);
	HalfSectionAccel(GET_WALL_ON);;
	UpdatePosition();
	r_cnt++;
	ConfRoute_NESW();

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
				DriveAccel(HALF_MM);
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
		ConfRoute_NESW();

//	}while((MOUSE.X != goal_x) || (MOUSE.Y != goal_y));
	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length));

	HalfSectionDecel();
	StopTimer();
	if(MF.FLAG.SCND == 0){
	   SaveMapInEeprom();
	}
	StartTimer();
	WaitMs(2000);
	MelodyGoal();

}

void SearchSlalom(uint8_t goal_length)
{
	uint8_t fix_flag = 0;

	if(MF.FLAG.SEARCH == 1)	InitializeMap();

	GyroInit();

	//====歩数等初期化====
	m_step = r_cnt = 0;									//歩数と経路カウンタの初期化
	GetWallData();
	WriteMap();											//地図の初期化
	MakeStepMap(goal_length);							//歩数図の初期化
	MakeRoute_NESW();									//最短経路探索(route配列に動作が格納される)

	MelodyRayearth();

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

	center.angle_target = 0.0f;
	utsutsu_time = 0;
	MF.FLAG.CTRL = 0;

	if(goal_x == 0 && goal_y == 0){

	}else{
		DriveAccel(SET_MM);
	}
	SetMotionDirection(FORWARD);
	HalfSectionAccel(GET_WALL_ON);;
	UpdatePosition();
	r_cnt++;
	ConfRoute_NESW();

	//====探索走行====
	do{
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			case STRAIGHT:
				GoOneSectionContinuous();                                //このプログラムには無い関数、他のプログラムと比べて類似の関数を探してピッタリなのを作れ　標
				break;

			case TURN_RIGHT:
				SlalomR90();
				UpdateDirection(DIR_SPIN_R90);
				SetMotionDirection(FORWARD);
				break;

			case TURN_BACK:
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
				HalfSectionAccel(GET_WALL_ON);;
				break;

			case TURN_LEFT:
				SlalomL90();
				UpdateDirection(DIR_SPIN_L90);
				SetMotionDirection(FORWARD);

				break;
		}

		UpdatePosition();
		ConfRoute_NESW();

	}while((MOUSE.X != goal_x) || (MOUSE.Y != goal_y));

//	}while(CheckGoal(MOUSE.X,MOUSE.Y,goal_length) != GOAL_OK);

	HalfSectionDecel();
	StopTimer();
	WaitMs(1000);
//	DisableMotor();
	if(MF.FLAG.SCND == 0){
	   SaveMapInEeprom();
	}
	WaitMs(1000);
	MelodyGoal();
	StartTimer();
}

uint8_t CheckGoal(uint8_t x, uint8_t y, uint8_t goal_length)
{
	uint8_t goal_check_x, goal_check_y;
	uint8_t i;
	uint8_t x_flag = 0;
	uint8_t y_flag = 0;

	for(i=0;i < goal_length;i++){
		if(x == goal_x + i && x_flag == 0){
			goal_check_x = GOAL_OK;
			x_flag = 1;
		}else if(x_flag == 0){
			goal_check_x = GOAL_FAIL;
		}
		if(y == goal_y + i && y_flag == 0){
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
	uint16_t i = 0;
	uint16_t half_straight_count = 0;

	while(1){
		switch(route[i]){
			case STRAIGHT:
				half_straight_count += 2;
				pass[i] = half_straight_count;

				if((route[i+1] == TURN_RIGHT) && (route[i+2] == STRAIGHT)){
					//BigTurnR90();
					i += 2;
				}else if((route[i+1] == TURN_RIGHT) && (route[i+2] == TURN_RIGHT) && (route[i+3] == STRAIGHT)){
					//BigTurnR180();
					i += 3;
				}else if((route[i+1] == TURN_LEFT) && (route[i+2] == STRAIGHT)){
					//BigTurnL90():
					i += 2;
				}else if((route[i+1] == TURN_LEFT) && (route[i+2] == TURN_LEFT) && (route[i+3] == STRAIGHT)){
					//BigTurnL180();
					i += 3;
				}else{

				}

				break;

			case TURN_RIGHT:
				half_straight_count = 0;
				i++;
				break;

			case TURN_LEFT:
				half_straight_count = 0;
				i++;
				break;

			default:
				break;
		}
	}


}

void ReadPass(void)
{


}

