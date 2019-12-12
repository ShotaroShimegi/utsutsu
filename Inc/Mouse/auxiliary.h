#ifndef AUXILIARY_H_
	#define AUXILIARY_H_
/*============================================================
		各種定数･変数宣言
============================================================*/
	typedef struct{
		//motion parameter
		float vel_max;
		float big_vel_max;
		float accel;
		float omega_max;
		float omega_accel;
		float big90_omega_max;
		float big90_omega_accel;
		float big180_omega_max;
		float big180_omega_accel;

		//distance parameter
		uint8_t TURN90_before;
		uint8_t TURN90_after;
		uint8_t big90_before;
		uint8_t big90_after;
		uint8_t big180_before;
		uint8_t big180_after;

		//angle offset parameter
		float R90_offset;
		float L90_offset;

	} params;

	typedef struct{
		float vel_kp;
		float vel_ki;
		float omega_kp;
		float omega_ki;
		float wall_kp;
		float wall_kd;
		float angle_kp;
		float angle_kd;
	} gain;

#define MEMORY 4000

	#ifdef MAIN_C_										//call from main.c
		/*Gloabl Variable Define*/
		params params_now;
		gain gain_now;
		params params_search1;
		params params_short1;
		params params_short2;

		gain gain_search1;
		gain gain_short1;
		gain gain_short2;

		volatile float test1[MEMORY];
		volatile float test2[MEMORY];
		volatile float test3[MEMORY];
		volatile float test4[MEMORY];

	#else
		extern params params_now;
		extern params params_short1;
		extern params params_short2;

		extern gain gain_now;
		extern params params_search1;
		extern gain gain_search1;
		extern gain gain_short1;
		extern gain gain_short2;

		extern volatile float test1[MEMORY];
		extern volatile float test2[MEMORY];
		extern volatile float test3[MEMORY];
		extern volatile float test4[MEMORY];

	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	void WaitMs(unsigned int);

	void ModeSelect(uint8_t *mode);
	void StartWaiting(void);
	void FirstAction(void);

	void AutoCalibration(float,float);
	void SetParams(params *instance);
	void SetGain(gain *instance);
	void LedDisplay(uint8_t *led);
	void ResetDistance(void);
	void CheckBattery(void);

	void CalculateNormalParams(params *instance,float,float,float);
	void CalculateBigParams(params *instance,float,float);
	void ApplyOffsetParams(params *instance, uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,float,float);
	void ApplyGain(gain *instance, float,float,float,float,float,float,float,float);

	void FailSafe(void);
	void FailCheck(void);

	void UtsutsuSystem();

#endif /* AUXILIARY_H_ */
