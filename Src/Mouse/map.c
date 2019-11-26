/*
 * map.c
 *
 *  Created on: 2019/11/11
 *      Author: 標 祥太郎
 */

#include "Mouse/global.h"

void InitializeMap()
{
	//====変数宣言====
	uint8_t x, y;										//for文用変数

	//====初期化開始====
	//マップのクリア
	for(y = 0; y <= 0x0f; y++){								//各Y座標で実行
		for(x = 0; x <= 0x0f; x++){							//各X座標で実行
			map[y][x] = 0xf0;								//上位(2次走行時)を壁あり、下位(1次走行時)を壁なしとする。
		}
	}

	//確定壁の配置
	for(y = 0; y <= 0x0f; y++){								//各Y座標で実行
		map[y][0] |= 0xf1;									//最西に壁を配置
		map[y][15] |= 0xf4;									//最東に壁を配置
	}
	for(x = 0; x <= 0x0f; x++){								//各X座標で実行
		map[0][x] |= 0xf2;									//最南に壁を配置
		map[15][x] |= 0xf8;									//最北に壁を配置
	}
}

void WriteMap()
{
	//====変数宣言====
	uint8_t m_temp;											//向きを補正した壁情報

	//====壁情報の補正格納====
	m_temp = (wall_info >> MOUSE.DIR) & 0x0f;					//センサ壁情報をMOUSE.DIRで向きを補正させて下位4bit分を残す
	m_temp |= (m_temp << 4);								//上位4bitに下位4bitをコピー。この作業でm_tempにNESW順で壁が格納

	//====データの書き込み====
	map[MOUSE.Y][MOUSE.X] = m_temp; 			//現在地に書き込み
	//----周辺に書き込む----
	//北側について
	if(MOUSE.Y != 15){								//現在最北端でないとき
		if(m_temp & 0x88){							//北壁がある場合
			map[MOUSE.Y + 1][MOUSE.X] |= 0x22;		//北側の区画から見て南壁ありを書き込む
		}else{										//北壁がない場合
			map[MOUSE.Y + 1][MOUSE.X] &= 0xDD;		//北側の区画から見て南壁なしを書き込む
		}
	}
	//東側について
	if(MOUSE.X != 15){								//現在最東端でないとき
		if(m_temp & 0x44){									//東壁がある場合
			map[MOUSE.Y][MOUSE.X + 1] |= 0x11;	//東側の区画から見て西壁ありを書き込む
		}else{												//北壁がない場合
			map[MOUSE.Y][MOUSE.X + 1] &= 0xEE;	//東側の区画から見て西壁なしを書き込む
		}
	}
	//南壁について
	if(MOUSE.Y != 0){									//現在最南端でないとき
		if(m_temp & 0x22){									//南壁がある場合
			map[MOUSE.Y - 1][MOUSE.X] |= 0x88;	//南側の区画から見て北壁ありを書き込む
		}else{												//南壁がない場合
			map[MOUSE.Y - 1][MOUSE.X] &= 0x77;	//南側の区画から見て北壁なしを書き込む
		}
	}
	//西側について
	if(MOUSE.X != 0){									//現在最西端でないとき
		if(m_temp & 0x11){									//西壁がある場合
			map[MOUSE.Y][MOUSE.X - 1] |= 0x44;	//西側の区画から見て東壁ありを書き込む
		}else{												//西壁がない場合
			map[MOUSE.Y][MOUSE.X - 1] &= 0xBB;	//西側の区画から見て東側なしを書き込む
		}
	}
}

void UpdatePosition()
{
	switch(MOUSE.DIR){
	case NORTH:
		MOUSE.Y++;
		break;
	case EAST:
		MOUSE.X++;
		break;
	case SOUTH:
		MOUSE.Y--;
		break;
	case WEST:
		MOUSE.X--;
		break;
	}
	printf("x: %d, y: %d, DIR: %x\n",MOUSE.X,MOUSE.Y,MOUSE.DIR);
}

void ConfRoute_NESW(uint8_t goal_size)
{
	//----壁情報書き込み----
	WriteMap();

	//----最短経路上に壁があれば進路変更----
	if(wall_info & route[r_cnt]){
		MakeStepMap(goal_size);							//歩数マップを更新
		MakeRoute_NESW();									//最短経路を更新
		r_cnt = 0;											//経路カウンタを0に
	}
}

void UpdateDirection(uint8_t t_pat)
{
	MOUSE.DIR = (MOUSE.DIR + t_pat) & 0x03;					//Update Mouse Direction Along [t_pat]
}

void MakeStepMap(uint8_t goal_length)
{
	uint8_t x, y;
	uint8_t m_temp;											//Keep Step number

	//====Clear Step Map====
	for(y = 0; y <= 0x0f; y++){
		for( x = 0; x <= 0x0f; x++){
			step_map[y][x] = 0xff;							//First, All Map Number is too bigger
		}
	}

	//====Set Step Number in Goal Axis====
	step_map[goal_y][goal_x] = 0;

	if(goal_length == 2) {
		step_map[goal_y + 1][goal_x] = 0;
		step_map[goal_y][goal_x + 1] = 0;
		step_map[goal_y + 1][goal_x + 1] = 0;
	}

	//====歩数カウンタを0にする====
	m_step = 0;												//Set First Step Number

	//====自分の座標にたどり着くまでループ====
	do{
		//----マップ全域を捜索----
		for( y = 0; y <= 0x0f; y++){						//各Y座標で実行
			for( x = 0; x <= 0x0f; x++){					//各X座標で実行
				//----現在最大の歩数を発見したとき----
				if( step_map[y][x] == m_step){				//歩数格納変数m_stepの値が現在最大の歩数
					m_temp = map[y][x];						//map配列からマップデータを取り出す
					if(MF.FLAG.SCND){						//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
						m_temp >>= 4;						//上位4bitを使うので4bit分右にシフトさせる
					}
					//----北壁についての処理----
					if(!(m_temp & 0x08) && y != 0x0f){		//北壁がなく現在最北端でないとき
						if(step_map[y+1][x] == 0xff){			//北側が未記入なら
							step_map[y+1][x] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----東壁についての処理----
					if(!(m_temp & 0x04) && x != 0x0f){		//東壁がなく現在最東端でないとき
						if(step_map[y][x+1] == 0xff){			//東側が未記入なら
							step_map[y][x+1] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----南壁についての処理----
					if(!(m_temp & 0x02) && y != 0){			//南壁がなく現在最南端でないとき
						if(step_map[y-1][x] == 0xff){			//南側が未記入なら
							step_map[y-1][x] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----西壁についての処理----
					if(!(m_temp & 0x01) && x != 0){			//西壁がなく現在最西端でないとき
						if(step_map[y][x-1] == 0xff){			//西側が未記入なら
							step_map[y][x-1] = m_step + 1;		//次の歩数を書き込む
						}
					}
				}
			}
		}
		//====歩数カウンタのインクリメント====
		m_step++;
	}while(step_map[MOUSE.Y][MOUSE.X] == 0xff);		//現在座標が未記入ではなくなるまで実行
}

void MakeRoute_NESW()
{
	//====変数宣言====
	uint8_t i = 0;
	uint8_t x, y;
	uint8_t dir_temp =  MOUSE.DIR;
	uint8_t m_temp;					//マップデータ一時保持

	//====最短経路を初期化====
	do{
		route[i++] = 0xff;										//routeを初期化、iをインクリメント
	}while(i != 0);												//iが0でない間実行(iがオーバーフローして0になるまで実行？)

	//====歩数カウンタをセット====
	m_step = step_map[MOUSE.Y][MOUSE.X];

	//====x, yに現在座標を書き込み====
	x = (uint8_t)MOUSE.X;
	y = (uint8_t)MOUSE.Y;

	//====最短経路を導出====
	do{
		m_temp = map[y][x];										//比較用マップ情報の格納
		if(MF.FLAG.SCND){										//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
			m_temp >>= 4;										//上位4bitを使うので4bit分右にシフトさせる
		}

		//----北を見る----
		if(!(m_temp & 0x08) && (step_map[y+1][x] < m_step)){		//北側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x00 - MOUSE.DIR) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y+1][x];								//最大歩数マップ値を更新
			y++;													//北に進んだのでY座標をインクリメント
		}
		//----東を見る----
		else if(!(m_temp & 0x04) && (step_map[y][x+1] < m_step)){	//東側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x01 - MOUSE.DIR) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y][x+1];								//最大歩数マップ値を更新
			x++;													//東に進んだのでX座標をインクリメント
		}
		//----南を見る----
		else if(!(m_temp & 0x02) && (step_map[y-1][x] < m_step)){	//南側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x02 - MOUSE.DIR) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y-1][x];								//最大歩数マップ値を更新
			y--;												//南に進んだのでY座標をデクリメント
		}
		//----西を見る----
		else if(!(m_temp & 0x01) && (step_map[y][x-1] < m_step)){	//西側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x03 - MOUSE.DIR) & 0x03;					//route配列に進行方向を記録
			m_step = step_map[y][x-1];								//最大歩数マップ値を更新
			x--;												//西に進んだのでX座標をデクリメント
		}

		//----格納データ形式変更----
		switch(route[i]){										//route配列に格納した要素値で分岐
		case 0x00:												//前進する場合
			route[i] = 0x88;									//格納データ形式を変更
			break;
		case 0x01:												//右折する場合
			UpdateDirection(DIR_SPIN_R90);								//内部情報の方向を90度右回転
			route[i] = 0x44;									//格納データ形式を変更
			break;
		case 0x02:												//Uターンする場合
			UpdateDirection(DIR_SPIN_180);						//内部情報の方向を180度回転
			route[i] = 0x22;									//格納データ形式を変更
			break;
		case 0x03:												//左折する場合
			UpdateDirection(DIR_SPIN_L90);						//内部情報の方向を90度右回転
			route[i] = 0x11;									//格納データ形式を変更
			break;
		default:												//それ以外の場合
			route[i] = 0x00;									//格納データ形式を変更
			break;
		}
		i++;													//カウンタをインクリメント
	}while( step_map[y][x] != 0);								//進んだ先の歩数マップ値が0(=ゴール)になるまで実行
	MOUSE.DIR = dir_temp;											//方向を始めの状態に戻す
}

void ConvertMapIntoWall()
{
	uint8_t x,y;
	//horizontal[16] -> Absolute North Wall
	//vertical[16] -> Absolute East Wall
	wall_horizontal[16] = 0xffff;
	wall_vertical[16] = 0xffff;

	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			wall_horizontal[y] += (((map[y][x] & 0x02) >> 1) << x);
			wall_vertical[x] +=  ((map[y][x] & 0x01) << y);
		}
	}
}

void PrintWallData()
{
	int8_t x,y;
	for(y=16;y>0;y--){
		printf("+");
		for(x=0;x<16;x++){
			if(wall_horizontal[y] & (0x01 << x)){
				printf("---");
			}else{
				printf("   ");
			}
			printf("+");
		}
		printf("\n");
		for(x=0;x<17;x++){
			if(wall_vertical[x] & (0x01 << (y-1))){
				printf("|   ");
			}else{
				printf("    ");
			}
		}
		printf("\n");
	}
	printf("+");
	for(x=0;x<16;x++){
		if(wall_horizontal[0] & (0x01 << x)){
			printf("---");
		}else{
			printf("   ");
		}
			printf("+");
	}
	printf("\n");
}

void PrintHorizontalWallData()
{
	int8_t y;
	printf("horizontal\n");
	for(y=16;y>=0;y--){
		printf("%d, %#x\n",y,wall_horizontal[y]);
	}

}
void PrintVerticalWallData()
{
	int8_t x;
	printf("vertical\n");
		for(x=0;x<17;x++){
			printf("%d, %#x\n",x,wall_vertical[x]);
		}
}
