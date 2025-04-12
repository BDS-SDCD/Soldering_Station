#include"Button.h"
/*
 * Button.c
 *
 *  Created on: Jul 30, 2024
 *      Author: I
 */
//---------------------------------------------------------------------------------
void Encoder_Handler(struct Encoder *self){
	/**
	 * This function describes method of interacting with Encoder based on buffer
	 */
	if(self->Rotary_Switch.event){
		if(self->Rotary_Switch.Rotary_Switch_Direcion==Direct)
			if(self->Rotary_Switch.State){
				self->Encoder_Rotary_Switch_Buffer++;
			}
			else{
				self->Encoder_Rotary_Switch_Buffer--;
			}
		else
			if(self->Rotary_Switch.State){
				self->Encoder_Rotary_Switch_Buffer--;
			}
			else{
				self->Encoder_Rotary_Switch_Buffer++;
			}
	}else if(self->Button.Button_Event){
		if(self->Button.State==1){
			self->Encoder_Button_State=Encoder_Button_Short;
		}else{
			self->Encoder_Button_State=Encoder_Button_long;
		}
		self->Button.Button_Event=0;
	}
}
//---------------------------------------------------------------------------------
void Button_Handler(struct Button* self){
	self->Button_Event=1;
}
//---------------------------------------------------------------------------------
void Rotary_Switch_Handler(struct Rotary_Switch *self){
	/**
	 * Set event equal 1 and run Encoder_Handler
	 */
	self->event=1;
	Encoder_Handler(self->base);
	self->event=0;
}
//---------------------------------------------------------------------------------
void Rotary_Switch_EXTI(struct Rotary_Switch *self, uint16_t* EXTI_PIN){
	/**
	 * Check if current EXTI are cursed by current Rotary_Switch EXTI_PIN
	 */
	if(self->EXTI_PIN==*EXTI_PIN){
		self->State=HAL_GPIO_ReadPin(self->GPIO,self->PIN);
		Rotary_Switch_Handler(self);
	}
}
//---------------------------------------------------------------------------------
void Button_EXTI(struct Button* self, uint16_t *EXTI_PIN){
	/*
	 * Check if current EXTI are cursed by current BUTTON EXTI_PIN
	 * All buttons with same EXTI_PIN gets EXTI_Event
	 */
	if(self->EXTI_PIN==*EXTI_PIN){
		self->EXTI_Event=1;
		self->count=0;
	}
}
//---------------------------------------------------------------------------------
void Encoder_EXTI(struct Encoder *self, uint16_t *EXTI_PIN){
	/**
	 *Check if current EXTI are cursed by current Encoder's parts
	 */
	Rotary_Switch_EXTI(&(self->Rotary_Switch),  EXTI_PIN);
	Button_EXTI(&(self->Button), EXTI_PIN);
}
//---------------------------------------------------------------------------------
void Button_MODE_it(struct Button* self){
	/**
	 * Function that describes every button's work mode algorithm
	 */
	switch (self->MODE){

		case Button_Mode_Regular_With_EXTI:				//Button witch check PIN state only when EXTI_Event goes
			self->Stable_State=self->PIN_State;			//Write stable state
			if(self->Stable_State!=self->Previos_Stable_State){
				Button_Handler(self);						//Run button handler
				self->Previos_Stable_State=self->Stable_State;
			}
			self->EXTI_Event=0;							//Reset EXTI event to didn't check the button without EXTI interrupt
		break;

		case Button_Mode_Regular_Without_EXTI:			//Button without interrupts check its PIN state every TIM cycle EXTI_Event always equal 1
														//so we need to check iteration where stable state are different (Rising/Falling edge)
			self->Stable_State=self->PIN_State;			//Write stable state
			if(self->Stable_State!=self->Previos_Stable_State){
				self->State=self->PIN_State;
				Button_Handler(self);					//Run button handler
			}
			self->Previos_Stable_State=self->Stable_State;	//Rewrite prew stable state

		break;

		case Button_Mode_Encoder:									//Button_Encoder must distinguish between short and long button presses

			self->Stable_State=self->PIN_State;
			if(self->Stable_State==GPIO_PIN_RESET){					//
				self->Presed_counter++;								//Counting how many iterations button was pressed
			}
			else if(self->Previos_Stable_State==GPIO_PIN_RESET){	//If stable state was changed (rising edge)
				if(self->Presed_counter<self->Presed_counter_max)	//and button was pressed less than Presed_counter_max
					self->State=1;									//State equal 1(short press)
				else
					self->State=2;									//State equal 1(Long press)

				self->Presed_counter=0;								//Reset pressed iteration counter
				self->EXTI_Event=0;									//Reset EXTI event
				self->Button_Event=1;								//Set button event
				Encoder_Handler(self->base);						//run Encoder_Handler
			}
			else{													//If stable state wasn't changed
				self->EXTI_Event=0;									//Reset EXTI event
				self->Presed_counter=0;								//Reset pressed iteration counter
			}
			self->Previos_Stable_State=self->Stable_State;			//Rewrite prew stable state

		break;
	}
}
//---------------------------------------------------------------------------------
void Button_it(struct Button* self){
	/**
	 * This function realize "Contact bounce" filter and run main logic function
	 */
	if(self->EXTI_Event==1){												//Check pins state every time when EXTI's interrupt goes
		if(self->count==1){													//Contact bounce filter
			self->PIN_State=HAL_GPIO_ReadPin(self->GPIO,self->PIN);
		}
		else if(self->count==self->count_max){
			if(self->PIN_State==HAL_GPIO_ReadPin(self->GPIO,self->PIN)){
				Button_MODE_it(self);
			}
			self->count=0;
		}
		self->count++;
	}
}
//---------------------------------------------------------------------------------
void Encoder_it(struct Encoder *self){
	/*
	 * Encoder iteration
	 */
	Button_it(&self->Button);
}
//---------------------------------------------------------------------------------
void Button_ini(struct Button* self){
	/**
	 *Base button's parameters initialization
	 */
	self->count=0;
	self->Presed_counter=0;
	self->Previos_Stable_State=1;
	self->Button_Event=0;
	if(self->MODE==Button_Mode_Regular_Without_EXTI)
		self->EXTI_Event=1;								//for button without interrupts EXTI_Event always equal 1
	else
		self->EXTI_Event=0;
}
//---------------------------------------------------------------------------------
void Button_Vector_Create(struct Button_Vector *Vector, struct Button *Button){
	/**
	 * Create a new element of Button_Vector which contain Button struct
	 */
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
void Button_Vector_EXTI(struct Button_Vector *self, uint16_t* GPIO){
	/**
	 * Run Button EXTI for all Button into Button_Vector
	 * First Parameter must be head of vector
	 */
	struct Button_Vector *NOW=self;
	do{
		Button_EXTI(NOW->Button,GPIO);
		NOW=NOW->next;
	}while(NOW!=NULL);
}
//---------------------------------------------------------------------------------
void Button_Vector_it(struct Button_Vector *self){
	/**
	 * Run Button_it for all Button into Button_Vector
	 */
	struct Button_Vector *NOW=self;
	do{
		Button_it(NOW->Button);
		NOW=NOW->next;
	} while(NOW!=NULL);
}
//---------------------------------------------------------------------------------
GPIO_PinState Button_Get_Pin_State(struct Button* self){
	/*
	 * Return Button stable state and reset Button_Event
	 */
	self->Button_Event=0;
	return self->Stable_State;
}
//---------------------------------------------------------------------------------
uint8_t  Button_Get_Event_State(struct Button* self){
	/*
	 * Return Button event
	 */
	return self->Button_Event;
}
//---------------------------------------------------------------------------------
void Encoder_Reset_Rotary_Switch_Buffer(struct Encoder *self){
	/**
	 * Reset Encoder_Rotary switch buffer
	 */
	self->Encoder_Rotary_Switch_Buffer=0;
}
//---------------------------------------------------------------------------------
int Encoder_Get_Rotary_Switch_Buffer(struct Encoder *self){
	/**
	 * Return how many times Rotary_Switch has switch it's position
	 * Return value can have negative value
	 */
	return  self->Encoder_Rotary_Switch_Buffer;
}
//---------------------------------------------------------------------------------
void Encoder_Reset_Button_State(struct Encoder *self){
	self->Encoder_Button_State=Encoder_Button_NotPressed;
}
//---------------------------------------------------------------------------------
enum Encoder_Button_State Encoder_Get_Button_State(struct Encoder *self){
	return self->Encoder_Button_State;
}
//---------------------------------------------------------------------------------
void Rotary_Switch_ini(struct Rotary_Switch *self){

}
//---------------------------------------------------------------------------------
void Encoder_ini(struct Encoder *self){
	/**
	 * Base Encoder's parameters initialization
	 */
	Button_ini(&(self->Button));
	self->Button.base=self;
	self->Rotary_Switch.base=self;
	self->Button.ID=self->ID;
	self->Rotary_Switch.ID=self->ID;
	self->Encoder_Button_State=Encoder_Button_NotPressed;
	self->Encoder_Rotary_Switch_Buffer=0;
}
//---------------------------------------------------------------------------------


