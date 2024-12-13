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
#include "OLED.h"
#include "Button.h"
#include "PID.h"
#include "Soldering_Station.h"


#define DROW_LIMIT 7

extern struct OLED OLED1;

enum Type{
	INT,
	UINT8,
	UINT16,
	BOOL
};
//----------------------------------------------------------------------------
struct Menu_List_Element_Vector{
	struct Menu_List_Element_Vector *next;
	uint8_t ID;
	char *name;
	enum Type mode;
	uint8_t step;
	void *parametr;
};
//----------------------------------------------------------------------------
struct Menu_List_Vector{
	struct Menu_List_Vector *next;
	struct Menu_List_Element_Vector *Menu_List_Element_Vector;
	uint8_t ID;
	char *name;

};
//----------------------------------------------------------------------------
void Menu_List_Element_Vector_it(struct Menu_List_Element_Vector *self);
//void Menu_List_Element_Create(struct Menu_List_Element_Vector *self, char *str, enum Type mode, uint8_t step, void *value);
void Menu_List_Element_Create(struct Menu_List_Element_Vector *head,struct Menu_List_Element_Vector *ini_element);
void Menu_List_Create(struct Menu_List_Vector *List_Vector,struct Menu_List_Element_Vector *List_Element_Vector, char *str);
void Menu_List_Vector_it(struct Menu_List_Vector *self);
void Menu_it(struct Menu_List_Vector *self);
//----------------------------------------------------------------------------



#endif /* INC_MENU_H_ */
