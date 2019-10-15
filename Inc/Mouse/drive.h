typedef struct{
	float dif;			//速度偏差
	float p_out;			//P制御出力値
	float i_out;			//I制御出力
	int8_t dir;		//モータの回転方向
	float out;
} pid_control;

typedef struct{
	int16_t pulse;			//Raw Value
	int16_t  dif;
	int64_t sum; 			//Raw Integral
	float distance;			//Converted Value [mm]
	float velocity;			//unit m/s
} encoder;

typedef struct{
	float velocity;			//重心速度
	float vel_target;		//重心目標速度
	float omega_rad;		//重心角速度　radian単位
	float omega_deg;		//重心角速度　degree単位
	float omega_target;		//重心目標角速度
	float pre_omega_deg;	//角度計算用の保存用 radian単位

	int8_t omega_dir;		//角速度変化方向
	int8_t velocity_dir;
	float distance;			//重心走行距離
	float angle;			//重心角度　degree単位

} gravity;

#ifndef DRIVE_H_

	#define DRIVE_H_									//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----Signal Motor Direction----
	#define FORWARD	0x00
	#define BACK	0x11
	#define TURN_L	0x01
	#define TURN_R	0x10

	//----Signal Mouse Direction----
	#define DIR_SPIN_R90	0x01
	#define DIR_SPIN_L90	0xff
	#define DIR_SPIN_180	0x02

	#define GET_WALL_ON 1
	#define GET_WALL_OFF 0

	//====変数====
#ifdef MAIN_C_

	/*** Structure [Gravity-Center] [encoder] ***/
	volatile gravity center;
	volatile encoder encoder_r;
	volatile encoder encoder_l;

	/*** Structure [Velocity Control] [Angular Velocity Control]***/
	volatile pid_control vel_ctrl_R;
	volatile pid_control vel_ctrl_L;
	volatile pid_control omega_control;

	volatile uint16_t utsutsu_time, ms_time;
	volatile float maxindex, maxindex_w;		//時間・加速必要時間・角加速必要時間？

	volatile float Kvolt,Kxr;				//加速度計算するための電流定数，距離変換のための定数

#else
	/*** 重心・位置　構造体 ***/
	extern volatile gravity center;
	extern volatile encoder encoder_r;
	extern volatile encoder encoder_l;

	/*** 速度制御 構造体***/
	extern volatile pid_control vel_ctrl_R;
	extern volatile pid_control vel_ctrl_L;

	/***　角速度制御 構造体***/
	extern volatile pid_control omega_control;

	extern volatile uint16_t utsutsu_time,ms_time;
	extern volatile float maxindex,maxindex_w;

	extern volatile float Kvolt,Kxr;

#endif

/*============================================================
		Funciton Private Declaration
============================================================*/
	//====Peripheral Function====
	void StartMotion(void);
	void StopMotion(void);
	void DisableMotor(void);
	void SetMotionDirection(uint8_t);

	//====Basic Function====
	void DriveAccel(float);
	void DriveDecel(float, unsigned char);
	void DriveSpin(float);
	void DriveSlalom(int16_t);

	//====Drive Function====
	void HalfSectionAccel(uint8_t);
	void HalfSectionDecel();
	void GoOneSectionStop();
	void GoOneSectionContinuous();
	void SpinR90();
	void SpinL90();
	void Spin180();
	void FixPosition(uint8_t);
	void SlalomR90();
	void SlalomL90();

	void DriveTest(uint8_t *mode);

#endif /* DRIVE_H_ */
