/*
 * Solid_Station.h
 *
 *  Created on: Sep 6, 2024
 *      Author: I
 */

#ifndef INC_SOLDERING_STATION_H_
#define INC_SOLDERING_STATION_H_

#include <PAC.h>
#include "stm32f1xx_hal.h"
#include "PID.h"
#include "Filter.h"
#include "Button.h"
#include "Flash_Custom.h"
//---------------------------------------------------------------------------------
#define Soldering_Station_ALL_Struct_Flash_Page 126
#define Soldering_Iron_Struct_Key 0x0801F802
#define Soldering_Heat_Gun_Struct_Key 0x0801F804
#define Soldering_Separator_Struct_Key 0x1801F804

#define Soldering_Iron_Manual_Temperature_Flash_Key 0x0801F806
#define Soldering_Iron_Manual_Temperature_Flash_Page 125

#define Soldering_Heat_Gun_Manual_Temperature_Flash_Key 0x0801F808
#define Soldering_Heat_Gun_Manual_Temperature_Flash_Page 124

#define Soldering_Separator_Manual_Temperature_Flash_Key 0x1801F808
#define Soldering_Separator_Manual_Temperature_Flash_Page 123

#define Flash_Rewrite_Timer_Max_Count 200

#define MAX_ADC_Value 3800

//---------------------------------------------------------------------------------

enum MODE{
	MANUAL,
	PRESSET1,
	PRESSET2,
	SLEEP
};

enum Flash_Timer_State{
	Flash_Timer_State_OFF,
	Flash_Timer_State_ON
};
struct Temperature_Pressets{
		uint16_t Manual;
		uint16_t Presset1;
		uint16_t Presset2;
		uint16_t Sleep;
	};

struct Temperature_Corection{
		int Manual;
		int Presset1;
		int Presset2;
	};

struct Flash_Rewrite_Timer{
	uint16_t Flash_Timer_Count;
	enum Flash_Timer_State Flash_Timer_State;
};

struct Vibration_Sensor{
	GPIO_TypeDef *GPIO;
	uint16_t EXTI_PIN;
	uint8_t State;
	uint8_t Prew_State;
};

struct Soldering_Iron {
	struct PID PID;
	struct Filter Filter;
	enum MODE MODE;
	enum MODE PREW_MODE;
	struct Temperature_Pressets Temperature_Pressets;
	struct Temperature_Corection Temperature_Corection;

	struct Vibration_Sensor *VS;
	struct Button *Full_Power_Button;
	TIM_HandleTypeDef *PWM_htim;

	uint16_t Add_Temperature;
	uint8_t Full_Power_State;

	uint16_t Temperature_Set;
	uint16_t Temperature_Set_Prew;
	uint32_t Temperature;

	uint16_t Timer;
	uint8_t Sleep_time;
	uint16_t Disable_time;

	enum {
		Soldering_Iron_OFF=0,
		Soldering_Iron_ON=1,
	}State;

	struct Flash_Rewrite_Timer Flash_Rewrite_Timer;


	uint32_t Flash_Key;
};

struct Soldering_Heat_Gun {
	struct PID PID;
	struct Filter Filter;
	enum MODE MODE;
	struct Temperature_Pressets Temperature_Pressets;
	struct Temperature_Corection Temperature_Corection;
	struct PAC_Control_Vector *PAC_Control;
	struct PAC *PAC;

	uint16_t Add_Temperature;

	uint16_t Temperature_Set;
	uint16_t Temperature_Set_Prew;
	uint32_t Temperature;

	uint16_t OFF_PIN;
	GPIO_TypeDef *OFF_GPIO;

	uint16_t MANUAL_PIN;
	GPIO_TypeDef *MANUAL_GPIO;

	uint16_t Temperature_Coolling;

	enum {
		Heat_Gun_OFF=0,
		Heat_Gun_ON=1,
		Heat_Gun_COOLING=2
	}State;

	struct Flash_Rewrite_Timer Flash_Rewrite_Timer;

	uint32_t Flash_Key;
};

struct Soldering_Separator {
	struct PID PID;
	struct Filter Filter;
	enum MODE MODE;
	struct Temperature_Pressets Temperature_Pressets;
	struct Temperature_Corection Temperature_Corection;
	struct PAC_Control_Vector *PAC_Control;
	struct PAC *PAC;

	uint16_t Add_Temperature;

	uint16_t Temperature_Set;
	uint16_t Temperature_Set_Prew;
	uint32_t Temperature;

	enum {
		Separator_OFF=0,
		Separator_ON=1,
	}State;

	struct Flash_Rewrite_Timer Flash_Rewrite_Timer;

	uint32_t Flash_Key;
};
//---------------------------------------------------------------------------------
void Solder_Iron_it(struct Soldering_Iron *self);
void Solder_Iron_tim_it(struct Soldering_Iron *self);
void Solder_Iron_ON(struct Soldering_Iron *self);
void Solder_Iron_OFF(struct Soldering_Iron *self);
void Solder_Iron_ini(struct Soldering_Iron *self);
void Solder_Iron_Set_Temperature(struct Soldering_Iron *self, enum MODE MODE);
void Solder_Iron_Sleep_Time_Resset(struct Soldering_Iron *self);
uint32_t Solder_Iron_Flash_Write_Manual_Temperature(struct Soldering_Iron *self);
uint8_t Solder_Iron_Flash_Read_Manual_Temperature(struct Soldering_Iron *self);
void Solder_Iron_Flash_Reset_Rewrite_Timer(struct Soldering_Iron *self);

void Soldering_Heat_Gun_ini(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_it(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_ON(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_OFF(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_Temperature(struct Soldering_Heat_Gun *self,enum MODE MODE);
uint8_t Soldering_Heat_Gun_Flash_Read_Manual_Temperature(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(struct Soldering_Heat_Gun *self);

void Soldering_Separator_ini(struct Soldering_Separator *self);
void Soldering_Separator_ON(struct Soldering_Separator *self);
void Soldering_Separator_OFF(struct Soldering_Separator *self);
void Soldering_Separator_Set_Temperature(struct Soldering_Separator *self, enum MODE MODE);
void Soldering_Separator_it(struct Soldering_Separator *self);
uint8_t Soldering_Separator_Flash_Read_Manual_Temperature(struct Soldering_Separator *self);
void Soldering_Separator_Flash_Reset_Rewrite_Timer(struct Soldering_Separator *self);


uint32_t Write_Manual_Temperature(struct Temperature_Pressets *self, uint32_t Flash_Key, uint8_t Flash_Page);
uint8_t Flash_Read_Manual_Temperature(struct Temperature_Pressets *self, uint32_t Flash_Key, uint8_t Flash_Page);
uint8_t Soldering_Station_Read_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
uint8_t Soldering_Station_Write_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
void Flash_Rewrite_Timer_tim_it(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
void Flash_Reset_Rewrite_Timer(struct Flash_Rewrite_Timer *self);
void Flash_Rewrite_Timer_ini(struct Flash_Rewrite_Timer *self);
//---------------------------------------------------------------------------------
#endif /* INC_SOLDERING_STATION_H_ */
