typedef struct{
	float dif;
	float p_out;
	float i_out;
	int8_t dir;
	float out;
} pid_control;

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

	/*** Structure [Velocity Control] [Angular Velocity Control]***/
	pid_control vel_ctrl_R;
	pid_control vel_ctrl_L;
	pid_control omega_control;

	volatile uint16_t utsutsu_time, ms_time;
	volatile float accel_time,omega_accel_time,big_accel_time;

	volatile float Kvolt,Kxr;				//加速度計算するための電流定数，距離変換のための定数

#else

	extern pid_control vel_ctrl_R;
	extern pid_control vel_ctrl_L;
	extern pid_control omega_control;

	extern uint16_t utsutsu_time,ms_time;
	extern volatile float accel_time,omega_accel_time,big_accel_time;

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
	void DriveSlalomFree(params*,int16_t);

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
	void PIDStructureInit(pid_control *);

#endif /* DRIVE_H_ */
