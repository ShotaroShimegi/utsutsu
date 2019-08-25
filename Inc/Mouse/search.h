#ifndef SEARCH_H_											//対応ファイルで一度も読み込まれていない場合以下を定義
	#define SEARCH_H_										//読み込んだことを表す
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

	//====変数====
	#ifdef MAIN_C_											//対応ファイルでEXTERNが定義されている場合
		/*グローバル変数の定義*/
		unsigned char map[16][16];									//マップ格納配列
		unsigned char smap[16][16];									//歩数マップ格納配列
		unsigned char wall_info;									//壁情報格納変数
		unsigned char m_dir;										//マウスの方向
		unsigned char m_step;										//歩数格納
		uint16_t goal_x, goal_y;								//ゴール座標
		unsigned char route[256];									//最短経路格納配列
		unsigned char r_cnt;										//経路カウンタ
	#else													//対応ファイルでEXTERNが定義されていない場合
		/*グローバル変数の宣言*/
		extern unsigned char map[16][16];							//マップ格納配列
		extern unsigned char smap[16][16];							//歩数マップ格納配列
		extern unsigned char wall_info;								//壁情報格納変数
		extern unsigned char m_dir;									//マウスの方向
		extern unsigned char m_step;								//歩数格納
		extern uint16_t goal_x, goal_y;							//ゴール座標
		extern unsigned char route[256];							//最短経路格納配列
		extern unsigned char r_cnt;									//経路カウンタ
	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====探索系====
	void searchA();											//一区画停止型探索走行
	void searchSA();										//連続探索走行
	void searchSA_ESNW();

	void searchSLA();
	void searchSLA_ESNW();

	void adv_pos();											//マウスの位置情報を前進
	void conf_route_NESW();										//次ルートの確認
	void conf_route_ESNW();
	void map_Init();										//マップデータ初期化
	void write_map();										//マップ書き込み
	void turn_dir(unsigned char);									//自機方向情報変更
	void make_smap();										//歩数マップ作成
	void make_route_NESW();										//最短経路検索
	void make_route_ESNW();

#endif /* SEARCH_H_ */
