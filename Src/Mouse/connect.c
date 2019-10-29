#include <Mouse/global.h>

void GyroInit(void)
{
	uint8_t who_am_i = ReadByte(WHO_AM_I);
	uint8_t i = 0;
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

	for(i=0;i<100;i++){
		gyro_base += ReadGyro();
	}
		gyro_base = gyro_base * 0.01;
}

float ReadGyro(void){
	int16_t omega_raw_z;
	float omega;
	omega_raw_z = (int16_t)(ReadByte(GYRO_ZOUT_H) << 8 | ReadByte(GYRO_ZOUT_L));	//0x47が上位，0x48が下位の16bitデータでジャイロ値を取得
	omega = (float)(omega_raw_z / GYRO_FIX);
	return omega;
}

void GetGyroOffset(uint16_t num){
	float gyro_offset = 0;
	int i;

	for(i=0;i<num;i++){
		gyro_offset += ReadGyro();
		WaitMs(1);
	}
	gyro_base = gyro_offset / num;
}
