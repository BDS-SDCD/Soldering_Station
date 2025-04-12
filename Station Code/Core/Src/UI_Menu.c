#include "UI_Menu.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Draw(struct Menu_List_Element_Vector *self, uint8_t y){
	/**
	 * Draw line with name and value of parameter
	 */
	UI_Clear_Line(y);
	UI_Draw_String(self->name, 1, y, Direct_Display_MODE);
	UI_Draw_Parmetr_Ptr(self->parametr, 16, y, self->mode);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Vector_Draw(struct Menu_List_Element_Vector *self, uint8_t start_point, uint8_t quantity){
	/**
	 * Draw all  Menu_List_Element from start point ID of Menu_List_Element_Vector to start point + quantity
	 */
	while(self->ID!=start_point)		//skip to start point
		self=self->next;
	for(uint8_t i=0;i<quantity;i++){
		if(self!=NULL){																//if Menu_List_Element was created
			Menu_List_Element_Draw(self, i+Menu_List_Element_Vector_Draw_Shift);	//draw it based on position
			self=self->next;
		}
		else																		//else
			UI_Clear_Line(i+Menu_List_Element_Vector_Draw_Shift);					//clear it's based on line position
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Menu_List_Vector_Size(struct Menu_List_Element_Vector *self){
	/**
	 * Return quantity of elements in  Menu_List_Element_Vector
	 */
	uint8_t MAX_ID=0;
	while(self->next!=NULL){
			self=self->next;
			MAX_ID++;
	}
	return MAX_ID++;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Get_Prescaler(struct Button_Vector *Button_Vector){
	/**
	 *	Return number based on witch buttons are pressed
	 *	x1 x2 x5 x10
	 */
	struct Button_Vector *now=Button_Vector;

	static  uint8_t Button_State=0;											//Every bit of Button_State contain local button state (Turn OFF or ON)
	uint8_t Prescaler=1;
	do{
		if(Button_Get_Event_State(now->Button)){							//If Button Event
			switch(now->Button->ID){										//Check button ID
				case X2_Modefire_Button_ID:
					if(Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET){	//If Falling edge
						Button_State^=0x02;									//Toggle state bit
					}
				break;
				case X5_Modefire_Button_ID:
					if(Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET){	//If Falling edge
						Button_State^=0x01;									//Toggle state bit
					}
				break;
			}
		}
		now=now->next;														//Go to next button
	}while(now!=NULL);

	if((Button_State&0x02)!=0)												//Set value based on Button_State
		Prescaler*=2;
	if((Button_State&0x01)!=0)
		Prescaler*=5;

	return Prescaler;														//Return value
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Value_Change(struct Menu_List_Element_Vector *self, uint8_t y, struct Encoder *Encoder, struct Button_Vector *Button_Vector){
	/**
	 *	Changing value of Menu_List_Element and drawing it's value until encoder Button state not Encoder_Button_long
	 */
	UI_Encoder_Reset_Button_State(Encoder);

	UI_Set_Cursor(Menu_List_Element_Vector_Cursor_x-1, y);																//SET CURSOR ON VALUE
	while(Encoder_Get_Button_State(Encoder)!=Encoder_Button_long){
		UI_Change_Value_Ptr(self->parametr, (self->step)*Get_Prescaler(Button_Vector), self->mode, 0, 65535, Encoder);	//Changing value
		UI_Draw_Parmetr_Ptr(self->parametr, Menu_List_Element_Vector_Cursor_x, y, self->mode);							//Draw value
	}
	UI_Encoder_Reset_Button_State(Encoder);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Menu_List_Element_Vector_it(struct Menu_List_Element_Vector *self, char *Name, struct Encoder *Encoder, struct Button_Vector *Button_Vector){
	/**
	 * 	Display name, value of each element of current vector and can change value based on Type..... until Encoder button != Encoder_Button_long
	 *
	 */

	int Menu_Count=0,Menu_Count_Prew=255;				//Menu_Count current menu options, Menu_Count_Prew=255 needed to draw menu at first iteration
	uint8_t MAX_ID=Menu_List_Vector_Size(self);			//Get max size of vector

	UI_Encoder_Reset_Button_State(Encoder);				//Reset encoder button state
	UI_Draw_String_Center(Name, 0, Inverse_Display_MODE);	//Draw Submenu name
	UI_Set_Cursor(0, Menu_Count% (Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift)+Menu_List_Element_Vector_Draw_Shift);	//Set cursor to first element based on shift

	while(Encoder_Get_Button_State(Encoder)!=Encoder_Button_long){	//Main body which	contain main logic work with Menu_List_Element_Vector

		Menu_Count+= Encoder_Get_Rotary_Switch_Buffer(Encoder);		//Get Menu_Count based on Rotary_Switch_Buffer
		Encoder_Reset_Rotary_Switch_Buffer(Encoder);				//Reset  Rotary_Switch_Buffer

		if(Menu_Count<0)											//Check if current Menu_Count is more than 0 and less than size of vector
			Menu_Count=0;
		else if(Menu_Count>MAX_ID)
			Menu_Count=MAX_ID;

		if(Menu_Count_Prew!=Menu_Count){							//if Menu_Count was changed
			if(((Menu_Count/(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift))!=((Menu_Count_Prew/(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift)))
				// if Menu_Count more than quantity of rows on display than redraw new page with next names and values based on vector
				Menu_List_Element_Vector_Draw(self, (uint8_t)(Menu_Count/(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift), Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift);
			UI_Set_Cursor(0, Menu_Count% (Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift)+Menu_List_Element_Vector_Draw_Shift);	//Change cursor position every time
			Menu_Count_Prew=Menu_Count;
		}

		if(Encoder_Get_Button_State(Encoder)==Encoder_Button_Short){	//If encoder button was pressed
			struct Menu_List_Element_Vector *now;
			now=self;
			while(now->ID!=Menu_Count){				//skip to Menu_List_Element which ID is equal to Menu_Count
				now=now->next;
			}
			Menu_List_Element_Value_Change(now, now->ID% (Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift)+Menu_List_Element_Vector_Draw_Shift, Encoder, Button_Vector);
			//change value
			UI_Set_Cursor(0, Menu_Count% (Draw_LIMIT-Menu_List_Element_Vector_Draw_Shift)+Menu_List_Element_Vector_Draw_Shift);	//Return cursor
			UI_Encoder_Reset_Button_State(Encoder);		//Reset encoder button state
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Menu_List_Element_Create(struct Menu_List_Element_Vector *head,struct Menu_List_Element_Vector *ini_element){
	/**
	 * Add a new element to enu_List_Element_Vector based on copy of ini_element
	 */
	char *st= malloc((sizeof(char)*(strlen(ini_element->name)+1)));
	strcpy(st,ini_element->name);
	if(head->parametr==NULL){				//If Head have not any data
		*head=*ini_element;					//Head
		head->ID=0;
		head->next=NULL;
	}
	else{									//else
		struct Menu_List_Element_Vector *now=head;
		struct Menu_List_Element_Vector *NEW_Element= malloc(sizeof(struct Menu_List_Element_Vector));	//Create new element
		*NEW_Element=*ini_element;																		//Copy data to the new element
		NEW_Element->ID=head->ID;
		NEW_Element->next=NULL;
		while(now->next!=NULL){																			//skip head to last element
			now=now->next;
			NEW_Element->ID++;
		}
		NEW_Element->ID++;
		now->next=NEW_Element;																			//pointer to a next element pointing to a new element
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Menu_List_Size(struct Menu_List_Vector *self){
	/**
	 * Return quantity of elements in  Menu_List_Vector
	 */
	uint8_t MAX_ID=0;
	while(self->next!=NULL){
			self=self->next;
			MAX_ID++;
	}
	return MAX_ID++;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Vector_Draw(struct Menu_List_Vector *self, uint8_t start_point, uint8_t quantity){
	/**
	 * Draw all names of submenu based on  Menu_List_Vector and clear all other lines
	 */
	while(self->ID!=start_point)						//skip to statr point
		self=self->next;
	for(uint8_t i=1;i<Menu_List_Vector_Draw_Shift;i++){//Clear all lined before Menu_List_Vector. Line 0 is NAME of MENU
		UI_Clear_Line(i);
	}
	for(uint8_t i=Menu_List_Vector_Draw_Shift;i<quantity+Menu_List_Vector_Draw_Shift;i++){						//Draw all submenu's names and clear all other lines
		if(self!=NULL){
			UI_Clear_Line(i);
			UI_Draw_String_Center(self->name, i, Direct_Display_MODE);
			self=self->next;
		}
		else
			UI_Clear_Line(i);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Set_List_Cursor(struct Menu_List_Vector *self, uint8_t start_point, uint8_t quantity, uint8_t cursor_y){
	/**
	 *	Draw all names of submenu based on  Menu_List_Vector and inverse NAME output on cursor_y
	 */
	while(self->ID!=start_point)			//skip to start point
		self=self->next;
	for(uint8_t i=0;i<quantity;i++){		//Draw all Submenu titles
		if(self!=NULL){
			if(i==cursor_y)					//if selected
				UI_Draw_String_Center(self->name, i+Menu_List_Vector_Draw_Shift, Inverse_Display_MODE);	//display title inverse
			else																						//else
				UI_Draw_String_Center(self->name, i+Menu_List_Vector_Draw_Shift, Direct_Display_MODE);	//display title direct
			self=self->next;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Menu_it(struct Menu_List_Vector *self, struct Encoder *Encoder, struct Button_Vector *Button_Vector){

	int Menu_Count=0, Menu_Count_Prew=255;		//Menu_Count current menu options, Menu_Count_Prew=255 needed to draw menu at first iteration

	uint8_t MAX_ID=Menu_List_Size(self);		//Get max size of vector
	UI_Encoder_Reset_Button_State(Encoder);		//Reset encoder button state

	UI_Clear_Line(0);							//Clear line for Menu Title
	UI_Draw_String_Center("MENU", 0, Direct_Display_MODE);	//Draw Menu Title
	Menu_List_Vector_Draw(self, (uint8_t)(Menu_Count/(Draw_LIMIT-Menu_List_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Vector_Draw_Shift), Draw_LIMIT-Menu_List_Vector_Draw_Shift);
															//Draw Menu subtitles based on Menu_List_Vector

	while(Encoder_Get_Button_State(Encoder)!=Encoder_Button_long){	//main cycle

		Menu_Count+= Encoder_Get_Rotary_Switch_Buffer(Encoder);		//Get Menu_Count based on Rotary_Switch_Buffer
		Encoder_Reset_Rotary_Switch_Buffer(Encoder);				//Reset encoder button state

		if(Menu_Count<0)											//Check if current Menu_Count is more than 0 and less than size of vector
			Menu_Count=0;
		else if(Menu_Count>MAX_ID)
			Menu_Count=MAX_ID;

		if(Menu_Count_Prew!=Menu_Count){							//if Menu_Count was changed
			Menu_Set_List_Cursor(self,								//Redraw cursor and subtitle list
					(uint8_t)(Menu_Count/(Draw_LIMIT-Menu_List_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Vector_Draw_Shift),
					Draw_LIMIT-Menu_List_Vector_Draw_Shift,
					(uint8_t)Menu_Count%(Draw_LIMIT-Menu_List_Vector_Draw_Shift));
			Menu_Count_Prew=Menu_Count;
		}

		if(Encoder_Get_Button_State(Encoder)==Encoder_Button_Short){   								//Element select
			struct Menu_List_Vector *now;
			now=self;
			while(now->ID!=Menu_Count){
				now=now->next;																//Shift Element pointer to select element
			}
			UI_Menu_List_Element_Vector_it(now->Menu_List_Element_Vector, self->name, Encoder, Button_Vector);		//Transceive control to Menu_List_Element_Vector menu
																													//Area after resceive control start
			UI_Clear_Line(0);										//Clear line for Menu Title
			UI_Draw_String_Center("MENU", 0, Direct_Display_MODE);	//Draw Menu Title
			Menu_List_Vector_Draw(self, (uint8_t)(Menu_Count/(Draw_LIMIT-Menu_List_Vector_Draw_Shift))*(Draw_LIMIT-Menu_List_Vector_Draw_Shift), Draw_LIMIT-Menu_List_Vector_Draw_Shift);
			Menu_Count_Prew=255;									//Reset state
			UI_Encoder_Reset_Button_State(Encoder);					//Reset encoder button state
																	//Area after resceive control end
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UI_Menu_List_Create(struct Menu_List_Vector *List_Vector, struct Menu_List_Element_Vector *List_Element_Vector, char *name){
	/**
	 * Add a new element to List_Vector based on copy of List_Element_Vector
	 */
	char *st= malloc((sizeof(char)*(strlen(name)+1)));			//Lock memory for name str
	strcpy(st,name);											//Copy string
	if(List_Vector->Menu_List_Element_Vector==NULL){			//If Head have not any data
		List_Vector->ID=0;
		List_Vector->name=st;
		List_Vector->Menu_List_Element_Vector=List_Element_Vector;
	}
	else{
		struct Menu_List_Vector *now=List_Vector;
		struct Menu_List_Vector *NEW_Element=malloc(sizeof(struct Menu_List_Vector));	//create new element
		NEW_Element->ID=List_Vector->ID;
		NEW_Element->next=NULL;
		NEW_Element->name=st;
		NEW_Element->Menu_List_Element_Vector=List_Element_Vector;						//Copy data to the new element
		while(now->next!=NULL){															//Skip until current pointer is'not tail
			now=now->next;
			NEW_Element->ID++;
		}
		NEW_Element->ID++;
		now->next=NEW_Element;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
















