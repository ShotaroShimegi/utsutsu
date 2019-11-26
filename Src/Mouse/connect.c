#include <Mouse/global.h>

void GyroInit(void)
{
	uint8_t who_am_i = ReadByte(WHO_AM_I);
	printf("Who am I ? -> 0x%x\n", who_am_i);

	if(who_am_i != GYRO_CORREST_REACTION){
		who_am_i = ReadByte(WHO_AM_I);
		while(who_am_i != GYRO_CORREST_REACTION){
			printf("Gyro Error\n");
			who_am_i = ReadByte(WHO_AM_I);
			HAL_Delay(100);
		}
	}
	printf("Who am I ? -> 0x%x\n", who_am_i);

	printf("Gyro OK\n");

	WriteByte(PWR_MGMT_1,0x00);
	HAL_Delay(10);
	WriteByte(CONFIG,0x00);
	HAL_Delay(10);
	WriteByte(GYRO_CONFIG,0x18);
	HAL_Delay(10);
	WriteByte(ACCEL_CONFIG,0x18);
	HAL_Delay(10);

	GetOmegaOffset(100);
	GetAccelOffset(100);

	printf("omega_base: %4lf,accel_base: %4lf\n",gyro_omega_base,gyro_accel_base);

	center.angle = 0.0f;
	center.angle_target = 0.0f;

}

float ReadGyroOmegaZ(void){
	int16_t omega_raw_z;
	float real_omega;
	omega_raw_z = (int16_t)(ReadByte(GYRO_ZOUT_H) << 8 | ReadByte(GYRO_ZOUT_L));	//0x47が上位，0x48が下位の16bitデータでジャイロ値を取得
	real_omega = (float)(omega_raw_z / GYRO_FIX);
	return real_omega;
}

float ReadGyroAccelX(void){
	int16_t accel_raw_x;
	float real_accel;
	accel_raw_x = (int16_t)(ReadByte(ACCEL_XOUT_H) << 8 | ReadByte(ACCEL_XOUT_L));	//0x47が上位，0x48が下位の16bitデータでジャイロ値を取得
	real_accel = (float)(accel_raw_x / ACCEL_FIX);
	return real_accel;
}

void GetOmegaOffset(uint16_t num){
	float gyro_offset = 0;
	int i;

	for(i=0;i<num;i++){
		gyro_offset += ReadGyroOmegaZ();
		WaitMs(1);
	}
	gyro_omega_base = gyro_offset / (float)num;
}

void GetAccelOffset(uint16_t num){
	float gyro_offset = 0;
	int i;

	for(i=0;i<num;i++){
		gyro_offset += ReadGyroAccelX();
		WaitMs(1);
	}
	gyro_accel_base = gyro_offset / (float)num;
}
