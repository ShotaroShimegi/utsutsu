#ifndef SEARCH_H_
	#define SEARCH_H_

#define STRAIGHT 0x88
#define TURN_RIGHT 0x44
#define TURN_BACK 0x22
#define TURN_LEFT 0x11

#define GOAL_OK 0
#define GOAL_FAIL 1

//----Pass Action Number----
#define R90 -1
#define L90 -2
#define BIG_R90 -3
#define BIG_L90 -4
#define BIG_R180 -5
#define BIG_L180 -6

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====Sernch Function====
	void SearchOneSection(uint8_t);
	void SearchContinuous(uint8_t);
	void SearchSlalom(uint8_t);

	uint8_t CheckGoal(uint8_t,uint8_t,uint8_t);
	void MakePass(void);
	void ReadPass(void);
	void ShowPass(void);

#endif /* SEARCH_H_ */
