/*
 * Flash.h
 *
 *  Created on: Nov 11, 2024
 *      Author: I
 */

#ifndef INC_FLASH_CUSTOM_H_
#define INC_FLASH_CUSTOM_H_
//---------------------------------------------------------------------------------
#include "stm32f1xx.h"
//---------------------------------------------------------------------------------
#define Flash_Base_Addr 0x08000000
#define Flash_Addr_Page_Size 0x0400

//---------------------------------------------------------------------------------
uint8_t  Flash_Erase (uint32_t addr, uint16_t size);
uint32_t Flash_Write (uint32_t addr, void *data_ptr, uint16_t size);
void Flash_Read (uint32_t addr, void *data_ptr, uint16_t size);
uint32_t Flash_Write_Page (uint8_t Flash_Page, void *data_ptr, uint16_t size);
void Flash_Read_Page (uint32_t addr, void *data_ptr, uint16_t size);
//---------------------------------------------------------------------------------
#endif /* INC_FLASH_CUSTOM_H_ */
