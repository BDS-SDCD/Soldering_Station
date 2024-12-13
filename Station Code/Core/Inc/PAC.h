/*
 * ZCD.h
 *
 *  Created on: Oct 1, 2024
 *      Author: I
 */

#ifndef INC_PAC_H_
#define INC_PAC_H_
//---------------------------------------------------------------------------------
#include "stm32f1xx.h"
#include "stdlib.h"
//---------------------------------------------------------------------------------

struct PAC{
	struct ZCD *ZCD;
	struct PAC_Control_Vector  *PAC_Control_Vector_Head;
};

struct PAC_Control_Vector{
	struct PAC_Control_Vector *next;
	uint16_t PIN;
	GPIO_TypeDef *GPIO;
	uint16_t Control_Value;
	enum PAC_Control_State{
		PAC_Control_OFF=0,
		PAC_Control_ON=1,
	} State;

};

struct ZCD{
	uint8_t Pin_State_Now:1;
	uint8_t Pin_State_Prew:1;
	enum ZCD_State{
		ZCD_STATE_OFF=0,
		ZCD_STATE_ON=1,
		ZCD_STATE_SETUP=2
	} State;


	uint8_t Frequency;
	uint8_t Frequency_Counter;

	uint16_t TIM_Counter;
	uint16_t MAX_TIM_Counter;

	uint16_t EXTI_PIN;
	GPIO_TypeDef *GPIO;
};
//---------------------------------------------------------------------------------
void ZCD_ini(struct ZCD *self);
uint8_t PAC_EXTI(struct PAC *self,uint16_t *GPIO);
void PAC_it(struct PAC *self);
void PAC_ON(struct PAC *self);
void PAC_OFF(struct PAC *self);
void ZCD_ON_Callback(struct ZCD *self);

struct PAC_Control_Vector *PAC_Control_Vector_Create(struct PAC *self, struct PAC_Control_Vector *element);
//---------------------------------------------------------------------------------

#endif /* INC_PAC_H_ */
