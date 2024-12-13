/*
 * ZCD.c
 *
 *  Created on: Oct 1, 2024
 *      Author: I
 */
#include "PAC.h"

//---------------------------------------------------------------------------------
struct PAC_Control_Vector *PAC_Control_Vector_Create(struct PAC *self, struct PAC_Control_Vector *element){
	struct PAC_Control_Vector *NEW_Element= malloc(sizeof(struct PAC_Control_Vector));
	*NEW_Element=*element;
	NEW_Element->next=NULL;
	if(self->PAC_Control_Vector_Head==NULL){
		self->PAC_Control_Vector_Head=NEW_Element;
	}else{
		struct PAC_Control_Vector *now=self->PAC_Control_Vector_Head;
		while(now->next!=NULL){
			now=now->next;
		}
		now->next=NEW_Element;
	}
	return NEW_Element;
}
//---------------------------------------------------------------------------------
void PAC_Control_Vector_PIN_Reset(struct PAC_Control_Vector *self){
	while(self!=NULL){
		HAL_GPIO_WritePin(self->GPIO, self->PIN, GPIO_PIN_RESET);
		self=self->next;
	}
}
//---------------------------------------------------------------------------------
void PAC_Controll_Callback(struct PAC *self){
	struct PAC_Control_Vector *now=self->PAC_Control_Vector_Head;
	do{
		if(now->State==PAC_Control_ON){
			if(self->ZCD->TIM_Counter==(self->ZCD->MAX_TIM_Counter-now->Control_Value)){
				HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_SET);
			}else if(self->ZCD->TIM_Counter>=self->ZCD->MAX_TIM_Counter){
				HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_RESET);
			}
		}
		now=now->next;
	}while(now!=NULL);
}
//---------------------------------------------------------------------------------
void ZCD_Falling_Adge_Callback(struct PAC *self){
	if(self->ZCD->State==ZCD_STATE_SETUP){
		self->ZCD->Frequency_Counter++;
	}
	self->ZCD->TIM_Counter=0;
}
//---------------------------------------------------------------------------------
void ZCD_Rising_Adge_Callback(struct PAC *self){
	if(self->ZCD->State==ZCD_STATE_SETUP){
		if(self->ZCD->Frequency!=0){
			self->ZCD->MAX_TIM_Counter=self->ZCD->TIM_Counter-3;
			ZCD_ON_Callback(self->ZCD);
			self->ZCD->State=ZCD_STATE_ON;
		}
	}
	PAC_Control_Vector_PIN_Reset(self->PAC_Control_Vector_Head);
}
//---------------------------------------------------------------------------------
void ZCD_ini(struct ZCD *self){
	self->Pin_State_Now=0;
	self->Pin_State_Prew=0;
	self->Frequency=0;
	self->MAX_TIM_Counter=0;
	self->TIM_Counter=0;
}
//---------------------------------------------------------------------------------
void ZCD_OFF(struct ZCD *self){
	self->State=ZCD_STATE_OFF;
	self->Frequency=0;
}
//---------------------------------------------------------------------------------
void ZCD_ON(struct ZCD *self){
	self->State=ZCD_STATE_SETUP;
}
//---------------------------------------------------------------------------------
void PAC_ON(struct PAC *self){
	if(self->ZCD->State==ZCD_STATE_OFF){
		ZCD_ini(self->ZCD);
		ZCD_ON(self->ZCD);
	}
}
//---------------------------------------------------------------------------------
void PAC_OFF(struct PAC *self){

	struct PAC_Control_Vector *now;
	uint8_t State=0;
	now=self->PAC_Control_Vector_Head;

	while(now!=NULL){
		State|=now->State;
		if(now->State==PAC_Control_OFF)
			HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_RESET);
		now=now->next;
	}
	if(State==PAC_Control_OFF){
		ZCD_OFF(self->ZCD);
	}

}
//---------------------------------------------------------------------------------
uint8_t PAC_EXTI(struct PAC *self,uint16_t *GPIO){
	if(self->ZCD->State!=ZCD_STATE_OFF){
		if(*GPIO==self->ZCD->EXTI_PIN){
			self->ZCD->Pin_State_Now=HAL_GPIO_ReadPin(self->ZCD->GPIO,self->ZCD->EXTI_PIN);
			if(self->ZCD->Pin_State_Now!=self->ZCD->Pin_State_Prew){
				if(self->ZCD->Pin_State_Now==GPIO_PIN_RESET){
					ZCD_Falling_Adge_Callback(self);
				}
				else{
					ZCD_Rising_Adge_Callback(self);
				}
			}
			self->ZCD->Pin_State_Prew=self->ZCD->Pin_State_Now;
			return 1;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------
void PAC_it(struct PAC *self){
	static uint16_t count=0;
	if(self->ZCD->State!=ZCD_STATE_OFF){
		if(self->ZCD->State==ZCD_STATE_SETUP){
			if(count++==9999){
				self->ZCD->Frequency=self->ZCD->Frequency_Counter;
				self->ZCD->Frequency_Counter=0;
				count=0;
			}
		}else{
			PAC_Controll_Callback(self);
		}
		self->ZCD->TIM_Counter++;
	}
}
//---------------------------------------------------------------------------------
