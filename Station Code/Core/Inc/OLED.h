#ifndef INC_OLED_H_
#define INC_OLED_H_
#include "stm32f1xx.h"
//----------------------------------------------------------------------------
#define OLED_Comand_Buffer 0x80
#define OLED_Data_Buffer 0x40
//----------------------------------------------------------------------------
struct OLED{
	I2C_HandleTypeDef *hi2c;
	uint16_t Adress;
};
//----------------------------------------------------------------------------
void OLED_ini(struct OLED * self);
void OLED_Send_Comand(struct OLED * self,uint8_t comand);
void OLED_Search_Adress(struct OLED * self);
void OLED_Set_Cursor(struct OLED * self,uint16_t x,uint8_t y);
void OLED_Set_Char_Cursor(struct OLED * self, uint16_t x,uint8_t y);
void OLED_Clear_Display(struct OLED * self);
void OLED_Send_Char(struct OLED * self, uint8_t data);
void OLED_Clear_Char(struct OLED * self,uint16_t x, uint8_t y);
void OLED_Clear_Line (struct OLED * self, uint8_t y);
void OLED_Send_String(struct OLED * self,char* str);
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* INC_OLED_H_ */
