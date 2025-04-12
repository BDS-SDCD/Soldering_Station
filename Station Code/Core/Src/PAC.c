/*
 * ZCD.c
 *
 *  Created on: Oct 1, 2024
 *      Author: I
 */
#include "PAC.h"

//---------------------------------------------------------------------------------
struct PAC_Control_Vector *PAC_Control_Vector_Create(struct PAC *self, struct PAC_Control_Vector *element){
	/**
	 * Create new PAC_Control_Unit to PAC_Control_Vector into PAC
	 * return pointer to new PAC_Control_Unit
	 */
	struct PAC_Control_Vector *NEW_Element= malloc(sizeof(struct PAC_Control_Vector));	//allocate memory for a new element
	*NEW_Element=*element;																//copy data to a new element
	NEW_Element->next=NULL;
	if(self->PAC_Control_Vector_Head==NULL){											//Check. is this a first element
		self->PAC_Control_Vector_Head=NEW_Element;										// New element is head
	}else{																				// else skip to last element
		struct PAC_Control_Vector *now=self->PAC_Control_Vector_Head;
		while(now->next!=NULL){
			now=now->next;
		}
		now->next=NEW_Element;															//next last element is new element
	}
	return NEW_Element;																	//return pointer for a new element
}
//---------------------------------------------------------------------------------
void PAC_Control_Vector_PIN_Reset(struct PAC_Control_Vector *self){
	/**
	 * Reset all GPIO Pins of all PAC_Control_Units
	 */
	while(self!=NULL){
		HAL_GPIO_WritePin(self->GPIO, self->PIN, GPIO_PIN_RESET);
		self=self->next;
	}
}
//---------------------------------------------------------------------------------
void PAC_Controll_Callback(struct PAC *self){
	/**
	 * Set and reset PIN's state for all PAC_Control_Unit in tight time
	 */
	struct PAC_Control_Vector *now=self->PAC_Control_Vector_Head;
	do{
		if(now->State==PAC_Control_ON){													//if Current PAC_Control_Unit is turned ON
			if(self->ZCD.TIM_Counter==(self->ZCD.MAX_TIM_Counter-now->Control_Value)){	// Check if current tim counter equal to Control_Value
				HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_SET);					//if true set PAC_Control_Unit GPIO's PIN state
			}else if(self->ZCD.TIM_Counter>=self->ZCD.MAX_TIM_Counter){					//if TIM_Counter more than MAX_TIM_Counter (max control value)
				HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_RESET);					//Reset PAC_Control_Unit GPIO's PIN state
			}
		}
		now=now->next;																	//go to next PAC_Control_Unit
	}while(now!=NULL);
}
//---------------------------------------------------------------------------------
void ZCD_Falling_Adge_Callback(struct PAC *self){
	/**
	 * Count frequency and reset TIM counter which contain Control Counter
	 */
	if(self->ZCD.State==ZCD_STATE_SETUP){
		self->ZCD.Frequency_Counter++;
	}
	self->ZCD.TIM_Counter=0;
}
//---------------------------------------------------------------------------------
void ZCD_Rising_Adge_Callback(struct PAC *self){
	/**
	 * Check ZCD State and set max control value after setup ends
	 */
	if(self->ZCD.State==ZCD_STATE_SETUP){							//if setup
		if(self->ZCD.Frequency!=0){									//and frequency count!=0
			self->ZCD.MAX_TIM_Counter=self->ZCD.TIM_Counter-3;		//Set max control value which can be set ass control value in PAC_Control_Unit
			PAC_ON_Callback(self);
			self->ZCD.State=ZCD_STATE_ON;							//set turn ON state
		}
	}
	PAC_Control_Vector_PIN_Reset(self->PAC_Control_Vector_Head);	//Reset all GPIO Pins of all PAC_Control_Units
}
//---------------------------------------------------------------------------------
void ZCD_ini(struct ZCD *self){
	/**
	 * Inicialistaion Reset all value
	 */
	self->Pin_State_Now=0;
	self->Pin_State_Prew=0;
	self->Frequency=0;
	self->MAX_TIM_Counter=0;
	self->TIM_Counter=0;
}
//---------------------------------------------------------------------------------
void ZCD_OFF(struct ZCD *self){
	/**
	 * Turn OFF ZCD
	 */
	self->State=ZCD_STATE_OFF;
	self->Frequency=0;				//Reset frequency count
}
//---------------------------------------------------------------------------------
void ZCD_ON(struct ZCD *self){
	/**
	 * Turn ON ZCD
	 */
	self->State=ZCD_STATE_SETUP;
}
//---------------------------------------------------------------------------------
void PAC_ON(struct PAC *self){
	/**
	 * Turn ON PAC
	 */
	if(self->ZCD.State==ZCD_STATE_OFF){
		ZCD_ini(&self->ZCD);			//Resets ZCD
		ZCD_ON(&self->ZCD);				//Turn ON ZCD
	}
}
//---------------------------------------------------------------------------------
void PAC_OFF(struct PAC *self){
	/**
	 * ZCD Turns OFF if all PAC_Control_Units are turned OFF
	 */
	struct PAC_Control_Vector *now;
	uint8_t State=0;
	now=self->PAC_Control_Vector_Head;

	while(now!=NULL){
		State|=now->State;
		if(now->State==PAC_Control_OFF)
			HAL_GPIO_WritePin(now->GPIO, now->PIN, GPIO_PIN_RESET);		//Resets GPIO pin of current PAC_Control_Unit
		now=now->next;													//Go to next unit
	}
	if(State==PAC_Control_OFF){											//Check State
		ZCD_OFF(&self->ZCD);											//If all PAC_Control_Units are turned OFF, turn OFF ZCD
	}

}
//---------------------------------------------------------------------------------
uint8_t PAC_EXTI(struct PAC *self,uint16_t *GPIO){
	/**
	 * PACs GPIOs PIN interrupt
	 * return 1 if current interrupt was created by ZCDs interrupt GPIOs PIN
	 * return 0 if current interrupt was not created by ZCDs interrupt GPIOs PIN
	 */
	if(self->ZCD.State!=ZCD_STATE_OFF){								//check if ZCD state is ON OR Setup
		if(*GPIO==self->ZCD.EXTI_PIN){								//Was current interrupt created by ZCD's interrupt GPIO's PIN
			self->ZCD.Pin_State_Now=HAL_GPIO_ReadPin(self->ZCD.GPIO,self->ZCD.EXTI_PIN);		//set current pin state
			if(self->ZCD.Pin_State_Now!=self->ZCD.Pin_State_Prew){	//if pin state was Changed
				if(self->ZCD.Pin_State_Now==GPIO_PIN_RESET){		//Falling edge
					ZCD_Falling_Adge_Callback(self);
				}
				else{												//Rising edge
					ZCD_Rising_Adge_Callback(self);
				}
			}
			self->ZCD.Pin_State_Prew=self->ZCD.Pin_State_Now;		// Set current state to prew state
			return 1;												//return 1
		}
	}
	return 0;														//return 0
}
//---------------------------------------------------------------------------------
void PAC_it(struct PAC *self){
	/**
	 * PAC TIM iteration
	 * before setup's end this function counting frequency and incrementing PAC control tim counter to compute max control value
	 * and after call
	 * this function must called for a 10000 times in one second
	 *
	 */
	static uint16_t count=0;
	if(self->ZCD.State!=ZCD_STATE_OFF){
		if(self->ZCD.State==ZCD_STATE_SETUP){
			if(count++==9999){										//counting for 1 second
				self->ZCD.Frequency=self->ZCD.Frequency_Counter/2;	//computing frequency	(ZCD.Frequency_Counter count every time when zero crossing 2 times for one sin period)
				self->ZCD.Frequency_Counter=0;
				count=0;
			}
		}else{														//ZCD_STATE is ON when frequency end MAX_TIM_Counter are computed
			PAC_Controll_Callback(self);							//Call Control Function
		}
		self->ZCD.TIM_Counter++;	//Counting how many TIM's interrupts are goes thru one period before Falling edge and Rising edge
									//in ZCD_Rising_Adge_Callback value from this counter will be written into MAX_TIM_Counter (MAX control value for PAC_Control_Unit);
	}
}
//---------------------------------------------------------------------------------
void PAC_Set_Control_Value(uint16_t value, struct PAC_Control_Vector *self){
	/**
	 * Set control value for current PAC_Control_Unit
	 */
	self->Control_Value=value;
}
//---------------------------------------------------------------------------------
enum ZCD_State PAC_Get_State(struct PAC *self){
	return self->ZCD.State;
}
//---------------------------------------------------------------------------------
uint8_t PAC_Get_Frequency(struct PAC *self){
	return self->ZCD.Frequency;
}
//---------------------------------------------------------------------------------
uint16_t PAC_Get_Max_Control_Value(struct PAC *self){
	/**
	 * MAX control value for PAC_Control_Unit
	 */
	return self->ZCD.MAX_TIM_Counter;
}
//---------------------------------------------------------------------------------
void PAC_ini(struct PAC *self){
	ZCD_ini(&self->ZCD);
}






