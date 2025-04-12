#include "UI_Base_Func.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Draw_Parmetr_Ptr(void *value, uint8_t x, uint8_t y, enum Type mode){
	/**
	 *	Displays the value on the display based on the variable pointer, type of variable and the x y position
	 *	Types INT, UIT8, UINT16, BOOL
	 */
	switch(mode){
		case INT:
			int num_int=*(int*)value;
			if(num_int<0){
				OLED_Set_Char_Cursor(&OLED1,x-1,y);
				OLED_Send_Char(&OLED1,'-');
			}
			OLED_Set_Char_Cursor(&OLED1,x-1,y);
			if(num_int<0){
				OLED_Send_Char(&OLED1,'-');
				num_int=abs(num_int);
			}
			else
				OLED_Send_Char(&OLED1,' ');
			OLED_Send_Char(&OLED1,(num_int/1000)%10+'0');
			OLED_Send_Char(&OLED1,(num_int/100)%10+'0');
			OLED_Send_Char(&OLED1,(num_int/10)%10+'0');
			OLED_Send_Char(&OLED1,(num_int)%10+'0');
		break;
		case UINT8:
			uint8_t *num_8=value;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			OLED_Send_Char(&OLED1,(*num_8/1000)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8/100)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8/10)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8)%10+'0');
		break;
		case UINT16:
			uint16_t *num_16=value;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			OLED_Send_Char(&OLED1,(*num_16/1000)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16/100)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16/10)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16)%10+'0');
		break;
		case BOOL:
			uint8_t *num=value;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			if(*num==1){
				OLED_Send_String(&OLED1,"ON ", Direct_Display_MODE);
			}else
				OLED_Send_String(&OLED1,"OFF", Direct_Display_MODE);
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Draw_Parmetr(int value, uint8_t x,uint8_t y,enum Type mode){
	/**
	 * Displays the value on the display based on the variable, type of variable and the x y position
	 */
	UI_Draw_Parmetr_Ptr(&value, x, y, mode);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Draw_String(char *str, uint8_t x, uint8_t y, enum Display_Write_MODE Display_Write_MODE){
	/**
	 * Displays the string on the display based x y position and Display_Write_MODE(direct inverse)
	 */
	OLED_Set_Char_Cursor(&OLED1,x,y);					//Sets position on display from what will be display based on ASII symbols size
	OLED_Send_String(&OLED1, str, Display_Write_MODE);	//Display string on the Display

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Set_Cursor(uint8_t x, uint8_t y){
	/**
	 * Set symbol cursor(>) based on x, y position and clear prew cursor position
	 */
	static uint8_t Prew_Pos_x=0,Prew_Pos_y=0;
		OLED_Set_Char_Cursor(&OLED1,Prew_Pos_x,Prew_Pos_y);
		OLED_Send_Char(&OLED1,' ');
		OLED_Set_Char_Cursor(&OLED1,x,y);
		OLED_Send_Char(&OLED1,'>');
		Prew_Pos_x=x;
		Prew_Pos_y=y;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Clear_Line(uint8_t y){
	/**
	 * Clear row on display based on y position
	 */
	OLED_Clear_Line(&OLED1,y);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Change_Value_Ptr(void *parametr, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Encoder *Encoder){
	/**
	 * Change the value based on the variable pointer, step, type of variable and encoder buffer
	 */
	switch(mode){
		case INT:
			*(int*)parametr+=Encoder_Get_Rotary_Switch_Buffer(Encoder)*step;
		break;
		case UINT8:
			if((*(uint8_t*)parametr+Encoder_Get_Rotary_Switch_Buffer(Encoder)*step)<min)
				*(uint8_t*)parametr=(uint8_t)min;
			else if ((*(uint8_t*)parametr+Encoder_Get_Rotary_Switch_Buffer(Encoder)*step)>max)
				*(uint8_t*)parametr=(uint8_t)max;
			else
				*(uint8_t*)parametr+=(uint8_t)Encoder_Get_Rotary_Switch_Buffer(Encoder)*step;
		break;
		case UINT16:
			if((*(uint16_t*)parametr+Encoder_Get_Rotary_Switch_Buffer(Encoder)*step)<min)
				*(uint16_t*)parametr=(uint16_t)min;
			else if ((*(uint16_t*)parametr+Encoder_Get_Rotary_Switch_Buffer(Encoder)*step)>max)
				*(uint16_t*)parametr=(uint16_t)max;
			else
				*(uint16_t*)parametr+=(uint16_t)Encoder_Get_Rotary_Switch_Buffer(Encoder)*step;
		break;
		case BOOL:
			if(Encoder_Get_Rotary_Switch_Buffer(Encoder)>0)
				*(uint8_t*)parametr=1;
			else if(Encoder_Get_Rotary_Switch_Buffer(Encoder)<0)
				*(uint8_t*)parametr=0;
		break;
	}
	Encoder_Reset_Rotary_Switch_Buffer(Encoder);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
int UI_Change_Value(int parametr, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Encoder *Encoder){
	/**
	 * Change the value based on the variable, step, type of variable and encoder buffer
	 * return Changed Variable
	 */
	UI_Change_Value_Ptr(&parametr, step, mode, min, max, Encoder);
	return parametr;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Draw_String_Center(char *str, uint8_t y, enum Display_Write_MODE Display_Write_MODE){
	/**
	 * Draw string in center of row based on row position and Display_Write_MODE
	 */
	uint8_t str_len =strlen(str);						//Get string length
	uint8_t center= (OLED_Colume_Number-str_len*6)/2;	//Counting start position x to draw string
	OLED_Set_Cursor(&OLED1, center, y);					//Sets position on display from what will be display
	OLED_Send_String(&OLED1, str, Display_Write_MODE);	//Display string on display
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Encoder_Reset_Rotary_Switch_Buffer(struct Encoder *Encoder){
	/**
	 * Reset Encoder_Rotary switch buffer
	 */
	Encoder_Reset_Rotary_Switch_Buffer(Encoder);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Encoder_Reset_Button_State(struct Encoder *Encoder){
	/**
	 * Reset Encoder_Rotary button buffer
	 */
	Encoder_Reset_Button_State(Encoder);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
