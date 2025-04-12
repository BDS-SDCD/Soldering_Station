/*
 * Button.h
 *
 *  Created on: Jul 30, 2024
 *      Author: I
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
//---------------------------------------------------------------------------------
#include "stm32f1xx.h"
#include "stdlib.h"
//---------------------------------------------------------------------------------
	/**
	 * Button_Vector-(next)->Button_Vector-(next)->Button_Vector-(next)->.....
	 *		|						|					  |
	 *	  Button				  Button				Button
	 */
//---------------------------------------------------------------------------------
	/**
	 *		 _________Encoder____________
	 *		|Encoder_Rotary_Switch_Buffer|
	 *		|		  Button			 |
	 *		 Encoder_Rotary_Switch_Buffer
	 *		 	  Encoder_Button_State
	 */
//---------------------------------------------------------------------------------
	/**
	 * Every element's interface of library works on state buffering.
	 * So we can read it's state in different code parts and interpreting as we need.
	 * Every button can have different EXTI_PIN and Signal pin
	 * so many Buttons can have the same EXTI_PIN
	 */
//---------------------------------------------------------------------------------
	/**
	 * Every button and encoder's part are identified by id
	 */
//---------------------------------------------------------------------------------
struct Button{
	uint8_t ID;

	GPIO_PinState PIN_State:1;
	GPIO_PinState Previos_State:1;
	GPIO_PinState Stable_State:1;
	GPIO_PinState Previos_Stable_State:1;

	uint8_t Presed_counter;
	uint8_t Presed_counter_max;

	uint8_t count;								//counting TIM interrupts. Need for "Contact bounce" filter
	uint8_t count_max;							//Number of tim interrupts rechecks PIN state in "Contact bounce" filter

	uint8_t EXTI_Event:1;						//Flag that contain that PIN EXTI was triggered

	uint8_t Button_Event:1;						//if
	uint8_t State;

	uint16_t EXTI_PIN;
	uint16_t PIN;
	GPIO_TypeDef *GPIO;

	enum {
		Button_Mode_Regular_With_EXTI,			//Button witch check PIN state only when EXTI_Event goes
		Button_Mode_Regular_Without_EXTI,		//Button without interrupts check its PIN state every TIM cycle EXTI_Event always equal 1
		Button_Mode_Encoder						//Button_Encoder distinguish between short and long button presses

	}MODE;

	struct Encoder *base;
};
//---------------------------------------------------------------------------------
struct Button_Vector{
	struct Button_Vector *next;
	struct Button *Button;
};
//---------------------------------------------------------------------------------

struct Rotary_Switch{
	uint8_t ID;
	GPIO_PinState State:1;
	uint8_t event:1;

	enum{
		Direct,					//direct
		Reverse					//reverse
	}Rotary_Switch_Direcion;

	uint16_t EXTI_PIN;
	uint16_t PIN;
	GPIO_TypeDef *GPIO;

	struct Encoder *base;

};
//---------------------------------------------------------------------------------
enum Encoder_Button_State{
	Encoder_Button_NotPressed=0,
	Encoder_Button_Short=2,
	Encoder_Button_long=3
};
//---------------------------------------------------------------------------------
struct Encoder{
	uint8_t ID;

	struct Button Button;
	struct Rotary_Switch Rotary_Switch;

	int Encoder_Rotary_Switch_Buffer;					//Encoder_Rotary_Switch_Buffer counting every Rotary_Switch switch
	enum Encoder_Button_State Encoder_Button_State;		//Buffer that contain last encoder button state
														//To reset this state use Encoder_Reset_Button_State(struct Encoder *self);
};
//---------------------------------------------------------------------------------
#define Button_Mode_Regular 1
#define Button_Mode_Encoder 2
//---------------------------------------------------------------------------------
#define Button_ID_SW8 0
#define Button_ID_SW6 1
#define Button_ID_SW5 2
#define Button_ID_SW2 3
#define Button_ID_SW3 4
#define Button_ID_SW7 5
#define Button_Gerkon_ID 6
#define Full_Power_Button_ID 7
#define Encoder_ID_P1 0
#define Encoder_ID_P2 1

//---------------------------------------------------------------------------------


void Encoder_ini(struct Encoder *self);
void Encoder_EXTI(struct Encoder *self, uint16_t *EXTI_PIN);
void Encoder_it(struct Encoder *self);
void Encoder_Handler(struct Encoder *self);
void Encoder_Reset_Rotary_Switch_Buffer(struct Encoder *self);
int Encoder_Get_Rotary_Switch_Buffer(struct Encoder *self);
void Encoder_Reset_Button_State(struct Encoder *self);
enum Encoder_Button_State Encoder_Get_Button_State(struct Encoder *self);

void Rotary_Switch_EXTI(struct Rotary_Switch *self, uint16_t* EXTI_PIN);
void Rotary_Switch_Handler(struct Rotary_Switch *self);

void Button_ini(struct Button* self);
void Button_EXTI(struct Button* self, uint16_t *EXTI_PIN);
void Button_it(struct Button* self);
void Button_Handler(struct Button* self);
GPIO_PinState Button_Get_Pin_State(struct Button* self);
uint8_t  Button_Get_Event_State(struct Button* self);


void Button_Vector_Create(struct Button_Vector *Vector, struct Button *Button);
void Button_Vector_EXTI(struct Button_Vector *self,uint16_t* GPIO);
void Button_Vector_it(struct Button_Vector *self);
//---------------------------------------------------------------------------------


#endif /* INC_BUTTON_H_ */
