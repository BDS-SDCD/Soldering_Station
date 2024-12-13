/*
 * Flash.c
 *
 *  Created on: Nov 11, 2024
 *      Author: I
 */
#include "Flash_Custom.h"



//---------------------------------------------------------------------------------
uint8_t Flash_Erase (uint32_t addr, uint16_t size){

	/**
	  * @addr  Base address.
	  * @size  Size all data for recording into page.
	  * @return HAL_StatusTypeDef HAL_OK or HAL_ERROR .
	  */

		FLASH_EraseInitTypeDef FlashErase;
		uint32_t sectorError = 0;

		__disable_irq();
		HAL_FLASH_Unlock();

		FlashErase.TypeErase = FLASH_TYPEERASE_PAGES;
		FlashErase.PageAddress = addr;
		FlashErase.NbPages = size / 1024 + 1;

		if (HAL_FLASHEx_Erase(&FlashErase, &sectorError) != HAL_OK)
		{
			HAL_FLASH_Lock();
	        __enable_irq();
			return HAL_ERROR;
		}
		HAL_FLASH_Lock();
		__enable_irq();
		return HAL_OK;
}

//---------------------------------------------------------------------------------
uint32_t Flash_Write (uint32_t addr, void *data_ptr, uint16_t size){
	/**
	  * addr  Base address.
	  * size  Size all data for recording into page.
	  * return HAL_StatusTypeDef HAL_OK or HAL_ERROR .
	  */

	HAL_StatusTypeDef status;
	__disable_irq();
	HAL_FLASH_Unlock();

	for (uint8_t i = 0; i < size/2; i++)
	{
		status += HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, ((uint16_t *)data_ptr)[i]);
		addr+=2;
	}
	HAL_FLASH_Lock();
	__enable_irq();

	if(status != HAL_OK)
		return status;
	else
		return addr;
}
//---------------------------------------------------------------------------------
void Flash_Read (uint32_t addr, void *data_ptr, uint16_t size){
	for (int i = 0; i < size; i++)
	{
		((uint8_t *)data_ptr)[i] = *(__IO uint32_t*)addr;
		addr++;
	}
}
//---------------------------------------------------------------------------------


