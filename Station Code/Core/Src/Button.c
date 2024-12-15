#include"Button.h"
/*
 * Button.c
 *
 *  Created on: Jul 30, 2024
 *      Author: I
 */

//---------------------------------------------------------------------------------
void Rotary_Switch_Handler(struct Rotary_Switch *self){
	self->event=1;
	Incoder_Handler(self->base);
	self->event=0;
}
//---------------------------------------------------------------------------------
void Rotary_Switch_EXTI(struct Rotary_Switch *self, uint16_t* GPIO){
	if(self->EXTI_PIN==*GPIO){
		self->State=HAL_GPIO_ReadPin(self->GPIO,self->PIN);
		Rotary_Switch_Handler(self);
	}
}
//---------------------------------------------------------------------------------
void Button_EXTI(struct Button* self,uint16_t* GPIO){
	if(self->EXTI_PIN==*GPIO){
		self->Flag=1;
		self->count=0;
		self->Presed_counter=0;
	}
}
//---------------------------------------------------------------------------------
void Incoder_EXTI(struct Incoder *self, uint16_t* GPIO){
	Rotary_Switch_EXTI(&(self->Rotary_Switch),  GPIO);
	Button_EXTI(&(self->Button), GPIO);
}
//---------------------------------------------------------------------------------
void Button_MODE_it(struct Button* self){
	switch (self->MODE){

		case Button_Mode_Regular_With_EXTI:
			if(self->PIN_State==HAL_GPIO_ReadPin(self->GPIO,self->PIN)){
				self->State=self->PIN_State;
				Button_Handler(self);
				self->Previos_Stable_State=self->State;
			}
			self->count=0;
			self->Flag=0;
		break;

		case Button_Mode_Regular_Without_EXTI:
			if(self->PIN_State==HAL_GPIO_ReadPin(self->GPIO,self->PIN)){
				self->Stable_State=self->PIN_State;
				if(self->Stable_State!=self->Previos_Stable_State){
					self->State=self->PIN_State;
					Button_Handler(self);
				}
				self->Previos_Stable_State=self->Stable_State;
			}
			self->count=0;

		break;

		case Button_Mode_Incoder:
			if(self->PIN_State==HAL_GPIO_ReadPin(self->GPIO,self->PIN)){
				self->Stable_State=self->PIN_State;
				if(self->Stable_State==GPIO_PIN_RESET){
					self->Presed_counter++;
				}
				else if(self->Previos_Stable_State==GPIO_PIN_RESET){
					if(self->Presed_counter<self->Presed_counter_max)
						self->State=1;
					else
						self->State=2;

					self->Presed_counter=0;
					self->Flag=0;
					self->event=1;
					Incoder_Handler(self->base);
					self->event=0;
				}
				self->Previos_Stable_State=self->Stable_State;
			}
			self->count=0;
		break;
	}
}
//---------------------------------------------------------------------------------
void Button_it(struct Button* self){
	if(self->Flag==1){
		if(self->count==1){
			self->PIN_State=HAL_GPIO_ReadPin(self->GPIO,self->PIN);
		}
		else if(self->count==self->count_max){
			Button_MODE_it(self);
		}
		self->count++;
	}
}
//---------------------------------------------------------------------------------
void Incoder_it(struct Incoder *self){
	Button_it(&self->Button);
}
//---------------------------------------------------------------------------------
void Button_ini(struct Button* self){
	self->count=0;
	self->Presed_counter=0;
	self->Previos_Stable_State=1;
	self->event=0;
	if(self->MODE==Button_Mode_Regular_Without_EXTI)
		self->Flag=1;
	else
		self->Flag=0;
}
//---------------------------------------------------------------------------------
void Button_Vector_Create(struct Button_Vector *Vector, struct Button *Button){
	Button_ini(Button);
	struct Button *Bpr= malloc(sizeof(struct Button));
	*Bpr= *Button;
	if(Vector->Button==NULL)
		Vector->Button=Bpr;
	else{
		struct Button_Vector *Vpr= malloc(sizeof(struct Button_Vector));
		struct Button_Vector *NOW=Vector;
		while(NOW->next!=NULL){
			NOW=NOW->next;
		}
		Vpr->Button=Bpr;
		NOW->next=Vpr;
		NOW->next->next=NULL;
	}
}
//---------------------------------------------------------------------------------
void Button_Vector_EXTI(struct Button_Vector *self,uint16_t* GPIO){
	struct Button_Vector *NOW=self;
	do{
		Button_EXTI(NOW->Button,GPIO);
		NOW=NOW->next;
	}while(NOW!=NULL);
}
//---------------------------------------------------------------------------------
void Button_Vector_it(struct Button_Vector *self){
	struct Button_Vector *NOW=self;
	do{
		Button_it(NOW->Button);
		NOW=NOW->next;
	} while(NOW!=NULL);
}
//---------------------------------------------------------------------------------
void Rotary_Switch_ini(struct Rotary_Switch *self){

}
//---------------------------------------------------------------------------------
void Incoder_ini(struct Incoder *self){
	Button_ini(&(self->Button));
	self->Button.base=self;
	self->Rotary_Switch.base=self;
	self->Button.ID=self->ID;
	self->Rotary_Switch.ID=self->ID;
}
//---------------------------------------------------------------------------------


