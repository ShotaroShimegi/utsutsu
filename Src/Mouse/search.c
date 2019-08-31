#include <Mouse/global.h>

/*===========================================================
		探索系関数
===========================================================*/
/*-----------------------------------------------------------
		足立法探索走行α(一区画走行)
-----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//searchA
//	goal座標に進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void searchA(){												//一次走行　一番基本的な初期装備

	//====マップデータ初期化====
	map_Init();												//マップデータを初期化する

	//====歩数等初期化====
	m_step = r_cnt = 0;										//歩数と経路カウンタの初期化
	get_wall_info();										//壁情報の初期化, 後壁はなくなる
	write_map();											//地図の初期化
	make_smap();											//歩数図の初期化
	make_route_NESW();											//最短経路探索(route配列に動作が格納される)
	sensor_start();

	//uart_printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d\r\n ", ad_l, ad_fl, ad_ff, ad_fr, ad_r);
	//====探索走行====
	do{

		//----進行----
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			//----前進----
			case 0x88:
				set_dir(FORWARD);
				//Melody(1120,500);
				break;
			//----右折----
			case 0x44:
				turn_R90();									//右回転
				if(wall_l.dif > wall_l.threshold){
					set_position(1);
				}
				turn_dir(DIR_TURN_R90);						//マイクロマウス内部位置情報でも右回転処理
				HAL_Delay(100);										//安定するまで待機
				set_dir(FORWARD);
				//Melody(920,500);
				break;
			//----180回転----
			case 0x22:
				turn_180();							//180度回転
				if(wall_ff.dif > wall_ff.threshold){
					set_position(1);
				}
				turn_dir(DIR_TURN_180);						//マイクロマウス内部位置情報でも180度回転処理
				HAL_Delay(100);
				set_dir(FORWARD);
				//Melody(1320,500);
				break;
			//----左折----
			case 0x11:
				turn_L90();									//左回転
				if(wall_r.dif > wall_ff.threshold){
					set_position(1);
				}
				turn_dir(DIR_TURN_L90);						//マイクロマウス内部位置情報でも左回転処理
				HAL_Delay(100);									//安定するまで待機
				set_dir(FORWARD);
				//Melody(720,500);
				break;
		}
/*		uart_printf("time = %d, wall_l = %d, wall_ff = %d, wall_r = %d\r\n",time2, wall_l.dif, wall_ff.dif, wall_r.dif);
		uart_printf("route is %2x, threshold_l = %d, threthreshold_r = %d\r\n", route[r_cnt - 1], wall_l.threshold, wall_r.threshold);
		ms_wait(500);
*/
		a_section();										//前進する
		adv_pos();										//マイクロマウス内部位置情報でも前進処理
		conf_route_NESW();										//最短経路で進行可能か判定

	}while((PRELOC.AXIS.X != goal_x) || (PRELOC.AXIS.Y != goal_y));
															//現在座標とgoal座標が等しくなるまで実行
	ms_wait(2000);											//スタートでは***2秒以上***停止しなくてはならない
	turn_180();												//180度回転
	turn_dir(DIR_TURN_180);									//マイクロマウス内部位置情報でも180度回転処理
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//searchSA
//	ちょっと早くgoal座標に進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void searchSA(){											//連続走行の未完成アルゴリズム、完成させればケッコー早い走行ができる、初期化は直してないからがんばって　標

	//====歩数等初期化====
	m_step = r_cnt = 0;										//歩数と経路カウンタの初期化
	get_wall_info();										//壁情報の初期化, 後壁はなくなる
	write_map();											//地図の初期化
	make_smap();											//歩数図の初期化
	make_route_NESW();											//最短経路探索(route配列に動作が格納される)

	sensor_start();
	printf("Michishirube\r\n");
	set_dir(FORWARD);

	if(wall_ff.dif > wall_ff.threshold){
		turn_180();
		HAL_Delay(100);
		turn_dir(DIR_TURN_180);
		set_dir(FORWARD);
	}

	half_sectionA();
	adv_pos();
	conf_route_NESW();
	//====探索走行====
	do{
		//----進行----
		switch(route[r_cnt++]){			//route配列によって進行を決定。経路カウンタを進める
			//----前進----
			case 0x88:
				s_section();
				break;
			//----右折----
			case 0x44:
				half_sectionD();
				turn_R90();
				HAL_Delay(100);
				turn_dir(DIR_TURN_R90);
				set_dir(FORWARD);
				half_sectionA();
				break;
			//----180回転----
			case 0x22:
				half_sectionD();
				if(wall_ff.dif > wall_ff.threshold){
					MF.FLAG.SET = 1;
				}
				turn_180();
				HAL_Delay(100);
				turn_dir(DIR_TURN_180);
				set_dir(FORWARD);

				if(MF.FLAG.SET){
					set_position(1);
					MF.FLAG.SET = 0;
				}
				MF.FLAG.CTRL = 0;
				driveA(HALF_MM);
				get_wall_info();
				break;
			//----左折----
			case 0x11:
				half_sectionD();
				turn_L90();
				HAL_Delay(100);
				turn_dir(DIR_TURN_L90);
				set_dir(FORWARD);

				half_sectionA();
				break;
		}

		adv_pos();
		conf_route_NESW();

	}while((PRELOC.AXIS.X != goal_x) || (PRELOC.AXIS.Y != goal_y));
	half_sectionD();
	ms_wait(2000);
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);


}

void searchSA_ESNW(){											//連続走行の未完成アルゴリズム、完成させればケッコー早い走行ができる、初期化は直してないからがんばって　標

	//====歩数等初期化====
	m_step = r_cnt = 0;										//歩数と経路カウンタの初期化
	get_wall_info();										//壁情報の初期化, 後壁はなくなる
	write_map();											//地図の初期化
	make_smap();											//歩数図の初期化
	make_route_ESNW();											//最短経路探索(route配列に動作が格納される)

	sensor_start();
	printf("Michishirube\r\n");
	set_dir(FORWARD);

	if(wall_ff.dif > wall_ff.threshold){
		turn_180();
		HAL_Delay(100);
		turn_dir(DIR_TURN_180);
		set_dir(FORWARD);
	}

	half_sectionA();
	adv_pos();
	conf_route_ESNW();
	//====探索走行====
	do{
		//----進行----
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			//----前進----
			case 0x88:
				s_section();
				break;
			//----右折----
			case 0x44:
				if(wall_l.dif > wall_l.dif + WALL_OFF){
					MF.FLAG.SET = 1;
				}
				half_sectionD();
				turn_R90();
				HAL_Delay(100);
				turn_dir(DIR_TURN_R90);
				set_dir(FORWARD);
				if(MF.FLAG.SET){
					set_position(1);
					MF.FLAG.SET = 0;
				}
				half_sectionA();
				break;
			//----180回転----
			case 0x22:
				half_sectionD();
				if(wall_ff.dif > wall_ff.threshold){
					MF.FLAG.SET = 1;
				}
				turn_180();
				HAL_Delay(100);
				turn_dir(DIR_TURN_180);
				set_dir(FORWARD);

				if(MF.FLAG.SET){
					set_position(1);
					MF.FLAG.SET = 0;
				}
				half_sectionA();
				break;
			//----左折----
			case 0x11:
				if(wall_r.dif > wall_r.threshold + WALL_OFF){
					MF.FLAG.SET = 1;
				}
				half_sectionD();
				turn_L90();
				HAL_Delay(100);
				turn_dir(DIR_TURN_L90);
				set_dir(FORWARD);
				if(MF.FLAG.SET){
					set_position(1);
					MF.FLAG.SET = 0;
				}
				half_sectionA();
				break;
		}
		adv_pos();
		conf_route_ESNW();


	}while((PRELOC.AXIS.X != goal_x) || (PRELOC.AXIS.Y != goal_y));
	half_sectionD();
	ms_wait(2000);
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);

}

void searchSLA(){											//連続走行の未完成アルゴリズム、完成させればケッコー早い走行ができる、初期化は直してないからがんばって　標

	//====歩数等初期化====
	m_step = r_cnt = 0;										//歩数と経路カウンタの初期化
	get_wall_info();										//壁情報の初期化, 後壁はなくなる
	write_map();											//地図の初期化
	make_smap();											//歩数図の初期化
	make_route_NESW();											//最短経路探索(route配列に動作が格納される)

	sensor_start();
	printf("Michishirube\r\n");
	set_dir(FORWARD);

	if(wall_ff.dif > wall_ff.threshold){
		turn_180();
		HAL_Delay(100);
		turn_dir(DIR_TURN_180);
		set_dir(FORWARD);
	}

	half_sectionA();
	adv_pos();
	conf_route_NESW();
	//====探索走行====
	do{
		//----進行----
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			//----前進----
			case 0x88:
				s_section();                                //このプログラムには無い関数、他のプログラムと比べて類似の関数を探してピッタリなのを作れ　標
				break;
			//----右折----
			case 0x44:
				turn_SLA_R90();
				turn_dir(DIR_TURN_R90);
				set_dir(FORWARD);
				break;
			//----180回転----
			case 0x22:
				half_sectionD();
				if(wall_ff.dif > wall_ff.threshold){
					MF.FLAG.SET = 1;
				}
				turn_180();
				HAL_Delay(100);
				turn_dir(DIR_TURN_180);
				set_dir(FORWARD);

				if(MF.FLAG.SET){
					set_position(0);
					MF.FLAG.SET = 0;
				}
				half_sectionA();			//ここでも一文だけ抜けています、走らせてみると上手くいかないのはこの辺のせい、　標
				break;
			//----左折----
			case 0x11:
				turn_SLA_L90();
				turn_dir(DIR_TURN_L90);
				set_dir(FORWARD);

				break;
		}
		//uart_printf("x:%d, y:%d\r\n",PRELOC.AXIS.X,PRELOC.AXIS.Y);
		adv_pos();
		conf_route_NESW();


	}while((PRELOC.AXIS.X != goal_x) || (PRELOC.AXIS.Y != goal_y));
	half_sectionD();
	ms_wait(2000);
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);

}

void searchSLA_ESNW(){											//連続走行の未完成アルゴリズム、完成させればケッコー早い走行ができる、初期化は直してないからがんばって　標

	//====歩数等初期化====
	m_step = r_cnt = 0;										//歩数と経路カウンタの初期化
	get_wall_info();										//壁情報の初期化, 後壁はなくなる
	write_map();											//地図の初期化
	make_smap();											//歩数図の初期化
	make_route_ESNW();											//最短経路探索(route配列に動作が格納される)

	sensor_start();
	printf("Michishirube\r\n");
	set_dir(FORWARD);

	if(wall_ff.dif > wall_ff.threshold){
		turn_180();
		HAL_Delay(100);
		turn_dir(DIR_TURN_180);
		set_dir(FORWARD);
	}

	half_sectionA();
	adv_pos();
	conf_route_NESW();
	//====探索走行====
	do{
		//----進行----
		switch(route[r_cnt++]){								//route配列によって進行を決定。経路カウンタを進める
			//----前進----
			case 0x88:
				s_section();                                //このプログラムには無い関数、他のプログラムと比べて類似の関数を探してピッタリなのを作れ　標
				break;
			//----右折----
			case 0x44:
				turn_SLA_R90();
				turn_dir(DIR_TURN_R90);
				set_dir(FORWARD);
				break;
			//----180回転----
			case 0x22:
				half_sectionD();
				if(wall_ff.threshold > wall_ff.threshold){
					MF.FLAG.SET = 1;
				}
				turn_180();
				HAL_Delay(100);
				turn_dir(DIR_TURN_180);
				set_dir(FORWARD);

				if(MF.FLAG.SET){
					set_position(0);
					MF.FLAG.SET = 0;
				}
				half_sectionA();			//ここでも一文だけ抜けています、走らせてみると上手くいかないのはこの辺のせい、　標
				break;
			//----左折----
			case 0x11:
				turn_SLA_L90();
				turn_dir(DIR_TURN_L90);
				set_dir(FORWARD);

				break;
		}
		//uart_printf("x:%d, y:%d\r\n",PRELOC.AXIS.X,PRELOC.AXIS.Y);
		adv_pos();
		conf_route_ESNW();


	}while((PRELOC.AXIS.X != goal_x) || (PRELOC.AXIS.Y != goal_y));
	half_sectionD();
	ms_wait(2000);
	Melody(g6,300);
	Melody(f6,300);
	Melody(e6,300);

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//adv_pos
//	マイクロマウス内部位置情報で前進させる
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void adv_pos()
{
	switch(m_dir){											//マイクロマウスが現在向いている方向で判定
	case 0x00:												//北方向に向いている場合
		(PRELOC.AXIS.Y)++;									//Y座標をインクリメント
		break;
	case 0x01:												//東方向に向いている場合
		(PRELOC.AXIS.X)++;									//X座標をインクリメント
		break;
	case 0x02:												//南方向に向いている場合
		(PRELOC.AXIS.Y)--;									//Y座標をデクリメント
		break;
	case 0x03:												//西方向に向いている場合
		(PRELOC.AXIS.X)--;									//X座標をデクリメント
		break;
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//conf_route
//	進路を判定する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void conf_route_NESW()
{
	//----壁情報書き込み----
	write_map();

	//----最短経路上に壁があれば進路変更----
	if(wall_info & route[r_cnt]){
		make_smap();										//歩数マップを更新
		make_route_NESW();										//最短経路を更新
		r_cnt = 0;											//経路カウンタを0に
	}
}

void conf_route_ESNW()
{
	//----壁情報書き込み----
	write_map();

	//----最短経路上に壁があれば進路変更----
	if(wall_info & route[r_cnt]){
		make_smap();										//歩数マップを更新
		make_route_ESNW();										//最短経路を更新
		r_cnt = 0;											//経路カウンタを0に
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//map_Init
//	マップ格納配列map[][]の初期化をする
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void map_Init()
{
	//====変数宣言====
	unsigned char x, y;												//for文用変数

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


//+++++++++++++++++++++++++++++++++++++++++++++++
//write_map
//	マップデータを書き込む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void write_map()
{
	//====変数宣言====
	unsigned char m_temp;											//向きを補正した壁情報

	//====壁情報の補正格納====
	m_temp = (wall_info >> m_dir) & 0x0f;					//センサ壁情報をm_dirで向きを補正させて下位4bit分を残す
	m_temp |= (m_temp << 4);								//上位4bitに下位4bitをコピー。この作業でm_tempにNESW順で壁が格納

	//====データの書き込み====
	map[PRELOC.AXIS.Y][PRELOC.AXIS.X] = m_temp; 			//現在地に書き込み
	//----周辺に書き込む----
	//北側について
	if(PRELOC.AXIS.Y != 15){								//現在最北端でないとき
		if(m_temp & 0x88){									//北壁がある場合
			map[PRELOC.AXIS.Y + 1][PRELOC.AXIS.X] |= 0x22;	//北側の区画から見て南壁ありを書き込む
		}else{												//北壁がない場合
			map[PRELOC.AXIS.Y + 1][PRELOC.AXIS.X] &= 0xDD;	//北側の区画から見て南壁なしを書き込む
		}
	}
	//東側について
	if(PRELOC.AXIS.X != 15){								//現在最東端でないとき
		if(m_temp & 0x44){									//東壁がある場合
			map[PRELOC.AXIS.Y][PRELOC.AXIS.X + 1] |= 0x11;	//東側の区画から見て西壁ありを書き込む
		}else{												//北壁がない場合
			map[PRELOC.AXIS.Y][PRELOC.AXIS.X + 1] &= 0xEE;	//東側の区画から見て西壁なしを書き込む
		}
	}
	//南壁について
	if(PRELOC.AXIS.Y != 0){									//現在最南端でないとき
		if(m_temp & 0x22){									//南壁がある場合
			map[PRELOC.AXIS.Y - 1][PRELOC.AXIS.X] |= 0x88;	//南側の区画から見て北壁ありを書き込む
		}else{												//南壁がない場合
			map[PRELOC.AXIS.Y - 1][PRELOC.AXIS.X] &= 0x77;	//南側の区画から見て北壁なしを書き込む
		}
	}
	//西側について
	if(PRELOC.AXIS.X != 0){									//現在最西端でないとき
		if(m_temp & 0x11){									//西壁がある場合
			map[PRELOC.AXIS.Y][PRELOC.AXIS.X - 1] |= 0x44;	//西側の区画から見て東壁ありを書き込む
		}else{												//西壁がない場合
			map[PRELOC.AXIS.Y][PRELOC.AXIS.X - 1] &= 0xBB;	//西側の区画から見て東側なしを書き込む
		}
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//write_map
//	マウスの方向を変更する
// 引数1：t_pat・・・回転方向(drive.hでマクロ定義)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_dir(unsigned char	t_pat)
{
	//====方向を変更====
	m_dir = (m_dir + t_pat) & 0x03;							//指定された分m_dirを回転させる
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//make_smap
//	歩数マップを作成する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void make_smap()
{
	//====変数宣言====
	unsigned char x, y;												//for文用変数
	unsigned char m_temp;											//マップデータ一時保持

	//====歩数マップのクリア====
	for(y = 0; y <= 0x0f; y++){								//各Y座標で実行
		for( x = 0; x <= 0x0f; x++){						//各X座標で実行
			smap[y][x] = 0xff;								//未記入部分は歩数最大とする
		}
	}

	//====ゴール座標を0にする====
	smap[goal_y][goal_x] = 0;

	//====歩数カウンタを0にする====
	m_step = 0;												//現在記入した最大の歩数となる

	//====自分の座標にたどり着くまでループ====
	do{
		//----マップ全域を捜索----
		for( y = 0; y <= 0x0f; y++){						//各Y座標で実行
			for( x = 0; x <= 0x0f; x++){					//各X座標で実行
				//----現在最大の歩数を発見したとき----
				if( smap[y][x] == m_step){					//歩数格納変数m_stepの値が現在最大の歩数
					m_temp = map[y][x];						//map配列からマップデータを取り出す
					if(MF.FLAG.SCND){						//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
						m_temp >>= 4;						//上位4bitを使うので4bit分右にシフトさせる
					}
					//----北壁についての処理----
					if(!(m_temp & 0x08) && y != 0x0f){		//北壁がなく現在最北端でないとき
						if(smap[y+1][x] == 0xff){			//北側が未記入なら
							smap[y+1][x] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----東壁についての処理----
					if(!(m_temp & 0x04) && x != 0x0f){		//東壁がなく現在最東端でないとき
						if(smap[y][x+1] == 0xff){			//東側が未記入なら
							smap[y][x+1] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----南壁についての処理----
					if(!(m_temp & 0x02) && y != 0){			//南壁がなく現在最南端でないとき
						if(smap[y-1][x] == 0xff){			//南側が未記入なら
							smap[y-1][x] = m_step + 1;		//次の歩数を書き込む
						}
					}
					//----西壁についての処理----
					if(!(m_temp & 0x01) && x != 0){			//西壁がなく現在最西端でないとき
						if(smap[y][x-1] == 0xff){			//西側が未記入なら
							smap[y][x-1] = m_step + 1;		//次の歩数を書き込む
						}
					}
				}
			}
		}
		//====歩数カウンタのインクリメント====
		m_step++;
	}while(smap[PRELOC.AXIS.Y][PRELOC.AXIS.X] == 0xff);		//現在座標が未記入ではなくなるまで実行
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//make_route
//	最短経路を導出する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void make_route_NESW()
{
	//====変数宣言====
	unsigned char i = 0;												//カウンタ
	unsigned char x, y;													//X、Y座標
	unsigned char dir_temp =  m_dir;									//方向の保管用変数
	unsigned char m_temp;												//マップデータ一時保持

	//====最短経路を初期化====
	do{
		route[i++] = 0xff;										//routeを初期化、iをインクリメント
	}while(i != 0);												//iが0でない間実行(iがオーバーフローして0になるまで実行？)

	//====歩数カウンタをセット====
	m_step = smap[PRELOC.AXIS.Y][PRELOC.AXIS.X];				//現在座標の歩数マップ値を取得

	//====x, yに現在座標を書き込み====
	x = (unsigned char)PRELOC.AXIS.X;
	y = (unsigned char)PRELOC.AXIS.Y;

	//====最短経路を導出====
	do{
		m_temp = map[y][x];										//比較用マップ情報の格納
		if(MF.FLAG.SCND){										//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
			m_temp >>= 4;										//上位4bitを使うので4bit分右にシフトさせる
		}

		//----北を見る----
		if(!(m_temp & 0x08) && (smap[y+1][x] < m_step)){		//北側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x00 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y+1][x];								//最大歩数マップ値を更新
			y++;												//北に進んだのでY座標をインクリメント
		}
		//----東を見る----
		else if(!(m_temp & 0x04) && (smap[y][x+1] < m_step)){	//東側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x01 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y][x+1];								//最大歩数マップ値を更新
			x++;												//東に進んだのでX座標をインクリメント
		}
		//----南を見る----
		else if(!(m_temp & 0x02) && (smap[y-1][x] < m_step)){	//南側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x02 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y-1][x];								//最大歩数マップ値を更新
			y--;												//南に進んだのでY座標をデクリメント
		}
		//----西を見る----
		else if(!(m_temp & 0x01) && (smap[y][x-1] < m_step)){	//西側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x03 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y][x-1];								//最大歩数マップ値を更新
			x--;												//西に進んだのでX座標をデクリメント
		}

		//----格納データ形式変更----
		switch(route[i]){										//route配列に格納した要素値で分岐
		case 0x00:												//前進する場合
			route[i] = 0x88;									//格納データ形式を変更
			break;
		case 0x01:												//右折する場合
			turn_dir(DIR_TURN_R90);								//内部情報の方向を90度右回転
			route[i] = 0x44;									//格納データ形式を変更
			break;
		case 0x02:												//Uターンする場合
			turn_dir(DIR_TURN_180);								//内部情報の方向を180度回転
			route[i] = 0x22;									//格納データ形式を変更
			break;
		case 0x03:												//左折する場合
			turn_dir(DIR_TURN_L90);								//内部情報の方向を90度右回転
			route[i] = 0x11;									//格納データ形式を変更
			break;
		default:												//それ以外の場合
			route[i] = 0x00;									//格納データ形式を変更
			break;
		}
		i++;													//カウンタをインクリメント
	}while( smap[y][x] != 0);									//進んだ先の歩数マップ値が0(=ゴール)になるまで実行
	m_dir = dir_temp;											//方向を始めの状態に戻す
}

void make_route_ESNW()
{
	//====変数宣言====
	unsigned char i = 0;												//カウンタ
	unsigned char x, y;													//X、Y座標
	unsigned char dir_temp =  m_dir;									//方向の保管用変数
	unsigned char m_temp;												//マップデータ一時保持

	//====最短経路を初期化====
	do{
		route[i++] = 0xff;										//routeを初期化、iをインクリメント
	}while(i != 0);												//iが0でない間実行(iがオーバーフローして0になるまで実行？)

	//====歩数カウンタをセット====
	m_step = smap[PRELOC.AXIS.Y][PRELOC.AXIS.X];				//現在座標の歩数マップ値を取得

	//====x, yに現在座標を書き込み====
	x = (unsigned char)PRELOC.AXIS.X;
	y = (unsigned char)PRELOC.AXIS.Y;

	//====最短経路を導出====
	do{
		m_temp = map[y][x];										//比較用マップ情報の格納
		if(MF.FLAG.SCND){										//二次走行用のマップを作成する場合(二次走行時はMF.FLAG.SCNDをTrueにする)
			m_temp >>= 4;										//上位4bitを使うので4bit分右にシフトさせる
		}

		//----東を見る----
		if(!(m_temp & 0x04) && (smap[y][x+1] < m_step)){	//東側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x01 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y][x+1];								//最大歩数マップ値を更新
			x++;												//東に進んだのでX座標をインクリメント
		}
		//----南を見る----
		else if(!(m_temp & 0x02) && (smap[y-1][x] < m_step)){	//南側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x02 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y-1][x];								//最大歩数マップ値を更新
			y--;												//南に進んだのでY座標をデクリメント
		}
		//----北を見る----
		else if(!(m_temp & 0x08) && (smap[y+1][x] < m_step)){		//北側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x00 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y+1][x];								//最大歩数マップ値を更新
			y++;												//北に進んだのでY座標をインクリメント
		}
		//----西を見る----
		else if(!(m_temp & 0x01) && (smap[y][x-1] < m_step)){	//西側に壁が無く、現在地より小さい歩数マップ値であれば
			route[i] = (0x03 - m_dir) & 0x03;					//route配列に進行方向を記録
			m_step = smap[y][x-1];								//最大歩数マップ値を更新
			x--;												//西に進んだのでX座標をデクリメント
		}


		//----格納データ形式変更----
		switch(route[i]){										//route配列に格納した要素値で分岐
		case 0x00:												//前進する場合
			route[i] = 0x88;									//格納データ形式を変更
			break;
		case 0x01:												//右折する場合
			turn_dir(DIR_TURN_R90);								//内部情報の方向を90度右回転
			route[i] = 0x44;									//格納データ形式を変更
			break;
		case 0x02:												//Uターンする場合
			turn_dir(DIR_TURN_180);								//内部情報の方向を180度回転
			route[i] = 0x22;									//格納データ形式を変更
			break;
		case 0x03:												//左折する場合
			turn_dir(DIR_TURN_L90);								//内部情報の方向を90度右回転
			route[i] = 0x11;									//格納データ形式を変更
			break;
		default:												//それ以外の場合
			route[i] = 0x00;									//格納データ形式を変更
			break;
		}
		i++;													//カウンタをインクリメント
	}while( smap[y][x] != 0);									//進んだ先の歩数マップ値が0(=ゴール)になるまで実行
	m_dir = dir_temp;											//方向を始めの状態に戻す
}
