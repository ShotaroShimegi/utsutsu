/*
 * eeprom.c
 *
 *  Created on: May 2, 2019
 *      Author: blue
 */

#include "Mouse/global.h"
#include "stm32f4xx_hal.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_enable_write
// eepromとして使うflashメモリ領域をeraseし，書き込みを有効にする
// 引数：なし
// 戻り値：問題が起こらなければHAL_OKを返す
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_enable_write(void){
  HAL_StatusTypeDef status;
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PageError = 0;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.Sector = FLASH_SECTOR_11;
  EraseInitStruct.NbSectors = 1;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  status = HAL_FLASH_Unlock();

  if(status != HAL_OK) return status;
  status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
  return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_disable_write
// eepromとして使うflashメモリ領域への書き込みを無効にする
// 引数：なし
// 戻り値：問題が起こらなければHAL_OKを返す
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_disable_write(void){
  return HAL_FLASH_Lock();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_write_halfword
// eepromとして使うflashメモリ領域へ2バイトの値を書き込む
// 引数：address …… eepromとして使うflashメモリ領域の先頭アドレスからのオフセット，data …… 書き込みたいuin16_t型データ
// 戻り値：問題が起こらなければHAL_OKを返す
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_write_halfword(uint32_t address, uint16_t data){
  HAL_StatusTypeDef status;
  address = address*2 + EEPROM_START_ADDRESS;
  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
  return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_write_word
// eepromとして使うflashメモリ領域へ4バイトの値を書き込む
// 引数：address …… eepromとして使うflashメモリ領域の先頭アドレスからのオフセット，data …… 書き込みたいuin32_t型データ
// 戻り値：問題が起こらなければHAL_OKを返す
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_write_word(uint32_t address, uint32_t data) {
  HAL_StatusTypeDef status;
  address = address*4 + EEPROM_START_ADDRESS;
  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
  return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_read_halfword
// eepromとして使うflashメモリ領域から2バイトの値を読み込む
// 引数：address …… eepromとして使うflashメモリ領域の先頭アドレスからのオフセット
// 戻り値：読み込んだuin16_t型データ
//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t eeprom_read_halfword(uint32_t address){
  uint16_t val = 0;
  address = address*2 + EEPROM_START_ADDRESS;
  val = *(__IO uint16_t *)address;
  return val;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_read_word
// eepromとして使うflashメモリ領域から4バイトの値を読み込む
// 引数：address …… eepromとして使うflashメモリ領域の先頭アドレスからのオフセット
// 戻り値：読み込んだuin32_t型データ
//+++++++++++++++++++++++++++++++++++++++++++++++
uint32_t eeprom_read_word(uint32_t address){
  uint32_t val = 0;
  address = address*4 + EEPROM_START_ADDRESS;
  val = *(__IO uint32_t*)address;
  return val;
}

void SaveMapInEeprom(void)
{
  eeprom_enable_write();
  int i,j;

  for(i = 0; i < 16; i++){
    for(j = 0; j < 16; j++){
      eeprom_write_halfword(i*16 + j, (uint16_t) map[i][j]);
    }
  }
  eeprom_disable_write();
}

void LoadMapFromEeprom(void)
{
  int i,j;

  for(i = 0; i < 16; i++){
    for(j = 0; j < 16; j++){
      map[i][j] = (uint8_t) eeprom_read_halfword(i*16 + j);
    }
  }
}
