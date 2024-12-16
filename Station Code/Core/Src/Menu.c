/*
 * menu.c
 *
 *  Created on: Aug 23, 2024
 *      Author: I
 */
#include "Menu.h"


extern struct Soldering_Iron Soldering_Iron;
extern struct Soldering_Heat_Gun Soldering_Heat_Gun;
extern struct Soldering_Separator Soldering_Separator;
extern struct ZCD ZCD;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Parmetr_Drow(void *num_void, uint8_t x,uint8_t y,enum Type mode){
	switch(mode){
		case INT:
			int num_int=*(int*)num_void;
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
			uint8_t *num_8=num_void;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			OLED_Send_Char(&OLED1,(*num_8/1000)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8/100)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8/10)%10+'0');
			OLED_Send_Char(&OLED1,(*num_8)%10+'0');
		break;
		case UINT16:
			uint16_t *num_16=num_void;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			OLED_Send_Char(&OLED1,(*num_16/1000)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16/100)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16/10)%10+'0');
			OLED_Send_Char(&OLED1,(*num_16)%10+'0');
		break;
		case BOOL:
			uint8_t *num=num_void;
			OLED_Set_Char_Cursor(&OLED1,x,y);
			if(*num==1){
				OLED_Send_String(&OLED1,"ON ");
			}else
				OLED_Send_String(&OLED1,"OFF");
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_String_Drow(char *str, uint8_t x, uint8_t y){
	OLED_Set_Char_Cursor(&OLED1,x,y);
	OLED_Send_String(&OLED1, str);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Drow(struct Menu_List_Element_Vector *self, uint8_t y){
	OLED_Clear_Line(&OLED1,y);
	Menu_String_Drow(self->name, 1, y);
	Menu_Parmetr_Drow(self->parametr, 16, y, self->mode);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_Center_String_Drow(char *str, uint8_t y,uint8_t *mas){
	if(mas!=NULL){
		*mas =strlen(str);
		*(mas+1)=(21-*mas)/2;
		OLED_Clear_Line(&OLED1,y);
		Menu_String_Drow(str, *(mas+1), y);
	}
	else{
		OLED_Clear_Line(&OLED1,y);
		Menu_String_Drow(str, (21-strlen(str))/2, y);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Vector_Drow(struct Menu_List_Element_Vector *self, uint8_t start_point, uint8_t quantity,uint8_t Ress){
	static uint8_t  PRW_Start_Point=254;
	if(Ress==1)
		PRW_Start_Point=254;
	if(start_point!=PRW_Start_Point){
		while(self->ID!=start_point)
			self=self->next;
		for(uint8_t i=0;i<quantity;i++){
			if(self!=NULL){
				Menu_List_Element_Drow(self, i+1);
				self=self->next;
			}
			else
				OLED_Clear_Line(&OLED1,i+1);
		}
		PRW_Start_Point=start_point;
	}

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Menu_List_Vector_Size(struct Menu_List_Element_Vector *self){
	uint8_t MAX_ID=0;
	while(self->next!=NULL){
			self=self->next;
			MAX_ID++;
	}
	return MAX_ID++;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void OLED_Set_Menu_Cursor(struct OLED *self, uint8_t x, uint8_t y,uint8_t Ress){
	static uint8_t Prew_Pos_x=0,Prew_Pos_y=254;
	if(Ress==1)
		Prew_Pos_y=254;
	if(Prew_Pos_x!=x||Prew_Pos_y!=y){
		OLED_Set_Char_Cursor(&OLED1,Prew_Pos_x,Prew_Pos_y);
		OLED_Send_Char(&OLED1,' ');
		OLED_Set_Char_Cursor(&OLED1,x,y);
		OLED_Send_Char(&OLED1,'>');
		Prew_Pos_x=x;
		Prew_Pos_y=y;
	}
}

void Value_Changing(void *parametr, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Incoder *Incoder){
	switch(mode){
		case INT:
			*(int*)parametr+=Incoder_Get_Rotary_Switch_Buffer(Incoder)*step;
		break;
		case UINT8:
			if((*(uint8_t*)parametr+Incoder_Get_Rotary_Switch_Buffer(Incoder)*step)<min)
				*(uint8_t*)parametr=(uint8_t)min;
			else if ((*(uint8_t*)parametr+Incoder_Get_Rotary_Switch_Buffer(Incoder)*step)>max)
				*(uint8_t*)parametr=(uint8_t)max;
			else
				*(uint8_t*)parametr+=(uint8_t)Incoder_Get_Rotary_Switch_Buffer(Incoder)*step;
		break;
		case UINT16:
			if((*(uint16_t*)parametr+Incoder_Get_Rotary_Switch_Buffer(Incoder)*step)<min)
				*(uint16_t*)parametr=(uint16_t)min;
			else if ((*(uint16_t*)parametr+Incoder_Get_Rotary_Switch_Buffer(Incoder)*step)>max)
				*(uint16_t*)parametr=(uint16_t)max;
			else
				*(uint16_t*)parametr+=(uint16_t)Incoder_Get_Rotary_Switch_Buffer(Incoder)*step;
		break;
		case BOOL:
			if(Incoder_Get_Rotary_Switch_Buffer(Incoder)>0)
				*(uint8_t*)parametr=1;
			else if(Incoder_Get_Rotary_Switch_Buffer(Incoder)<0)
				*(uint8_t*)parametr=0;
		break;
	}
	Incoder_Reset_Rotary_Switch_Buffer(Incoder);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Value_Change(void *parametr, uint8_t x, uint8_t y, uint8_t step, enum Type mode, uint16_t min, uint16_t max, struct Incoder *Incoder){

	Value_Changing(parametr, step, mode,  min, max, Incoder);
	Menu_Parmetr_Drow(parametr, x, y,mode);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Value_Change(struct Menu_List_Element_Vector *self, uint8_t y, struct Incoder *Incoder){
	Incoder_Reset_Button_State(Incoder);
	OLED_Set_Menu_Cursor(&OLED1,15, y,0);
	while(Incoder_Get_Button_State(Incoder)!=Incoder_Button_long){
		Value_Change(self->parametr, 16, y, self->step, self->mode, 0, 65535, Incoder);
	}
	Incoder_Reset_Button_State(Incoder);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Vector_it(struct Menu_List_Element_Vector *self, struct Incoder *Incoder){
	int Menu_Count=0;
	uint8_t MAX_ID=Menu_List_Vector_Size(self);
	Incoder_Reset_Button_State(Incoder);
	Menu_List_Element_Vector_Drow(self, (uint8_t)(Menu_Count/DROW_LIMIT)*DROW_LIMIT, DROW_LIMIT,1);
	OLED_Set_Menu_Cursor(&OLED1, 0, Menu_Count% DROW_LIMIT+1,1);

	while(Incoder_Get_Button_State(Incoder)!=Incoder_Button_long){

		Menu_Count+= Incoder_Get_Rotary_Switch_Buffer(Incoder);
		Incoder_Reset_Rotary_Switch_Buffer(Incoder);

		if(Menu_Count<0)
			Menu_Count=0;
		else if(Menu_Count>MAX_ID)
			Menu_Count=MAX_ID;

		Menu_List_Element_Vector_Drow(self, (uint8_t)(Menu_Count/DROW_LIMIT)*DROW_LIMIT, DROW_LIMIT,0);
		OLED_Set_Menu_Cursor(&OLED1, 0, Menu_Count% DROW_LIMIT+1,0);

		if(Incoder_Get_Button_State(Incoder)==Incoder_Button_Short){
			struct Menu_List_Element_Vector *now;
			now=self;
			while(now->ID!=Menu_Count){
				now=now->next;
			}
			Menu_List_Element_Value_Change(now,  now->ID% DROW_LIMIT+1,Incoder);
			OLED_Set_Menu_Cursor(&OLED1, 0, Menu_Count% DROW_LIMIT+1,1);
			Incoder_Reset_Button_State(Incoder);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Element_Create(struct Menu_List_Element_Vector *head,struct Menu_List_Element_Vector *ini_element){
	char *st= malloc((sizeof(char)*(strlen(ini_element->name)+1)));
	strcpy(st,ini_element->name);
	if(head->parametr==NULL){
		*head=*ini_element;
		head->ID=0;
		head->next=NULL;
	}
	else{
		struct Menu_List_Element_Vector *now=head;
		struct Menu_List_Element_Vector *NEW_Element= malloc(sizeof(struct Menu_List_Element_Vector));
		*NEW_Element=*ini_element;
		NEW_Element->ID=head->ID;
		NEW_Element->next=NULL;
		while(now->next!=NULL){
			now=now->next;
			NEW_Element->ID++;
		}
		NEW_Element->ID++;
		now->next=NEW_Element;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Menu_List_Size(struct Menu_List_Vector *self){
	uint8_t MAX_ID=0;
	while(self->next!=NULL){
			self=self->next;
			MAX_ID++;
	}
	return MAX_ID++;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Vector_Drow(struct Menu_List_Vector *self, uint8_t start_point, uint8_t quantity,uint8_t Ress ,uint8_t *mas){
	static uint8_t  PRW_Start_Point=254;
	if(Ress==1)
		PRW_Start_Point=254;
	if(start_point!=PRW_Start_Point){
		while(self->ID!=start_point)
			self=self->next;
		for(uint8_t i=0;i<quantity;i++){
			if(self!=NULL){
				Menu_Center_String_Drow(self->name, i+1, (mas+(i*2)));
				self=self->next;
			}
			else
				OLED_Clear_Line(&OLED1,i+1);
		}
		PRW_Start_Point=start_point;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void OLED_Set_Menu_List_Cursor(struct OLED *self, uint8_t x, uint8_t y,uint8_t Lens, uint8_t Ress){
	static uint8_t Prew_Pos_x=0,Prew_Pos_y=254, Prew_Lens=0;
	if(Ress==1)
		Prew_Pos_y=254;
	if(Prew_Pos_x!=x||Prew_Pos_y!=y){
		OLED_Set_Char_Cursor(&OLED1,Prew_Pos_x-2,Prew_Pos_y);
		OLED_Send_Char(&OLED1,' ');
		if(Prew_Lens!=0);
		OLED_Set_Char_Cursor(&OLED1,Prew_Pos_x+Prew_Lens+1,Prew_Pos_y);
		OLED_Send_Char(&OLED1,' ');

		OLED_Set_Char_Cursor(&OLED1,x-2,y);
		OLED_Send_Char(&OLED1,'>');
		OLED_Set_Char_Cursor(&OLED1,x+Lens+1,y);
		OLED_Send_Char(&OLED1,'<');

		Prew_Pos_x=x;
		Prew_Pos_y=y;
		Prew_Lens=Lens;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Vector_it(struct Menu_List_Vector *self, struct Incoder *Incoder){
	int Menu_Count=0;
	uint8_t mas[15]; // 0 Length 1-st str; 1 start position 1-st str; 2 lenth 1-st str; 3 start postition 2-st str.......


	uint8_t MAX_ID=Menu_List_Size(self);
	Incoder_Reset_Button_State(Incoder);
	Menu_Center_String_Drow("MENU",0,NULL);
	Menu_List_Vector_Drow(self, (uint8_t)(Menu_Count/DROW_LIMIT)*DROW_LIMIT, DROW_LIMIT,1, &mas[0]);
	OLED_Set_Menu_List_Cursor(&OLED1, mas[1], Menu_Count% DROW_LIMIT+1,mas[0],1);

	while(Incoder_Get_Button_State(Incoder)!=Incoder_Button_long){

		Menu_Count+= Incoder_Get_Rotary_Switch_Buffer(Incoder);
		Incoder_Reset_Rotary_Switch_Buffer(Incoder);

		if(Menu_Count<0)
			Menu_Count=0;
		else if(Menu_Count>MAX_ID)
			Menu_Count=MAX_ID;

		Menu_List_Vector_Drow(self, (uint8_t)(Menu_Count/DROW_LIMIT)*DROW_LIMIT, DROW_LIMIT,0,&mas[0]);
		OLED_Set_Menu_List_Cursor(&OLED1, mas[(Menu_Count% DROW_LIMIT)*2+1], Menu_Count% DROW_LIMIT+1,mas[(Menu_Count% DROW_LIMIT)*2],0);

		if(Incoder_Get_Button_State(Incoder)==Incoder_Button_Short){   								//Element select
			struct Menu_List_Vector *now;
			now=self;
			while(now->ID!=Menu_Count){
				now=now->next;																//Shift Element pointer to select element
			}

			Menu_Center_String_Drow(now->name,0,NULL);										//Draw name of element menu
			Menu_List_Element_Vector_it(now->Menu_List_Element_Vector, Incoder);						//Transceive control to Menu_List_Element_Vector menu
																							//Area after resceive control start

			Soldering_Station_Write_Struct(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);

			Menu_Center_String_Drow("MENU",0,NULL);
			Menu_List_Vector_Drow(self, (uint8_t)(Menu_Count/DROW_LIMIT)*DROW_LIMIT, DROW_LIMIT,1, &mas[0]);
			OLED_Set_Menu_List_Cursor(&OLED1, mas[(Menu_Count% DROW_LIMIT)*2+1], Menu_Count% DROW_LIMIT+1,mas[(Menu_Count% DROW_LIMIT)*2],1);
			Incoder_Reset_Button_State(Incoder);
																							//Area after resceive control end
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_List_Create(struct Menu_List_Vector *List_Vector,struct Menu_List_Element_Vector *List_Element_Vector, char *str){
	char *st= malloc((sizeof(char)*(strlen(str)+1)));
	strcpy(st,str);
	if(List_Vector->Menu_List_Element_Vector==NULL){
		List_Vector->ID=0;
		List_Vector->name=st;
		List_Vector->Menu_List_Element_Vector=List_Element_Vector;
	}
	else{
		struct Menu_List_Vector *now=List_Vector;
		struct Menu_List_Vector *NEW_Element=malloc(sizeof(struct Menu_List_Vector));
		NEW_Element->ID=List_Vector->ID;
		NEW_Element->next=NULL;
		NEW_Element->name=st;
		NEW_Element->Menu_List_Element_Vector=List_Element_Vector;
		while(now->next!=NULL){
			now=now->next;
			NEW_Element->ID++;
		}
		NEW_Element->ID++;
		now->next=NEW_Element;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void start_Page_Drow(){
	OLED_Clear_Line(&OLED1,0);
	Menu_String_Drow("SoldIron", 1, 0);
	OLED_Clear_Line(&OLED1,1);
	Menu_String_Drow("State: ", 0, 1);
	OLED_Clear_Line(&OLED1,2);
	Menu_String_Drow("TSetC:", 0, 2);
	OLED_Clear_Line(&OLED1,3);
	Menu_String_Drow("TNowC:", 0, 3);
	OLED_Clear_Line(&OLED1,4);
	Menu_String_Drow("AdPow:", 0, 4);
	OLED_Clear_Line(&OLED1,5);


	Menu_String_Drow("Freq:", 0, 5);
	OLED_Clear_Line(&OLED1,6);
	Menu_String_Drow("Count:", 0, 6);


	Menu_String_Drow("SoldFAN", 12, 0);
	Menu_String_Drow("State: ", 11, 1);
	Menu_String_Drow("TSetC:", 11, 2);
	Menu_String_Drow("TNowC:", 11, 3);

	Menu_String_Drow("Separator", 12, 4);
	Menu_String_Drow("State: ", 11, 5);
	Menu_String_Drow("TSetC:", 11, 6);
	Menu_String_Drow("TNowC:", 11, 7);

}
//---------------------------------------------------------------------------------Button_Handler
void Button_Handler(struct Button* self){
	switch(self->ID){
		case Button_ID_E2B1:
			if((Button_Get_Pin_State(self)==GPIO_PIN_RESET)&&(Soldering_Iron.State==1)){
				Solder_Iron_Sleep_Time_Resset(&Soldering_Iron);
				Solder_Iron_Set_Temperature(&Soldering_Iron,PRESSET2);
			}
			break;
		case Button_ID_E2B2:
			if(Button_Get_Pin_State(self)==GPIO_PIN_RESET){
				Soldering_Heat_Gun_Set_Temperature(&Soldering_Heat_Gun,PRESSET2);
			}

			break;
		case Button_ID_E2B3:
			if(Button_Get_Pin_State(self)==GPIO_PIN_RESET)
				Soldering_Separator_Set_Temperature(&Soldering_Separator, PRESSET2);

			break;
		case Button_ID_E1B1:
			if((Button_Get_Pin_State(self)==GPIO_PIN_RESET)&&(Soldering_Iron.State==1)){
				Solder_Iron_Sleep_Time_Resset(&Soldering_Iron);
				Solder_Iron_Set_Temperature(&Soldering_Iron, PRESSET1);
			}
			break;
		case Button_ID_E1B2:
			if(Button_Get_Pin_State(self)==GPIO_PIN_RESET){
				Soldering_Heat_Gun_Set_Temperature(&Soldering_Heat_Gun, PRESSET1);
			}

			break;
		case Button_ID_E1B3:
			if(Button_Get_Pin_State(self)==GPIO_PIN_RESET)
				Soldering_Separator_Set_Temperature(&Soldering_Separator, PRESSET1);
			break;
		case Button_Gerkon_ID:
			if(Button_Get_Pin_State(self)==GPIO_PIN_RESET)
				Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
			else
				Soldering_Heat_Gun_ON(&Soldering_Heat_Gun);

			break;
		case Full_Power_Button_ID:
			if(Soldering_Iron.State==1){
				if(Button_Get_Pin_State(self)==GPIO_PIN_RESET){
					Soldering_Iron.Full_Power_State=1;
				}else{
					Soldering_Iron.Full_Power_State=0;
				}
				Solder_Iron_Sleep_Time_Resset(&Soldering_Iron);
				Solder_Iron_Set_Temperature(&Soldering_Iron, Soldering_Iron.MODE);
			}
			break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Menu_it(struct Menu_List_Vector *self, struct Incoder *Incoder_P1, struct Incoder *Incoder_P2){

	enum {
		Interface_Contoll_Soldering_Heat_Gun,
		Interface_Contoll_Soldering_Separator
	}Interface_Contoll;

	Interface_Contoll=Interface_Contoll_Soldering_Heat_Gun;
	start_Page_Drow();

	while(1){

		if(Incoder_Get_Button_State(Incoder_P2)==Incoder_Button_long){

			Solder_Iron_OFF(&Soldering_Iron);
			Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
			Soldering_Separator_OFF(&Soldering_Separator);

			Menu_List_Vector_it(self, Incoder_P2);
			Incoder_Reset_Button_State(Incoder_P2);
			start_Page_Drow();
		}
		if(Incoder_Get_Button_State(Incoder_P1)==Incoder_Button_long){
			Incoder_Reset_Button_State(Incoder_P1);
			if(Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun)
				Interface_Contoll=Interface_Contoll_Soldering_Separator;
			else
				Interface_Contoll=Interface_Contoll_Soldering_Heat_Gun;
		}

		Value_Changing(&Soldering_Iron.Temperature_Pressets.Manual, 100, UINT16, 800, MAX_ADC_Value, Incoder_P2);
		if(Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun)
			Value_Changing(&Soldering_Heat_Gun.Temperature_Pressets.Manual, 100, UINT16, 400, MAX_ADC_Value, Incoder_P1);
		else
			Value_Changing(&Soldering_Separator.Temperature_Pressets.Manual, 100, UINT16, 400, MAX_ADC_Value, Incoder_P1);



		Menu_Parmetr_Drow(&Soldering_Iron.State, 6, 1, BOOL);
		Menu_Parmetr_Drow(&Soldering_Iron.Temperature_Set, 6, 2, UINT16);
		Menu_Parmetr_Drow(&Soldering_Iron.Temperature, 6, 3, UINT16);
		Menu_Parmetr_Drow(&Soldering_Iron.Full_Power_State, 6, 4, BOOL);
		Menu_Parmetr_Drow(&ZCD.Frequency, 6, 5, UINT8);
		Menu_Parmetr_Drow(&Soldering_Heat_Gun.PID.MAX_Control, 6, 6, UINT16);
		Menu_Parmetr_Drow(&Soldering_Heat_Gun.PAC_Control->Control_Value, 6, 7, UINT16);


		Menu_Parmetr_Drow(&Soldering_Heat_Gun.State, 17, 1, BOOL);
		Menu_Parmetr_Drow(&Soldering_Heat_Gun.Temperature_Set, 17, 2, UINT16);
		Menu_Parmetr_Drow(&Soldering_Heat_Gun.Temperature, 17, 3, UINT16);

		Menu_Parmetr_Drow(&Soldering_Separator.State, 17, 5, BOOL);
		Menu_Parmetr_Drow(&Soldering_Separator.Temperature_Set, 17, 6, UINT16);
		Menu_Parmetr_Drow(&Soldering_Separator.Temperature, 17, 7, UINT16);
		Menu_Parmetr_Drow(&Soldering_Separator.PAC_Control->Control_Value, 1, 7, UINT16);



		if(Incoder_Get_Button_State(Incoder_P2)==Incoder_Button_Short){                 //Solder_Iron_ON/OFF
			Incoder_Reset_Button_State(Incoder_P2);
			if(Soldering_Iron.State==0){
				Solder_Iron_ON(&Soldering_Iron);
			}else{
				Solder_Iron_OFF(&Soldering_Iron);
			}
		}

		if(Incoder_Get_Button_State(Incoder_P1)==Incoder_Button_Short&&Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun){		//Soldering_Heat_Gun_ON/OFF
			Incoder_Reset_Button_State(Incoder_P1);
			if(Soldering_Heat_Gun.State!=Heat_Gun_ON)
				Soldering_Heat_Gun_ON(&Soldering_Heat_Gun);
			else
				Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
		}
		else if(Incoder_Get_Button_State(Incoder_P1)==Incoder_Button_Short&&Interface_Contoll==Interface_Contoll_Soldering_Separator){
			Incoder_Reset_Button_State(Incoder_P1);															//Soldering_Separator_ON/OFF
				if(Soldering_Separator.State!=Separator_ON)
					Soldering_Separator_ON(&Soldering_Separator);
				else
					Soldering_Separator_OFF(&Soldering_Separator);
		}



		if(Soldering_Iron.Temperature_Set_Prew!=Soldering_Iron.Temperature_Pressets.Manual){
			Soldering_Iron.Temperature_Set_Prew=Soldering_Iron.Temperature_Pressets.Manual;
			Solder_Iron_Sleep_Time_Resset(&Soldering_Iron);
			Solder_Iron_Set_Temperature(&Soldering_Iron, MANUAL);
			Solder_Iron_Flash_Reset_Rewrite_Timer(&Soldering_Iron);
		}
		if(Soldering_Heat_Gun.Temperature_Set_Prew!=Soldering_Heat_Gun.Temperature_Pressets.Manual){
			Soldering_Heat_Gun.Temperature_Set_Prew=Soldering_Heat_Gun.Temperature_Pressets.Manual;
			Soldering_Heat_Gun_Set_Temperature(&Soldering_Heat_Gun, MANUAL);
			Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(&Soldering_Heat_Gun);
		}
		if(Soldering_Separator.Temperature_Set_Prew!=Soldering_Separator.Temperature_Pressets.Manual){
			Soldering_Separator.Temperature_Set_Prew=Soldering_Separator.Temperature_Pressets.Manual;
			Soldering_Separator_Set_Temperature(&Soldering_Separator, MANUAL);
			Soldering_Separator_Flash_Reset_Rewrite_Timer(&Soldering_Separator);
		}
	}
}
















