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
struct Button{
	uint8_t ID;
	uint8_t PIN_State:1;

	uint8_t Previos_State:1;
	uint8_t Stable_State:1;
	uint8_t Previos_Stable_State:1;

	uint8_t Presed_counter;
	uint8_t Presed_counter_max;

	uint8_t count;
	uint8_t count_max;

	uint8_t Flag:1;
	uint8_t event:1;
	uint8_t State;

	uint16_t EXTI_PIN;
	uint16_t PIN;
	GPIO_TypeDef *GPIO;
	uint8_t MODE;

	struct Incoder *base;
};
//---------------------------------------------------------------------------------
struct Button_Vector{
	struct Button_Vector *next;
	struct Button *Button;
};
//---------------------------------------------------------------------------------

struct Rotary_Switch{
	uint8_t ID;
	uint8_t State:1;
	uint8_t event:1;

	uint16_t EXTI_PIN;
	uint16_t PIN;
	GPIO_TypeDef *GPIO;

	struct Incoder *base;

};
//---------------------------------------------------------------------------------
struct Incoder{
	uint8_t ID;
	struct Button Button;
	struct Rotary_Switch Rotary_Switch;
};
//---------------------------------------------------------------------------------
#define Button_Mode_Regular 1
#define Button_Mode_Incoder 2
//---------------------------------------------------------------------------------
#define Button_ID_E2B1 0
#define Button_ID_E2B2 1
#define Button_ID_E2B3 2
#define Button_ID_E1B1 3
#define Button_ID_E1B2 4
#define Button_ID_E1B3 5
#define Button_Gerkon_ID 6
#define Full_Power_Button_ID 7
#define Incoder_ID_P1 0
#define Incoder_ID_P2 1

//---------------------------------------------------------------------------------


void Incoder_ini(struct Incoder *self);
void Incoder_EXTI(struct Incoder *self, uint16_t* GPIO);
void Incoder_it(struct Incoder *self);
void Incoder_Handler(struct Incoder *self);

void Rotary_Switch_EXTI(struct Rotary_Switch *self, uint16_t* GPIO);
void Rotary_Switch_Handler(struct Rotary_Switch *self);

void Button_ini(struct Button* self);
void Button_EXTI(struct Button* self,uint16_t* GPIO);
void Button_it(struct Button* self);
void Button_Handler(struct Button* self);


void Button_Vector_Create(struct Button_Vector *Vector, struct Button *Button);
void Button_Vector_EXTI(struct Button_Vector *self,uint16_t* GPIO);
void Button_Vector_it(struct Button_Vector *self);
//---------------------------------------------------------------------------------


#endif /* INC_BUTTON_H_ */
