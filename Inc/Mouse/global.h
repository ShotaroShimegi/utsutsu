#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include "stdio.h"

#include "auxiliary.h"
#include "drive.h"
#include "sensor.h"
#include "search.h"
#include "connect.h"
#include "init.h"

/*============================================================
 各種定数・変数宣言
 ============================================================*/
/*------------------------------------------------------------
 走行系
 ------------------------------------------------------------*/
//----About Run----
#define ROT_ANGLE_R90 -90			//Target Angle [degree]
#define	ROT_ANGLE_L90 90
#define ROT_ANGLE_180 180
#define SET_MM 62					//Distance for Set Positiopn 54

#define CENTER_TIME 350
#define ROT_TIME 355
#define ROT180_TIME 500

#define GYRO_FIX 16.4			//Gain for Convert Gyro Value to Physical Unit，ジャイロデータシート参照
#define KW 0.01744				//Pi/180　degree -> radian に変換する定数
#define KWP 57.471

//----DC走行関連----
#define HALF_MM 90
#define ONE_MM 180
#define STOP_OFF_MM 0

//----Constante Variable for Calculate ----
#define RADIUS_WHEEL_mm 12.25f
#define DIA_PINI_mm 4.0f
#define DIA_SPUR_mm 21.0f
#define Pi 3.1415926f
#define Ktolk 1.98f
#define Rmotor 1.07f
#define TREAD_mm 68.0f
#define MASS 0.1f
#define VOLT_BAT 7.4f

//---メロディ周波数---
#define c6 1046
#define c6h 1108
#define d6 1174
#define d6h 1244
#define e6 1318

#define f6 1397
#define f6h 1480
#define g6 1568
#define g6h 1661
#define a6 1760
#define a6h 1865
#define b6 1976

#define c7 2093
#define c7h 2217
#define d7 2349
#define d7h 2489
#define e7 2637

#define f7 2794
#define f7h 2960
#define g7 3136
#define g7h 3222
#define a7 3520
#define a7h 3729
#define b7 3951

/*------------------------------------------------------------
 センサ系
 ------------------------------------------------------------*/
//----Sensor Threshoud----
#define WALL_BASE_F 150
#define WALL_BASE_L 150
#define WALL_BASE_R 100
#define WALL_OFF 100

#define WALL_TURN_VALUE 120
#define WALL_OFFSET 30
#define WALL_START 300

//----Sensor Control for Threshoud----
#define SREF_MIN_L 100
#define SREF_MAX_L 4000
#define SREF_MIN_R 100
#define SREF_MAX_R 4000

/*------------------------------------------------------------
 探索系
 ------------------------------------------------------------*/

//----Goal Node----
#define GOAL_X 0
#define GOAL_Y 6
#define GOAL_LENGTH 1

#ifndef __MOUSE_FLAGS__
#define __MOUSE_FLAGS__
typedef union {
	uint16_t FLAGS;
	struct ms_flags {
		uint8_t SET :1;			//尻当てフラグ(B0)		(:1は1ビット分の意味、ビットフィールド)
		uint8_t SCND :1;		//二次走行フラグ(B1)
		uint8_t SEARCH :1;		//探索走行フラグ(B2)
		uint8_t CTRL :1;		//壁制御フラグ(B3)
		uint8_t ACCL :1;		//並進加速フラグ(B4)
		uint8_t DECL :1;		//並進減速フラグ(B5)
		uint8_t WACCL :1;		//回転加速フラグ(B6)
		uint8_t WDECL :1;		//回転減速フラグ(B7)
		uint8_t VCTRL :1;		//並進制御フラグ(B8)
		uint8_t WCTRL :1;		//回転制御フラグ(B9)
		uint8_t FAIL :1;		//フェイルセーフフラグ(B10)
		uint8_t ACTRL :1;		//予備フラグ(B11)
		uint8_t RESV12 :1;		//予備フラグ(B12)
		uint8_t RESV13 :1;		//予備フラグ(B13)
		uint8_t RESV14 :1;		//予備フラグ(B14)
		uint8_t RESV15 :1;		//予備フラグ(B15)
	} FLAG;
} mouse_flags;
#endif

#ifdef MAIN_C_
	volatile mouse_flags MF;
#else
extern volatile mouse_flags MF;
#endif

//----Mouse State Constructure----
#ifndef __MOUSE_STATE__
#define __MOUSE_STATE__
typedef struct {
	uint8_t X :8;
	uint8_t Y :8;
	uint8_t DIR :8;
} mouse_state;
#endif

#ifdef MAIN_C_
	volatile mouse_state MOUSE;
#else

extern volatile mouse_state MOUSE;
#endif

#endif /* GLOBAL_H_ */
