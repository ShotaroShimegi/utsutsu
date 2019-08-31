/*
==============================================================
 Name        : drive.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 走行関連の関数たちです．

==============================================================
*/

/*ヘッダファイルの読み込み*/
#include "port.h"

typedef struct{
	float real;			//測定した速度
	float dif;			//速度偏差
	float pre;			//偏差直前値
	float p_out;			//P制御出力値
	float i_out;			//I制御出力
	int8_t dir;		//モータの回転方向
	float out;
} vel_ctrl;

typedef struct{
	float target;			//目標角速度
	float dif;			//角速度偏差
	float p_out;			//P制御の出力値
	float i_out;			//I制御の出力値
	int8_t dir;			//回転方向
	float out;
} omega_ctrl;

typedef struct{
	int16_t pulse;			//Raw Value
	int16_t  dif;
	int64_t sum; 			//Raw Integral
	float distance;			//Converted Value [mm]
	float velocity;
} encoder;

typedef struct{
	float velocity;			//重心速度
	float vel_target;		//重心目標速度
	float omega_deg;		//重心角速度　degree単位
	float pre_omega_deg;		//角度計算用の保存用
	float omega_rad;		//重心角速度　radian単位
	int8_t omega_dir;		//角速度変化方向
	float distance;			//重心走行距離
	float angle;			//重心角度　degree単位

} gravity;

#ifndef DRIVE_H_

	#define DRIVE_H_									//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----動作方向関連----
	#define FORWARD	0x00			//前進向き
	#define BACK	0x11			//後退
	#define TURN_L	0x01			//回転向き(左)
	#define TURN_R	0x10			//回転向き(右)

	//----方向転換用定数----
	#define DIR_TURN_R90	0x01	//右90度回転
	#define DIR_TURN_L90	0xff	//左90度回転
	#define DIR_TURN_180	0x02	//180度回転


	//====変数====
#ifdef MAIN_C_

	/*** 重心・位置　構造体 ***/
	volatile gravity centor;
	volatile encoder encoder_r;
	volatile encoder encoder_l;

	/*** 速度制御 構造体***/
	volatile vel_ctrl vel_ctrl_R;
	volatile vel_ctrl vel_ctrl_L;

	/***　角速度制御 構造体***/
	volatile omega_ctrl omega;

	volatile uint16_t time,time2, ms_time;			//
	volatile float minindex, maxindex,maxindex_w, number;		//時間・加速必要時間・角加速必要時間？

	volatile float Kvolt,Kxr;				//加速度計算するための電流定数，距離変換のための定数

#else									//対応ファイルでEXTERNが定義されていない場合
	/*** 重心・位置　構造体 ***/
	extern volatile gravity centor;
	extern volatile encoder encoder_r;
	extern volatile encoder encoder_l;

	/*** 速度制御 構造体***/
	extern volatile vel_ctrl vel_ctrl_R;
	extern volatile vel_ctrl vel_ctrl_L;

	/***　角速度制御 構造体***/
	extern volatile omega_ctrl omega;

	extern volatile uint16_t time,time2, ms_time;
	extern volatile float minindex, maxindex,maxindex_w,number;

	extern volatile float Kvolt,Kxr;

#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====走行系====
	//----基幹関数----
	void driveA(float);	//加速走行
	void driveD(uint16_t, unsigned char);	//減速走行
	void driveAD(float);	//加減速走行
	void driveC(uint16_t, unsigned char);	//定時間走行，セッポジぐらいしか使ってない
	void driveX(uint16_t);			//位置・角度制御走行
	void driveW(int16_t);			//角速度制御走行

	void set_dir(unsigned char);		//進む方向の設定

	void drive_start();
	void drive_stop(unsigned char);

	//----簡易上位関数----
	void half_sectionA();		//加速半区画
	void half_sectionA2();
	void half_sectionD();		//減速半区画
	void a_section();		//加減速一区画
	void a_sectionU();		//等速一区画
	void s_section();		//連続区画直線走行
	void turn_R90();		//右90回転
	void turn_L90();		//左90回転
	void turn_180();		//180度回転
	void set_position(uint8_t);		//位置合わせ

	void turn_SLA_R90();
	void turn_SLA_L90();

	//----走行試験----
	void test_drive(char *mode);	//走行試験

	void start_ready();


#endif /* DRIVE_H_ */
