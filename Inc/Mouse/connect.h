#include "spi.h"

#ifndef CONNECT_H_
	#define CONNECT_H_

	/*ICM-20689用のマクロ*/
	#define WHO_AM_I 0x75
	#define GYRO_CORREST_REACTION 0x98
	#define	PWR_MGMT_1 0x6b
	#define	CONFIG 0x1a
	#define GYRO_CONFIG 0x1b
	#define GYRO_ZOUT_H 0x47
	#define GYRO_ZOUT_L 0x48

	#define ACCEL_CONFIG 0x1c
	#define ACCEL_XOUT_H 0x3b
	#define ACCEL_XOUT_L 0x3c

/*============================================================
		関数プロトタイプ宣言
============================================================*/
		void GyroInit();
		float ReadGyroOmegaZ(void);
		float ReadGyroAccelX(void);
		void GetOmegaOffset(uint16_t);
		void GetAccelOffset(uint16_t);

#endif /* CONNECT_H_ */
