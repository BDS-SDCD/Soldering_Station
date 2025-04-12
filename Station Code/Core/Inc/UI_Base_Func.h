
#ifndef INC_UI_BASE_FUNC_H_
#define INC_UI_BASE_FUNC_H_

#include "stm32f1xx.h"
#include "string.h"
#include "OLED.h"
#include "Button.h"
//----------------------------------------------------------------------------
extern struct OLED OLED1;
//----------------------------------------------------------------------------
enum Type{
	INT,
	UINT8,
	UINT16,
	BOOL
};
//----------------------------------------------------------------------------
void UI_Draw_Parmetr_Ptr(void *value, uint8_t x,uint8_t y,enum Type mode);
void UI_Draw_Parmetr(int value, uint8_t x,uint8_t y,enum Type mode);
void UI_Clear_Line(uint8_t y);
void UI_Set_Cursor(uint8_t x, uint8_t y);
void UI_Draw_String(char *str, uint8_t x, uint8_t y, enum Display_Write_MODE Display_Write_MODE);
void UI_Draw_String_Center(char *str, uint8_t y, enum Display_Write_MODE Display_Write_MODE);
void UI_Change_Value_Ptr(void *parametr, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Encoder *Encoder);
int UI_Change_Value(int parametr, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Encoder *Encoder);
void UI_Encoder_Reset_Rotary_Switch_Buffer(struct Encoder *Encoder);
void UI_Encoder_Reset_Button_State(struct Encoder *Encoder);
//----------------------------------------------------------------------------
#endif /* INC_UI_BASE_FUNC_H_ */
