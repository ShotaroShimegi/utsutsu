#ifndef AUXILIARY_H_
	#define AUXILIARY_H_
/*============================================================
		各種定数･変数宣言
============================================================*/
	typedef struct{
		//motion parameter
		float vel_max;
		float accel;
		float omega_max;
		float omega_accel;

		//distance parameter
		uint8_t R90_before;
		uint8_t R90_after;
		uint8_t L90_before;
		uint8_t L90_after;

	} params;

	typedef struct{
		float vel_kpR;
		float vel_kpL;
		float vel_kiR;
		float vel_kiL;
		float omega_kp;
		float omega_ki;
		float wall_kp;
		float wall_kd;
	} gain;

#define MEMORY 4000

	#ifdef MAIN_C_										//call from main.c
		/*Gloabl Variable Define*/
		volatile params params_now;
		volatile gain gain_now;
		volatile params params_search1;
		volatile gain gain_search1;
		volatile float test1[MEMORY];
		volatile float test2[MEMORY];
		volatile float test3[MEMORY];
	#else
		extern volatile params params_now;
		extern volatile gain gain_now;
		extern volatile params params_search1;
		extern volatile gain gain_search1;
		extern volatile float test1[MEMORY];
		extern volatile float test2[MEMORY];
		extern volatile float test3[MEMORY];

	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====その他====
	void stay(unsigned int);
	void WaitMs(unsigned int);

	void ModeSelect(uint8_t *mode);
	void MelodySummer(void);
	void MelodyMrLawrence(void);

	void Melody(uint32_t,uint32_t);
	void StartWaiting(void);
	void FirstAction(void);

	void AutoCalibration(float,float);
	void SetParams(params *instance);
	void SetGain(gain *instance);

	void LedDisplay(uint8_t *led);
	void ResetDistance(void);
	void CheckBattery(void);

	void UtsutsuSystem();

#endif /* AUXILIARY_H_ */
