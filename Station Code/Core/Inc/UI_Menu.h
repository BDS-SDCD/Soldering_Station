/*
 * menu.h
 *
 *  Created on: Aug 23, 2024
 *      Author: I
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "Flash_Custom.h"
#include "stm32f1xx.h"
#include "string.h"
#include "stdlib.h"
#include "UI_Base_Func.h"
#include "Soldering_Station.h"




#define Draw_LIMIT 8

#define Menu_List_Element_Vector_Draw_Shift 1
#define Menu_List_Element_Vector_Cursor_x 16

#define Menu_List_Vector_Draw_Shift 2

//----------------------------------------------------------------------------
/**
 *	 Menu_List_Vector->Menu_List_Element_Vector-> Menu_List_Element_Vector->Menu_List_Element_Vector.....
 *	 		|
 *	 	   \/
 *	 Menu_List_Vector->Menu_List_Element_Vector-> Menu_List_Element_Vector->Menu_List_Element_Vector.....
 *	 		|
 *	 	   \/
 *	 Menu_List_Vector->Menu_List_Element_Vector-> Menu_List_Element_Vector->Menu_List_Element_Vector.....
 *	 		|
 *	 	   \/
 *	 Menu_List_Vector->Menu_List_Element_Vector-> Menu_List_Element_Vector->Menu_List_Element_Vector.....
 *	 		|
 *	 	   \/
 *	 	  ....
 *
 *	Menu_List_Vector contain name of Menu_List_Element_Vector and pointer to Menu_List_Element_Vector.
 *		Based on Menu_List_Vector UI Menu drawing all menu titles of each Menu_List_Element_Vector and enter into submenu with variables based on Menu_List_Element_Vector.
 *	Every Menu_List_Element_Vector contain all necessary data to correct single variable (data interpretation to draw and change value).
 *
 */
//----------------------------------------------------------------------------
struct Menu_List_Element_Vector{
	struct Menu_List_Element_Vector *next;
	uint8_t ID;								//Menu_List_Element_Vector's ID
	char *name;								//Prameter's name. Needed to draw a prameter's name
	enum Type mode;							//Data_Type	Type must be the same as  declarated variable. Needed to correct data interpretation to draw ro change value
	uint8_t step;							//Step of value changing
	void *parametr;							//Pointer to a declarated variable
};
//----------------------------------------------------------------------------
struct Menu_List_Vector{
	struct Menu_List_Vector *next;
	struct Menu_List_Element_Vector *Menu_List_Element_Vector;
	uint8_t ID;
	char *name;													//name of submenu

};
//----------------------------------------------------------------------------
void UI_Menu_List_Element_Vector_it(struct Menu_List_Element_Vector *self, char *Name, struct Encoder *Encoder, struct Button_Vector *Button_Vector);
void UI_Menu_List_Element_Create(struct Menu_List_Element_Vector *head,struct Menu_List_Element_Vector *ini_element);
void UI_Menu_List_Create(struct Menu_List_Vector *List_Vector,struct Menu_List_Element_Vector *List_Element_Vector, char *name);
void UI_Menu_it(struct Menu_List_Vector *self, struct Encoder *Encoder, struct Button_Vector *Button_Vector);
//----------------------------------------------------------------------------



#endif /* INC_MENU_H_ */
