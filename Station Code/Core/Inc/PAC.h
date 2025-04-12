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
/** Phase Angle Control
 * Phase Angle Control consist from two parts:
 * -Zero Crossing Detection unit
 * Detect every time when the AC going thru 0 and count how many times TIMs interapts goes between two zero crossing.
 *
 * -PAC_Control_Vector
 * Consist from all PAC control units for all devices. Each device which control its power by PAC have its own PAC_Controll_Unit.
 * Every unit contain control value  and GPIO.
 *
 * ZCD State cycle
 * ZCD_STATE_OFF->ZCD_STATE_SETUP->ZCD_STATE_ON->ZCD_STATE_OFF
 * -ZCD_OFF PAC OFF
 *  Reset all parameters of PAC if all PAC_Controll_Unit is turn OFF.
 *
 * -ZCD_STATE_SETUP
 * Counting TIMs interapts and frequency
 *
 * -ZCD_STATE_ON
 * starts Phase-fired control every PAC_Controll_Unit if even one PAC_Controll_Unit is turn ON.
 *
 *| A
 *|
 *|
 *|                          *
 *|                    *	 |	   *
 *|				   *			       *
 *|            *						  *
 *|          *								 *
 *|        *								   *
 *|      *										*
 *|    *										 *
 *|  *											  *
 *|*												*
 *|____________________________________________________*___Time
 *	|												   |
 *MAX control value								min Control value
 */
//---------------------------------------------------------------------------------
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
	uint8_t Frequency_Counter;	//Frequency_Counter count every time when zero crossing 2 times for one sinus wave period

	uint16_t TIM_Counter;		//Counting how many TIM's interrupts are goes thru one period before Falling edge and Rising edge
	uint16_t MAX_TIM_Counter;	// MAX value of TIM_Counter (MAX Control Value for PAC_Control_Unit)

	uint16_t EXTI_PIN;
	GPIO_TypeDef *GPIO;
};
struct PAC{
	struct ZCD ZCD;
	struct PAC_Control_Vector  *PAC_Control_Vector_Head;
};
//---------------------------------------------------------------------------------
uint8_t PAC_EXTI(struct PAC *self,uint16_t *GPIO);
void PAC_it(struct PAC *self);
void PAC_ini(struct PAC *self);
void PAC_ON(struct PAC *self);
void PAC_OFF(struct PAC *self);
void PAC_Set_Control_Value(uint16_t value, struct PAC_Control_Vector *self);
enum ZCD_State PAC_Get_State(struct PAC *self);
uint8_t PAC_Get_Frequency(struct PAC *self);
void PAC_ON_Callback(struct PAC *self);
uint16_t PAC_Get_Max_Control_Value(struct PAC *self);

struct PAC_Control_Vector *PAC_Control_Vector_Create(struct PAC *self, struct PAC_Control_Vector *element);
//---------------------------------------------------------------------------------

#endif /* INC_PAC_H_ */
