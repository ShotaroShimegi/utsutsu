#include <Mouse/port.h>
#include "main.h"

void LedDisplay(uint8_t *led)
{
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, *led&0x01);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, *led&0x02);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, *led&0x04);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, *led&0x08);
	HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, *led&0x10);

}
