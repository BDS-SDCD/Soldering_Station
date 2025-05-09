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
//--------------------------------------------------------------------------------- For user

#define Soldering_Iron_PID_KP 5
#define	Soldering_Iron_PID_KI 0.05
#define	Soldering_Iron_PID_KD 1
#define Soldering_Iron_Temperature_Converting_Coeff 8.4
#define Soldering_Iron_MIN_Manual_Preset_Temperature_C 100

#define	Soldering_Heat_Gun_PID_KP 0.2
#define	Soldering_Heat_Gun_PID_KI 0.01//0.03;//0.03
#define	Soldering_Heat_Gun_PID_KD 0.4//1.5;
#define Soldering_Heat_Gun_Temperature_Converting_Coeff 8.4
#define Soldering_Heat_Gun_MIN_Manual_Preset_Temperature_C 50

#define	Soldering_Separator_PID_KP 0.15
#define	Soldering_Separator_PID_KI 0.0005//0.0005;//0.03
#define	Soldering_Separator_PID_KD 0.001//0.1;
#define Soldering_Separator_Temperature_Converting_Coeff 3.2
#define Soldering_Separator_MIN_Manual_Preset_Temperature_C 50

#define Solder_Iron_Set_Preset1_Button_ID 			Button_ID_SW8		//Buttons mapping for UI_Face
#define Solder_Iron_Set_Preset2_Button_ID 			Button_ID_SW2

#define Soldering_Heat_Gun_Set_Preset1_Button_ID 	Button_ID_SW6
#define Soldering_Heat_Gun_Set_Preset2_Button_ID 	Button_ID_SW3

#define Soldering_Separator_Set_Preset1_Button_ID 	Button_ID_SW5
#define Soldering_Separator_Set_Preset2_Button_ID	Button_ID_SW7



#define X2_Modefire_Button_ID						Button_ID_SW8		//Buttons mapping for UI_Menu
#define X5_Modefire_Button_ID						Button_ID_SW2
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


/**
 * tract from Periphery ADC devise to Control system;
 * ADC_HAL->ADC_Data[]->Filter->Temperatur_Correction->Set_Curennt_Point_PID->Control system(PWM,PAC_Controll);
 *
 * tract from Set MODE Temperature devise to PID Set Point;
 * Device_Set_MODE_Temperature->Set_Temperature->Temperature_Converting.Temperature_Set_ADC->PID_Set_Point;
 */

//---------------------------------------------------------------------------------
enum MODE{
	MANUAL,
	PRESET1,
	PRESET2,
	SLEEP
};

enum Flash_Timer_State{
	Flash_Timer_State_OFF,
	Flash_Timer_State_ON
};
struct Temperature_Presets{
		uint16_t Manual;
		uint16_t Preset1;
		uint16_t Preset2;
		uint16_t Sleep;
		uint16_t Prew_Set_Temperature_Manual;
	};

struct Temperature_Corection{
		int Manual;
		int Preset1;
		int Preset2;
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

struct Temperature_Converting{
	uint16_t Current_Temperature_ADC;
	uint16_t Current_Temperature_C;
	uint16_t Temperature_Set_ADC;
	uint16_t Temperature_Set_C;
	float Coeff;
};

struct Soldering_Iron {
	struct PID PID;
	struct Filter Filter;
	enum MODE MODE;
	enum MODE PREW_MODE;
	struct Temperature_Presets Temperature_Presets_C;
	struct Temperature_Converting Temperature_Converting;
	struct Temperature_Corection Temperature_Corection_C;
	struct Temperature_Corection Temperature_Corection_ADC;

	struct Vibration_Sensor VS;
	struct Button Full_Power_Button;
	TIM_HandleTypeDef *PWM_htim;

	enum Full_Power_State{
		Full_Power_OFF=0,
		Full_Power_ON=1
	}Full_Power_State;

	uint16_t Timer;
	uint8_t Sleep_time;
	uint16_t Disable_time;

	enum Soldering_Iron_State{
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
	struct Temperature_Presets Temperature_Presets_C;
	struct Temperature_Converting Temperature_Converting;
	struct Temperature_Corection Temperature_Corection_C;
	struct Temperature_Corection Temperature_Corection_ADC;
	struct PAC_Devices_Control_Vector *PAC_Control;
	struct PAC *PAC;

	uint16_t OFF_PIN;
	GPIO_TypeDef *OFF_GPIO;

	uint16_t MANUAL_PIN;
	GPIO_TypeDef *MANUAL_GPIO;

	struct{
		uint16_t ADC;
		uint16_t C;
	}Temperature_Coolling;

	enum Soldering_Heat_Gun_State{
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
	struct Temperature_Presets Temperature_Presets_C;
	struct Temperature_Converting Temperature_Converting;
	struct Temperature_Corection Temperature_Corection_C;
	struct Temperature_Corection Temperature_Corection_ADC;
	struct PAC_Devices_Control_Vector *PAC_Control;
	struct PAC *PAC;

	enum Soldering_Separator_State{
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
void Solder_Iron_Set_Manual_Temperature(struct Soldering_Iron *self);
void Solder_Iron_Set_Preset1_Temperature(struct Soldering_Iron *self);
void Solder_Iron_Set_Preset2_Temperature(struct Soldering_Iron *self);
void Solder_Iron_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Iron *self);
uint16_t Soldering_Iron_Get_Manual_Preset_Temperature_C(struct Soldering_Iron *self);
enum Soldering_Iron_State Solder_Iron_Get_State(struct Soldering_Iron *self);
uint16_t Soldering_Iron_Get_Current_Temperature_C(struct Soldering_Iron *self);
uint16_t Soldering_Iron_Get_Temperature_Set_C(struct Soldering_Iron *self);
void Solder_Iron_Full_Power_ON(struct Soldering_Iron *self);
void Solder_Iron_Full_Power_OFF(struct Soldering_Iron *self);
enum Full_Power_State Solder_Iron_Get_Full_Power_State(struct Soldering_Iron *self);
uint32_t Solder_Iron_Flash_Write_Manual_Temperature(struct Soldering_Iron *self);
HAL_StatusTypeDef Solder_Iron_Flash_Read_Manual_Temperature(struct Soldering_Iron *self);


void Soldering_Heat_Gun_ini(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_ON(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_OFF(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_Manual_Temperature(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_Preset1_Temperature(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_Preset2_Temperature(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Set_PID_MAX_Control_Value(uint16_t MAX_Control_Value, struct Soldering_Heat_Gun *self);
enum Soldering_Heat_Gun_State Soldering_Heat_Gun_Get_State(struct Soldering_Heat_Gun *self);
uint16_t Soldering_Heat_Gun_Get_Temperature_Set_C(struct Soldering_Heat_Gun *self);
uint16_t Soldering_Heat_Gun_Get_Current_Temperature_C(struct Soldering_Heat_Gun *self);
uint16_t Soldering_Heat_Gun_Get_Manual_Preset_Temperature_C(struct Soldering_Heat_Gun *self);
enum PAC_Device_Control_State Soldering_Heat_Gun_Get_PAC_Controll_State(struct Soldering_Heat_Gun *self);
uint16_t Soldering_Heat_Gun_Get_PAC_Controll_Value(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_it(struct Soldering_Heat_Gun *self);
HAL_StatusTypeDef Soldering_Heat_Gun_Flash_Read_Manual_Temperature(struct Soldering_Heat_Gun *self);


void Soldering_Separator_ini(struct Soldering_Separator *self);
void Soldering_Separator_ON(struct Soldering_Separator *self);
void Soldering_Separator_OFF(struct Soldering_Separator *self);
void Soldering_Separator_Set_Manual_Temperature(struct Soldering_Separator *self);
void Soldering_Separator_Set_Preset1_Temperature(struct Soldering_Separator *self);
void Soldering_Separator_Set_Preset2_Temperature(struct Soldering_Separator *self);
void Soldering_Separator_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Separator *self);
void Soldering_Separator_Set_PID_MAX_Control_Value(uint16_t MAX_Control_Value, struct Soldering_Separator *self);
enum Soldering_Separator_State Soldering_Separator_Get_State(struct Soldering_Separator *self);
uint16_t Soldering_Separator_Get_Temperature_Set_C(struct Soldering_Separator *self);
uint16_t Soldering_Separator_Get_Current_Temperature_C(struct Soldering_Separator *self);
uint16_t Soldering_Separator_Get_Manual_Preset_Temperature_C(struct Soldering_Separator *self);
enum PAC_Device_Control_State Soldering_Separator_Get_PAC_Controll_State(struct Soldering_Separator *self);
uint16_t Soldering_Separator_Get_PAC_Controll_Value(struct Soldering_Separator *self);
void Soldering_Separator_it(struct Soldering_Separator *self);
HAL_StatusTypeDef Soldering_Separator_Flash_Read_Manual_Temperature(struct Soldering_Separator *self);

void Soldering_Station_Temperature_Presets_Corection_Converting(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
HAL_StatusTypeDef Soldering_Station_Read_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
HAL_StatusTypeDef Soldering_Station_Write_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
void Flash_Rewrite_Timer_tim_it(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator);
void Flash_Reset_Rewrite_Timer(struct Flash_Rewrite_Timer *self);
void Flash_Rewrite_Timer_ini(struct Flash_Rewrite_Timer *self);
//---------------------------------------------------------------------------------
#endif /* INC_SOLDERING_STATION_H_ */
