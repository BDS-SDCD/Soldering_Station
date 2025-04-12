#ifndef INC_OLED_H_
#define INC_OLED_H_
#include "stm32f1xx.h"
#include "string.h"
#include "stdlib.h"
//----------------------------------------------------------------------------
#define OLED_Command_Buffer 0x80
#define OLED_Data_Buffer 0x40
#define OLED_Colume_Number 125
//----------------------------------------------------------------------------
/**
 * Display OLDE 128x64
 * Text font have 6x8 pixels character size. So 128/6=21 character + 2 pixels.
 * I remap display from 0 to 125 and now we have 21 character in one row.
 */

struct OLED{
	I2C_HandleTypeDef *hi2c;
	uint16_t Adress;
};
enum Display_Write_MODE{
	Direct_Display_MODE,	//direct
	Inverse_Display_MODE	//inverse
};
//----------------------------------------------------------------------------
void OLED_ini(struct OLED * self);
void OLED_Send_Command(struct OLED * self,uint8_t Command);
void OLED_Search_Adress(struct OLED * self);
void OLED_Set_Cursor(struct OLED * self,uint16_t x,uint8_t y);
void OLED_Set_Char_Cursor(struct OLED * self, uint16_t x,uint8_t y);
void OLED_Clear_Display(struct OLED * self);
void OLED_Send_Char(struct OLED * self, uint8_t data);
void OLED_Clear_Line (struct OLED * self, uint8_t y);
void OLED_Send_String(struct OLED * self,char* str, enum Display_Write_MODE Display_Write_MODE);
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* INC_OLED_H_ */
