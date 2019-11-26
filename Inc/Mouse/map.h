/*
 * map.h
 *
 *  Created on: 2019/11/11
 *      Author: 標 祥太郎
 */

#ifndef INC_MOUSE_MAP_H_
#define INC_MOUSE_MAP_H_


#define NORTH 0x00
#define EAST 0x01
#define SOUTH 0x02
#define WEST 0x03

/*============================================================
		各種定数･変数宣言
============================================================*/
	//迷路の絶対座標について、スタート地点が左下になるような位置から見たとき、
	//上方向を北、右方向を東、下方向を南、左方向を西として定義している。
	//m_dirの場合、北は0x00、東は0x01、南は0x02、西は0x03で表される。(turn_dir関数参照)
	//また、マップ格納配列map[][]について、要素は上位4nitと下位4bitに分かれていて、
	//北壁は3bit目、東壁は2bit目、南壁は1bit目、西壁は0bit目に、
	//壁がある場合は1、ない場合は0が格納されている。
	//下位4bitは1次走行用(未探索壁は無しと判定)で上位4bitは2次走行用(未探索壁は有りと判定)
	//を表している。(write_map関数およびmap_Init関数参照)
	//最後に、最短経路格納配列route[]について、進行順に移動が記録されている。
	//各要素に機体が前進する際は0x88が、右折する際は0x44が、Uターンする際は0x22が、左折する場合は0x11が、
	//それ以外の場合には0x00が格納される。(make_route関数参照)。なお、進行の経過はr_cntで管理されている。

	//====Variavle Initialize====
	#ifdef MAIN_C_
		/*グローバル変数の定義*/
		uint8_t map[16][16];								// wall_map
		uint8_t step_map[16][16];							//歩数マップ格納配列
		uint8_t wall_info;									//壁情報格納変数
		uint8_t m_step;										//歩数格納
		uint16_t goal_x, goal_y;							//ゴール座標
		uint8_t route[256];									//最短経路格納配列
		uint8_t r_cnt;										//経路カウンタ

		//Challenge Wall Data
		uint16_t wall_vertical[16];
		uint16_t wall_horizontal[16];

		int8_t pass[1024];
		uint16_t pass_end_count;

	#else
		extern uint8_t map[16][16];							//マップ格納配列
		extern uint8_t step_map[16][16];
		extern uint8_t wall_info;
		extern uint8_t m_step;
		extern uint16_t goal_x, goal_y;						//Goal Node
		extern uint8_t route[256];							//Shortest Route Array
		extern uint8_t r_cnt;								//経路カウンタ

		extern uint16_t wall_vertical[17];					//
		extern uint16_t wall_horizontal[17];				//

		extern int8_t pass[1024];
		extern uint16_t pass_end_count;
	#endif

		void InitializeMap();
		void WriteMap();

		void UpdatePosition();
		void UpdateDirection(uint8_t);
		void MakeStepMap(uint8_t);
		void MakeRoute_NESW();
		void ConfRoute_NESW(uint8_t);

		void ConvertMapIntoWall();
		void PrintWallData();
		void PrintHorizontalWallData();
		void PrintVerticalWallData();



#endif /* INC_MOUSE_MAP_H_ */
