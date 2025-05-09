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
 * -PAC_Devices_Control_Vector
 * Consist from all PAC control units for all devices. Each device which control its power by PAC have its own PAC_Controll_Unit.
 * Every unit contain control value  and GPIO.
 *
 * ZCD State cycle
 * ZCD_STATE_OFF->ZCD_STATE_SETUP->ZCD_STATE_ON->ZCD_STATE_OFF
 * -ZCD_OFF PAC OFF
 *  Reset all parameters of PAC if all PAC_Controll_Unit is turn OFF.
 *
 * -ZCD_STATE_SETUP
 * Counting TIMs interapts and frequency TIM set into Input capture capture mode + DMA;
 *
 * -ZCD_STATE_ON
 * starts Phase-fired control every PAC_Controll_Unit if even one PAC_Controll_Unit is turn ON.
 * TIM reinitialized into PWM MODE
 *
 * ZCD works if even one PAC_Device_Contol_unit is active
 * When all PAC_Device_Contol_unit are turned off ZCD turn off
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
#define KOEF 0.95

struct PAC_Init{
	DMA_HandleTypeDef *DMA;
	DMA_Channel_TypeDef *DMA_Chanel;
	TIM_TypeDef *TIM_Instance;
	GPIO_TypeDef *IC_Trigger_Channel_GPIO;
	uint16_t IC_Trigger_Channel_PIN;
	uint32_t TIM_CHANNEL;
};

struct PAC_Devices_Control_Init{
	uint16_t Channel_PIN;
	GPIO_TypeDef *Channel_GPIO;
	uint32_t TIM_CHANNEL;
};

struct PAC_Devices_Control_Vector{
	struct PAC_Devices_Control_Vector *next;
	uint16_t Channel_PIN;
	GPIO_TypeDef *Channel_GPIO;
	uint32_t TIM_CHANNEL;
	TIM_HandleTypeDef *tim;
	enum PAC_Device_Control_State{
		PAC_Device_OFF = 0,
		PAC_Device_ON = 1 ,
	} State;

	struct PAC *Parent;

};
struct ZCD{

	DMA_HandleTypeDef *DMA;
	DMA_Channel_TypeDef *DMA_Chanel;
	TIM_TypeDef *TIM_Instance;
	TIM_HandleTypeDef *tim;
	GPIO_TypeDef *IC_Trigger_Channel_GPIO;
	uint16_t IC_Trigger_Channel_PIN;
	uint32_t TIM_CHANNEL;
	enum ZCD_State{
		ZCD_STATE_OFF = 0,
		ZCD_STATE_ON = 1,
		ZCD_STATE_SETUP = 2
	} State;

	uint16_t CNT_DATA[3];
	uint16_t TIM_MAX_Count;
	uint16_t Frequency;


};
struct PAC{
	struct ZCD ZCD;
	struct PAC_Devices_Control_Vector  *PAC_Devices_Control_Vector_Head;
};
//---------------------------------------------------------------------------------


struct PAC *PAC_Base_Create(struct PAC_Init *self);
struct PAC_Devices_Control_Vector *PAC_Devise_Control_Create(struct PAC *self, struct PAC_Devices_Control_Init *element);

void PAC_Device_Control_Set_Control_Value(struct PAC_Devices_Control_Vector *self, uint16_t value);
uint16_t PAC_Device_Control_Get_Control_Value(struct PAC_Devices_Control_Vector *self);
enum PAC_Device_Control_State PAC_Device_Control_Get_State(struct PAC_Devices_Control_Vector *self);
uint16_t PAC_Get_Max_Control_Value(struct PAC *self);
void PAC_Device_Control_ON(struct PAC_Devices_Control_Vector *self);
void PAC_Device_Control_OFF(struct PAC_Devices_Control_Vector *self);

enum ZCD_State PAC_Get_State(struct PAC *self);
uint8_t PAC_Get_Frequency(struct PAC *self);

void PAC_IC_Capture_INT(struct PAC *self);
void PAC_ON_Callback(struct PAC *self);
//---------------------------------------------------------------------------------

#endif /* INC_PAC_H_ */
