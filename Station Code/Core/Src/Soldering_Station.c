#include "Soldering_Station.h"
//---------------------------------------------------------------------------------
uint16_t Set_Temperature(struct Temperature_Presets *Temperature_Presets_C, struct Temperature_Converting *Temperature_Converting, enum MODE *MODE){
	/**
	 * Set temperature in ADC Levels which occur to the MODE
	 * Temperature_Presets_C;
	 * MODE - MANUAL PRESSET1 PRESSET2 SLEEP
	 * return Temperature in ADC levels
	 */
	switch(*MODE){						//Set temperature in celsius which occur to the MODE
			case MANUAL:
				Temperature_Converting->Temperature_Set_C=Temperature_Presets_C->Manual;
				break;
			case PRESET1:
				Temperature_Converting->Temperature_Set_C=Temperature_Presets_C->Preset1;
				break;
			case PRESET2:
				Temperature_Converting->Temperature_Set_C=Temperature_Presets_C->Preset2;
				break;
			case SLEEP:
				Temperature_Converting->Temperature_Set_C=Temperature_Presets_C->Sleep;
				break;
		}

	Temperature_Converting->Temperature_Set_ADC=(uint16_t)(Temperature_Converting->Temperature_Set_C*Temperature_Converting->Coeff+Temperature_Converting->Coeff/2);	//Converting Celsius to ADC Levels

	if(Temperature_Converting->Temperature_Set_ADC>=MAX_ADC_Value){		// If result more than MAX_ADC_Value
		Temperature_Converting->Temperature_Set_ADC=MAX_ADC_Value;		// Set MAX_ADC_Value as result
		if(*MODE==MANUAL)
			Temperature_Presets_C->Manual=Temperature_Presets_C->Prew_Set_Temperature_Manual;	//Return Manual Preset to Prew_Manual_Preset
	}
	else
		Temperature_Presets_C->Prew_Set_Temperature_Manual=Temperature_Presets_C->Manual;

	return Temperature_Converting->Temperature_Set_ADC;
}
//---------------------------------------------------------------------------------
uint16_t Temperatur_Correction(uint16_t Filtred_ADC_Value, struct Temperature_Converting *Temperature_Converting, struct Temperature_Corection *Temperature_Corection_ADC, enum MODE *MODE){
	/**
	 * Correcting Current ADC Temperature which occur to the MODE and convert Current_Temperature_ADC to Celsius
	 * Return Current_Temperature in ADC levels
	 * Temperature correction work with ADC Level Presets
	 */
	switch(*MODE){
				case MANUAL:
					if(Filtred_ADC_Value>Temperature_Corection_ADC->Manual)		// if current value in ADC levels more than Temperature_Corection_ADC
						Temperature_Converting->Current_Temperature_ADC=Filtred_ADC_Value-Temperature_Corection_ADC->Manual;	//Set corrected Current Temperature in ADC Levels
					else
						Temperature_Converting->Current_Temperature_ADC=0;	//Set Current Temperature in ADC Levels to 0;
					break;
				case PRESET1:
					if(Filtred_ADC_Value>Temperature_Corection_ADC->Preset1)
						Temperature_Converting->Current_Temperature_ADC=Filtred_ADC_Value-Temperature_Corection_ADC->Preset1;
					else
						Temperature_Converting->Current_Temperature_ADC=0;
					break;
				case PRESET2:
					if(Filtred_ADC_Value>Temperature_Corection_ADC->Preset2)
						Temperature_Converting->Current_Temperature_ADC=Filtred_ADC_Value-Temperature_Corection_ADC->Preset2;
					else
						Temperature_Converting->Current_Temperature_ADC=0;
					break;
				case SLEEP:
					Temperature_Converting->Current_Temperature_ADC=Filtred_ADC_Value;
					break;
		}

	return Temperature_Converting->Current_Temperature_ADC;
}
//---------------------------------------------------------------------------------
void Temperature_Presets_Corection_Converting(struct Temperature_Corection *Temperature_Corection_C, struct Temperature_Corection *Temperature_Corection_ADC, struct Temperature_Converting *Temperature_Converting){
	/**
	 * Converting Temperature_Presets_Corection from Celsius to ADC Levels
	 */
	Temperature_Corection_ADC->Manual=Temperature_Corection_C->Manual*Temperature_Converting->Coeff;
	Temperature_Corection_ADC->Preset1=Temperature_Corection_C->Preset1*Temperature_Converting->Coeff;
	Temperature_Corection_ADC->Preset2=Temperature_Corection_C->Preset2*Temperature_Converting->Coeff;
}
//---------------------------------------------------------------------------------
uint16_t Get_Temperature_Set_C(struct Temperature_Converting *self){
	/**
	 * Return Set temperature in Celsius
	 */
	return self->Temperature_Set_C;
}
//---------------------------------------------------------------------------------
uint16_t Get_Current_Temperature_C(struct Temperature_Converting *Temperature_Converting){
	/**
	 * Convert and return Current temperature in Celsius
	 */
	Temperature_Converting->Current_Temperature_C=(uint16_t)Temperature_Converting->Current_Temperature_ADC/Temperature_Converting->Coeff; //Converting Current_Temperature_ADC to Celsius
	return Temperature_Converting->Current_Temperature_C;
}
//---------------------------------------------------------------------------------
uint16_t Get_Manual_Preset_Temperature_C(struct Temperature_Presets *self){
	return self->Manual;
}
//---------------------------------------------------------------------------------
void Set_Manual_Preset_Temperature_C(uint16_t value, struct Temperature_Presets *self){
	self->Manual=value;
}
//---------------------------------------------------------------------------------------------------------------------------------//Solder_Iron
void Vibration_Sensor_it(struct Soldering_Iron *self);
void Solder_Iron_Set_MODE_Temperature(struct Soldering_Iron *self, enum MODE MODE);
void Solder_Iron_Flash_Reset_Rewrite_Timer(struct Soldering_Iron *self);
uint32_t Write_Manual_Temperature(struct Temperature_Presets *self, uint32_t Flash_Key, uint8_t Flash_Page);
uint8_t Flash_Read_Manual_Temperature(struct Temperature_Presets *self, uint32_t Flash_Key, uint8_t Flash_Page);
//---------------------------------------------------------------------------------
void Solder_Iron_it(struct Soldering_Iron *self){
	/**
	 * PID and Filter iteration for Soldering_Iron
	 */
	Vibration_Sensor_it(self);				// We can read state of Vibration Sensor only when PWM is stopped
	Button_it(&self->Full_Power_Button); 	// We can read state of button only when PWM is stopped
	if(self->State==1){
	  HAL_TIM_PWM_Start(self->PWM_htim, TIM_CHANNEL_4);
	  PID_Set_Curent_Point(&self->PID,(int)(Temperatur_Correction(Filter_Combined(&self->Filter), &self->Temperature_Converting, &self->Temperature_Corection_ADC, &self->MODE)));
	  self->PWM_htim->Instance->CCR4=PID_it(&self->PID);	//PID it
	}
}
//---------------------------------------------------------------------------------
void Solder_Iron_Sleep_Time_Resset(struct Soldering_Iron *self){
	/**
	 * Reset sleep mode timer and if last mode was Sleep Set previous mode;
	 */
	if(self->MODE==SLEEP)
		self->MODE=self->PREW_MODE;
	self->Timer=0;
}
//---------------------------------------------------------------------------------
void Solder_Iron_tim_it(struct Soldering_Iron *self){
	/**
	 * Solder_Iron_tim_it Main purples ones a second create Button event for full power button and sleep Mode management
	 */
	static uint8_t TIM_prescaler=0;

	if(TIM_prescaler++==10){
		if(self->State){
			if(self->Timer++==self->Sleep_time){						//if Time more than Sleep time value set sleep mode
				self->PREW_MODE=self->MODE;
				Solder_Iron_Set_MODE_Temperature(self, SLEEP);			//Set Sleep Mode Temperature preset
			}else if(self->Timer==self->Disable_time+self->Sleep_time)	//if time more than Disable_time+Sleep_time Turn OFF Soldering_Iron
				Solder_Iron_OFF(self);
		}
		TIM_prescaler=0;
	}
}
//---------------------------------------------------------------------------------
void Solder_Iron_ini(struct Soldering_Iron *self){
	/**
	 * Basic initialization of all part for starting all processes
	 */
	PID_ini(&self->PID);
	Filter_ini(&self->Filter);
	Button_ini(&self->Full_Power_Button);
	Flash_Rewrite_Timer_ini(&self->Flash_Rewrite_Timer);
	Solder_Iron_Set_Manual_Temperature(self);
}

//---------------------------------------------------------------------------------
void Solder_Iron_ON(struct Soldering_Iron *self){
	Solder_Iron_Sleep_Time_Resset(self);  					//Reset Sleep_timer
	Solder_Iron_Set_MODE_Temperature(self,self->MODE);		//Set Temperature
	self->State=Soldering_Iron_ON;
}
//---------------------------------------------------------------------------------
void Solder_Iron_OFF(struct Soldering_Iron *self){
	self->State=Soldering_Iron_OFF;
	Solder_Iron_Sleep_Time_Resset(self); 	//Reset Sleep_timer
	Solder_Iron_ini(self);					//Reset All Spldering Iron parameters
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_MODE_Temperature(struct Soldering_Iron *self, enum MODE MODE){
	/**
	 * Solder_Iron_Set_Temperature based on MODE and Full_Power_Button
	 * Set Point for PID
	 */
	self->MODE=MODE;
	uint16_t Set_Temperature_ADC_Piont;

	if(self->Full_Power_State==1){
		Set_Temperature_ADC_Piont=self->Temperature_Converting.Temperature_Set_ADC=MAX_ADC_Value;
		self->Temperature_Converting.Temperature_Set_C=(uint16_t)(Set_Temperature_ADC_Piont/self->Temperature_Converting.Coeff);
	}
	else
		Set_Temperature(&self->Temperature_Presets_C, &self->Temperature_Converting, &self->MODE);

	PID_Set_Point(&self->PID, self->Temperature_Converting.Temperature_Set_ADC);		//Set Point for PID
}
//---------------------------------------------------------------------------------
void Solder_Iron_Temperature_Presets_Corection_Converting(struct Soldering_Iron *self){
	/**
	 * Converting Solder_Iron_Temperature_Presets_Corection from Celsius to ADC Levels
	 */
	Temperature_Presets_Corection_Converting(&self->Temperature_Corection_C, &self->Temperature_Corection_ADC, &self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_Manual_Temperature(struct Soldering_Iron *self){
	/**
	 * Set manual temperature for Solder_Iron and set Manual MODE
	 */
	if(self->Temperature_Presets_C.Prew_Set_Temperature_Manual!=self->Temperature_Presets_C.Manual){	//if curent set temperature != Prew_Manual set temperature
		Solder_Iron_Set_MODE_Temperature(self, MANUAL);	//Set temperature PID point
		Solder_Iron_Sleep_Time_Resset(self);			//Reset sleep timer
		Solder_Iron_Flash_Reset_Rewrite_Timer(self);	//
	}
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_Preset1_Temperature(struct Soldering_Iron *self){
	/**
	 * Set Preset1 temperature for Solder_Iron and set Preset1 MODE
	 */
	 Solder_Iron_Set_MODE_Temperature(self, PRESET1);	//Set temperature PID point
	 Solder_Iron_Sleep_Time_Resset(self);				//Reset sleep timer
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_Preset2_Temperature(struct Soldering_Iron *self){
	/**
	 * Set Preset2 temperature for Solder_Iron and set Preset2 MODE
	 */
	 Solder_Iron_Set_MODE_Temperature(self, PRESET2);	//Set temperature PID point
	 Solder_Iron_Sleep_Time_Resset(self);				//Reset sleep timer
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Iron *self){
	/**
	 * Set Manual_Preset_Temperature temperature for Solder_Iron in Celsius
	 */
	Set_Manual_Preset_Temperature_C(value, &self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
void Solder_Iron_Full_Power_ON(struct Soldering_Iron *self){
	/**
	 * Turn ON Full_Power
	 */
	self->Full_Power_State=Full_Power_ON;
	Solder_Iron_Set_MODE_Temperature(self, self->MODE);	//Set temperature PID point
	Solder_Iron_Sleep_Time_Resset(self);				//Reset sleep timer
}
//---------------------------------------------------------------------------------
void Solder_Iron_Full_Power_OFF(struct Soldering_Iron *self){
	/**
	 * Turn OFF Full_Power
	 */
	self->Full_Power_State=Full_Power_OFF;
	Solder_Iron_Set_MODE_Temperature(self, self->MODE);	//Set temperature PID point
	Solder_Iron_Sleep_Time_Resset(self);				//Reset sleep timer
}
//---------------------------------------------------------------------------------
enum Full_Power_State Solder_Iron_Get_Full_Power_State(struct Soldering_Iron *self){
	/**
	 * Return Full_Powr_State	(Full_Power_ON,Full_Power_OFF)
	 */
	return self->Full_Power_State;
}
//---------------------------------------------------------------------------------
enum Soldering_Iron_State Solder_Iron_Get_State(struct Soldering_Iron *self){
	/**
	 * Return Solder_Iron_State (Soldering_Iron_ON Soldering_Iron_OFF)
	 */
	return self->State;
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Iron_Get_Manual_Preset_Temperature_C(struct Soldering_Iron *self){
	/*
	 * Return manual preset preset temperature in celsius
	 */
	return Get_Manual_Preset_Temperature_C(&self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Iron_Get_Temperature_Set_C(struct Soldering_Iron *self){
	/*
	 * Return Current Set temperature in celsius
	 */
	return Get_Temperature_Set_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Iron_Get_Current_Temperature_C(struct Soldering_Iron *self){
	/*
	 * Return Current temperature in celsius
	 */
	return Get_Current_Temperature_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
void Vibration_Sensor_it(struct Soldering_Iron *self){
	/**
	 * Vibration sensor have not any check
	 * Main function reset Sleep timer when rising / falling edge
	 */
	self->VS.State=HAL_GPIO_ReadPin(self->VS.GPIO,self->VS.EXTI_PIN);						//Read Pin State
		if(self->VS.State!=self->VS.Prew_State){											//IF ReadState!=Prew_Stae
			Solder_Iron_Sleep_Time_Resset(self);											//Sleep Timer Reset and current MODE(sleep) set previous mode(Manual/Preset1/Preset2)
			Solder_Iron_Set_MODE_Temperature(self,self->MODE);								//Set previous mode temperature before sleep mode temperature;
		}
		self->VS.Prew_State=self->VS.State;													//Prew State = Read State
}
//---------------------------------------------------------------------------------------------------------------------------------// Solder_Iron Flash
uint32_t Solder_Iron_Flash_Write_Struct(struct Soldering_Iron *self,uint32_t addr){
	/**
	 * Using to write main structure Soldering_Iron into Flash Memory for an address
	 * return address of the end of writing
	 */
	return Flash_Write (addr, self, sizeof(*self));
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Solder_Iron_Flash_Read_Struct(struct Soldering_Iron *self,uint32_t addr){
	/**
	 * Using to read main structure Soldering_Iron from Flash Memory for an address
	 * return HAL_OK is read  struct was successful  and HAL_ERROR if not
	 * If reading is correct function write all data into a structure
	 */
	struct Soldering_Iron Read_Struct;
	Flash_Read (addr, &Read_Struct, sizeof(*self));			//Read struct into copy
	if(Read_Struct.Flash_Key==Soldering_Iron_Struct_Key){	//Check if wrote data is a struct
		*self=Read_Struct;
		return HAL_OK;
	}else
		return HAL_ERROR;
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Solder_Iron_Flash_Read_Manual_Temperature(struct Soldering_Iron *self){
	/**
	 * Using to read structure Temperature_Presets from Flash Memory for an Page
	 * return HAL_OK or HAL_ERROR
	 */
	return Flash_Read_Manual_Temperature(&self->Temperature_Presets_C, Soldering_Iron_Manual_Temperature_Flash_Key, Soldering_Iron_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Solder_Iron_Flash_Reset_Rewrite_Timer(struct Soldering_Iron *self){
	/**
	 * This function reset rewrite timer.
	 */
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//-----------------------------------------------------------------------------------------------------------------------------------// Heat gun
void Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(struct Soldering_Heat_Gun *self);
void Soldering_Heat_Gun_Temperature_Coolling_Converting(struct Soldering_Heat_Gun *self);
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_ini(struct Soldering_Heat_Gun *self){
	/**
	 * Basic initialization of all part for starting all processes
	 */
	PID_ini(&self->PID);
	Filter_ini(&self->Filter);
	Flash_Rewrite_Timer_ini(&self->Flash_Rewrite_Timer);
	Soldering_Heat_Gun_Set_Manual_Temperature(self);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_ON(struct Soldering_Heat_Gun *self){
	/**
	 * Turn ON Heat Gun
	 */

	PAC_Device_Control_ON(self->PAC_Control);	//Turn ON Phase Angle Control
	PID_ini(&self->PID);						//Reset PID
	Filter_ini(&self->Filter);					//Reset Filter

	HAL_GPIO_WritePin(self->OFF_GPIO, self->OFF_PIN, GPIO_PIN_RESET);		//Set manual mode to FAN rotating speed controll
	HAL_GPIO_WritePin(self->MANUAL_GPIO, self->MANUAL_PIN, GPIO_PIN_SET);

	self->State=Heat_Gun_ON;					//Set turn ON State
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_OFF(struct Soldering_Heat_Gun *self){
	/**
	 * Turn OFF Heat Gun
	 */
	Soldering_Heat_Gun_Temperature_Coolling_Converting(self);
	self->State=Heat_Gun_COOLING;					//Set COOLING State. It needs to cool heat gun to Temperature_Coolling. After set Turn OFF state
	PAC_Device_Control_OFF(self->PAC_Control);											// Turn OFF Phase Angle Control
	HAL_GPIO_WritePin(self->MANUAL_GPIO, self->MANUAL_PIN, GPIO_PIN_RESET);	 	//Turn ON Full Speed FAN rotation
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_MODE_Temperature(struct Soldering_Heat_Gun *self,enum MODE MODE){
	/**
	 * Heat_Gun set temperature for PID based on mode
	 * Needs set mode struct.mode(MANUAL/PRESET1..) Before use
	 */
	self->MODE=MODE;
	PID_Set_Point(&self->PID, Set_Temperature(&self->Temperature_Presets_C, &self->Temperature_Converting, &self->MODE));
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Temperature_Presets_Corection_Converting(struct Soldering_Heat_Gun *self){
	/**
	 * Converting Soldering_Heat_Gun_Temperature_Presets_Corection from Celsius to ADC Levels
	 */
	Temperature_Presets_Corection_Converting(&self->Temperature_Corection_C, &self->Temperature_Corection_ADC, &self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Temperature_Coolling_Converting(struct Soldering_Heat_Gun *self){
	self->Temperature_Coolling.ADC=(uint16_t)(self->Temperature_Coolling.C*self->Temperature_Converting.Coeff+self->Temperature_Converting.Coeff/2);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_Manual_Temperature(struct Soldering_Heat_Gun *self){
	/**
	 * Set manual temperature for Soldering_Heat_Gun and set Manual MODE
	 */
	if(self->Temperature_Presets_C.Prew_Set_Temperature_Manual!=self->Temperature_Presets_C.Manual){
		Soldering_Heat_Gun_Set_MODE_Temperature(self, MANUAL);	//Set temperature PID point
		Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(self);		//Reset rewrite timer
	}
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_Preset1_Temperature(struct Soldering_Heat_Gun *self){
	/**
	 * Set Preset1 temperature for Soldering_Heat_Gun and set Preset1 MODE
	 */
	Soldering_Heat_Gun_Set_MODE_Temperature(self, PRESET1);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_Preset2_Temperature(struct Soldering_Heat_Gun *self){
	/**
	 * Set Preset2 temperature for Soldering_Heat_Gun and set Preset2 MODE
	 */
	Soldering_Heat_Gun_Set_MODE_Temperature(self, PRESET2);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Heat_Gun *self){
	/**
	 * Set manual temperature for Soldering_Heat_Gun in Celsius
	 */
	Set_Manual_Preset_Temperature_C(value, &self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_PID_MAX_Control_Value(uint16_t MAX_Control_Value, struct Soldering_Heat_Gun *self){
	/**
	 * Set PID's max control value
	 */
	PID_Set_MAX_Control(MAX_Control_Value, &self->PID);
}
//---------------------------------------------------------------------------------
enum Soldering_Heat_Gun_State Soldering_Heat_Gun_Get_State(struct Soldering_Heat_Gun *self){
	/**
	 * Return Soldering_Heat_Gun_State (Heat_Gun_OFF Heat_Gun_ON Heat_Gun_COOLING)
	 */
	return self->State;
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Heat_Gun_Get_Manual_Preset_Temperature_C(struct Soldering_Heat_Gun *self){
	/*
	 * Return manual preset preset temperature in celsius
	 */
	return Get_Manual_Preset_Temperature_C(&self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Heat_Gun_Get_Temperature_Set_C(struct Soldering_Heat_Gun *self){
	/*
	 * Return Current set temperature  preset temperature in celsius
	 */
	return Get_Temperature_Set_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Heat_Gun_Get_Current_Temperature_C(struct Soldering_Heat_Gun *self){
	/*
	 * Return Current temperature temperature in celsius
	 */
	return Get_Current_Temperature_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
enum PAC_Device_Control_State Soldering_Heat_Gun_Get_PAC_Controll_State(struct Soldering_Heat_Gun *self){
	/**
	 * return  Soldering_Heat_Gun_Get_PAC_Controll_State(PAC_Control_OFF PAC_Control_ON)
	 */
	return PAC_Device_Control_Get_State(self->PAC_Control);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Heat_Gun_Get_PAC_Controll_Value(struct Soldering_Heat_Gun *self){
	/**
	 * return Soldering_Heat_Gun PAC's control value
	 */
	return PAC_Device_Control_Get_Control_Value(self->PAC_Control);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_it(struct Soldering_Heat_Gun *self){
	/**
	 * Main function PID and Filter iteration for Soldering_Heat_Gun
	 * Secondary Cooling Heat GUN
	 */
	if(self->State==Heat_Gun_ON&&PAC_Get_State(self->PAC)==ZCD_STATE_ON){									//if Heat GUN turn ON && ZCD ON
		PID_Set_Curent_Point(&self->PID,(int)Temperatur_Correction(Filter_Combined(&self->Filter), &self->Temperature_Converting, &self->Temperature_Corection_ADC, &self->MODE));
																										//Filter and set current Temperature and Heat_Gun PID IT
		PAC_Device_Control_Set_Control_Value(self->PAC_Control, (uint16_t)PID_it(&self->PID));							//Set PAC control value
	}else if(self->State==Heat_Gun_COOLING){															//if Heat GUN Cooling
		if(Temperatur_Correction(Filter_Combined(&self->Filter), &self->Temperature_Converting, &self->Temperature_Corection_C, &self->MODE)<=self->Temperature_Coolling.ADC){	//and set current Temperature											//if the current temperature is less than  Temperature_Coolling
			self->State=Heat_Gun_OFF;																	//Turn off heat gun
			HAL_GPIO_WritePin(self->OFF_GPIO, self->OFF_PIN, GPIO_PIN_SET);								// Turn OF FAN rotating
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------// Heat_Gun Flash
uint32_t Soldering_Heat_Gun_Flash_Write_Struct(struct Soldering_Heat_Gun *self,uint32_t addr){
	/**
	 * Using to write main structure Soldering_Heat_Gun into Flash Memory for an address
	 * return address of the end of writing
	 */
	return Flash_Write (addr, self, sizeof(*self));
}
//---------------------------------------------------------------------------------
uint8_t Soldering_Heat_Gun_Flash_Read_Struct(struct Soldering_Heat_Gun *self,uint32_t addr){
	/**
	 * Using to read main structure Soldering_Heat_Gun from Flash Memory for an address
	 * return HAL_OK is read  struct was successful  and HAL_ERROR if not
	 * If reading is correct function write all data into a structure
	 */
	struct Soldering_Heat_Gun Read_Struct;
	Flash_Read (addr, &Read_Struct, sizeof(*self));						//Read struct into copy
	if(Read_Struct.Flash_Key==Soldering_Heat_Gun_Struct_Key){			//Check if wrote data is a struct
		*self=Read_Struct;
		return HAL_OK;
	}else
		return HAL_ERROR;
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Soldering_Heat_Gun_Flash_Read_Manual_Temperature(struct Soldering_Heat_Gun *self){
	/**
	 * Using to read structure Temperature_Presets from Flash Memory for an Page
	 * return HAL_OK or HAL_ERROR
	 */
	return Flash_Read_Manual_Temperature(&self->Temperature_Presets_C, Soldering_Heat_Gun_Manual_Temperature_Flash_Key, Soldering_Heat_Gun_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(struct Soldering_Heat_Gun *self){
	/**
	 * This function reset rewrite timer.
	 */
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------------------------------------------------------//Soldering_Separator
void Soldering_Separator_Flash_Reset_Rewrite_Timer(struct Soldering_Separator *self);
//---------------------------------------------------------------------------------
void Soldering_Separator_ini(struct Soldering_Separator *self){
	/**
	 * Basic initialization of all part for starting all processes
	 */
	PID_ini(&self->PID);
	Filter_ini(&self->Filter);
	Flash_Rewrite_Timer_ini(&self->Flash_Rewrite_Timer);
	 Soldering_Separator_Set_Manual_Temperature(self);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_ON(struct Soldering_Separator *self){
	/**
	 * Turn ON Soldering_Separator
	 */
	PAC_Device_Control_ON(self->PAC_Control);				//Turn ON Phase Angle Control
	PID_ini(&self->PID);							//Reset PID
	Filter_ini(&self->Filter);						//Reset Filter
	self->State=Separator_ON;						//Set Turn ON State
}
//---------------------------------------------------------------------------------
void Soldering_Separator_OFF(struct Soldering_Separator *self){
	/**
	 * Turn OFF Soldering Separator
	 */
	PAC_Device_Control_OFF(self->PAC_Control); 				// Turn OFF Phase Angle Control
	self->State=Separator_OFF;						// Set Soldering_Separator Turn OFF State
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_MODE_Temperature(struct Soldering_Separator *self, enum MODE MODE){
	/**
	 * Soldering_Separator set temperature for PID based on mode
	 * Needs set mode struct.mode(MANUAL/PRESET1..) Before use
	 */
	self->MODE=MODE;
	PID_Set_Point(&self->PID, Set_Temperature(&self->Temperature_Presets_C, &self->Temperature_Converting, &self->MODE));
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Temperature_Presets_Corection_Converting(struct Soldering_Separator *self){
	Temperature_Presets_Corection_Converting(&self->Temperature_Corection_C, &self->Temperature_Corection_ADC, &self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_Manual_Temperature(struct Soldering_Separator *self){
	/**
	 * Set manual temperature for oldering_Separator and set Manual MODE
	 */
	if(self->Temperature_Presets_C.Prew_Set_Temperature_Manual!=self->Temperature_Presets_C.Manual){
		Soldering_Separator_Set_MODE_Temperature(self, MANUAL);	//Set temperature PID point
		Soldering_Separator_Flash_Reset_Rewrite_Timer(self);	//Reset rewrite Timer
	}
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_Preset1_Temperature(struct Soldering_Separator *self){
	/**
	 * Set Preset1 temperature for Soldering_Separator and set Preset1 MODE
	 */
	Soldering_Separator_Set_MODE_Temperature(self, PRESET1);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_Preset2_Temperature(struct Soldering_Separator *self){
	/**
	 * Set Preset2 temperature for Soldering_Separator and set Preset1 MODE
	 */
	Soldering_Separator_Set_MODE_Temperature(self, PRESET2);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_Manual_Preset_Temperature_C(uint16_t value, struct Soldering_Separator *self){
	/**
	 * Set manual preset temperature for Soldering_Separator in Celsius
	 */
	Set_Manual_Preset_Temperature_C(value, &self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
enum Soldering_Separator_State Soldering_Separator_Get_State(struct Soldering_Separator *self){
	/**
	 * return Soldering_Separator_State (Separator_OFF,Separator_ON)
	 */
	return self->State;
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Separator_Get_Temperature_Set_C(struct Soldering_Separator *self){
	/*
	 * Return Current set temperature preset temperature in celsius
	 */
	return Get_Temperature_Set_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Separator_Get_Current_Temperature_C(struct Soldering_Separator *self){
	/*
	 * Return Current temperature temperature in celsius
	 */
	return Get_Current_Temperature_C(&self->Temperature_Converting);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Separator_Get_Manual_Preset_Temperature_C(struct Soldering_Separator *self){
	/*
	 * Return manual preset temperature in celsius
	 */
	return Get_Manual_Preset_Temperature_C(&self->Temperature_Presets_C);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_PID_MAX_Control_Value(uint16_t MAX_Control_Value, struct Soldering_Separator *self){
	/**
	 * Set PID's max control value
	 */
	PID_Set_MAX_Control(MAX_Control_Value, &self->PID);
}
//---------------------------------------------------------------------------------
enum PAC_Device_Control_State Soldering_Separator_Get_PAC_Controll_State(struct Soldering_Separator *self){
	/**
	 * return  Soldering_Heat_Gun_Get_PAC_Controll_State(PAC_Control_OFF PAC_Control_ON)
	 */
	return PAC_Device_Control_Get_State(self->PAC_Control);
}
//---------------------------------------------------------------------------------
uint16_t Soldering_Separator_Get_PAC_Controll_Value(struct Soldering_Separator *self){
	/**
	 * return Soldering_Separator PAC's control value
	 */
	return PAC_Device_Control_Get_Control_Value(self->PAC_Control);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_it(struct Soldering_Separator *self){
	/**
	 * Main function PID and Filter iteration for Soldering_Separator
	 *
	 */
	if(self->State==Separator_ON&&PAC_Get_State(self->PAC)==ZCD_STATE_ON){				//if Heat Soldering_Separator turn ON && ZCD ON
		PID_Set_Curent_Point(&self->PID,(int)Temperatur_Correction(Filter_Combined(&self->Filter), &self->Temperature_Converting, &self->Temperature_Corection_ADC, &self->MODE));
																						//Filter and set current Temperature and Soldering_Separator PID IT;
		PAC_Device_Control_Set_Control_Value(self->PAC_Control, (uint16_t)PID_it(&self->PID));			//Set PAC control value
	}
}
//---------------------------------------------------------------------------------------------------------------------------------// Soldering_Separator Flash
uint32_t Soldering_Separator_Flash_Write_Struct(struct Soldering_Separator *self,uint32_t addr){
	/**
	 * Write general Soldering_Separator struct
	 */
	return Flash_Write (addr, self, sizeof(*self));
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Soldering_Separator_Flash_Read_Struct(struct Soldering_Separator *self,uint32_t addr){
	/**
	 * Read general Soldering_Separator struct
	 * return HAL_OK or HAL_ERROR
	 */
	struct Soldering_Separator Read_Struct;
	Flash_Read (addr, &Read_Struct, sizeof(*self));
	if(Read_Struct.Flash_Key==Soldering_Separator_Struct_Key){
		*self=Read_Struct;
		return HAL_OK;
	}else
		return HAL_ERROR;
}
//---------------------------------------------------------------------------------
uint32_t Soldering_Separator_Flash_Write_Manual_Temperature(struct Soldering_Separator *self){
	/**
	 * Write manual Temperature
	 */
	return Write_Manual_Temperature(&self->Temperature_Presets_C, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Soldering_Separator_Flash_Read_Manual_Temperature(struct Soldering_Separator *self){
	/**
	 * Read manual Temperature
	 * return HAL_OK or HAL_ERROR
	 */
	return Flash_Read_Manual_Temperature(&self->Temperature_Presets_C, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Flash_Reset_Rewrite_Timer(struct Soldering_Separator *self){
	/**
	 * rewrite Soldering_Separator's flash timer
	 */
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------------------------------------------------------//Soldering_Station
void Soldering_Station_Temperature_Presets_Corection_Converting(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){
	/**
	 * Convert Temperature_Presets_Corection_C (celsius) to Temperature_Presets_Corection_ADC For all part of soldering station
	 */
	Solder_Iron_Temperature_Presets_Corection_Converting(Soldering_Iron);
	Soldering_Heat_Gun_Temperature_Presets_Corection_Converting(Soldering_Heat_Gun);
	Soldering_Separator_Temperature_Presets_Corection_Converting(Soldering_Separator);

}
//---------------------------------------------------------------------------------------------------------------------------------
HAL_StatusTypeDef Soldering_Station_Read_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){
	/**
	 * 	Read all general structures from flash
	 * 	return HAL_OK or HAL_ERROR
	 */
	HAL_StatusTypeDef Status=HAL_OK;
	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Soldering_Station_ALL_Struct_Flash_Page;	//computing start read addr
	Status|=Solder_Iron_Flash_Read_Struct(Soldering_Iron,addr);										//read Solder_Iron general struct
	addr+=sizeof(*Soldering_Iron)+2;																//Address shift for prew read struct
	Status|=Soldering_Heat_Gun_Flash_Read_Struct(Soldering_Heat_Gun,addr);							//read Soldering_Heat_Gun general struct
	addr+=sizeof(*Soldering_Heat_Gun)+2;															//Address shift for prew read struct
	Status|=Soldering_Separator_Flash_Read_Struct(Soldering_Separator,addr);						//read Soldering_Separator general struct

	return Status;
}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Soldering_Station_Write_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){
	/**
	 *	Write all general structures from flash
	 * 	return HAL_OK or HAL_ERROR
	 */
	HAL_StatusTypeDef Status=HAL_OK;

	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Soldering_Station_ALL_Struct_Flash_Page;	//computing start read addr

	Flash_Erase (addr,sizeof(*Soldering_Iron)+2+sizeof(*Soldering_Heat_Gun)+2+sizeof(*Soldering_Separator));

	if(Solder_Iron_Flash_Write_Struct(Soldering_Iron,addr)==HAL_ERROR)					//write Solder_Iron general struct
		Status|=HAL_ERROR;
	addr+=sizeof(*Soldering_Iron)+2;													//Address shift for prew read struct
	if(Soldering_Heat_Gun_Flash_Write_Struct(Soldering_Heat_Gun,addr)==HAL_ERROR)		//write Soldering_Heat_Gun general struct
		Status|=HAL_ERROR;
	addr+=sizeof(*Soldering_Heat_Gun)+2;												//Address shift for prew read struct
	if(Soldering_Separator_Flash_Write_Struct(Soldering_Separator,addr)==HAL_ERROR)		//write Soldering_Separator general struct
		Status|=HAL_ERROR;

	return Status;
}
//---------------------------------------------------------------------------------
uint32_t Write_Manual_Temperature(struct Temperature_Presets *self, uint32_t Flash_Key, uint8_t Flash_Page){
	/**
	 * write manual temperature parameter
	 * return end address
	 */
	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Flash_Page;	//computing start write addr

	Flash_Erase (addr,sizeof(self->Manual)+sizeof(Flash_Key));			//Erese PAGE

	Flash_Write (addr,&self->Manual, sizeof(self->Manual));				//Write manual temperature parameter

	addr+=sizeof(self->Manual)+2;										//Address shift for prew read struct

	return Flash_Write (addr,&Flash_Key, sizeof(Flash_Key));			//Write Flash_Key

}
//---------------------------------------------------------------------------------
HAL_StatusTypeDef Flash_Read_Manual_Temperature(struct Temperature_Presets *self, uint32_t Flash_Key, uint8_t Flash_Page){
	/**
	 * Read Manual_Temperature parameter from flash
	 * return HAL_OK or HAL_ERROR
	 */
	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Flash_Page;		//computing start read addr
	uint32_t Flash_Read_Key=0;
	uint16_t Temperature;

	Flash_Read (addr, &Temperature, sizeof(self->Manual));					//read manual temperature
	addr+=sizeof(self->Manual)+2;											//Address shift for prew read struct
	Flash_Read (addr, &Flash_Read_Key,  sizeof(Flash_Read_Key));			//Read flash_key;

	if(Flash_Read_Key==Flash_Key){											//if flash key was read success
		self->Manual=Temperature;											//write temperature into struct
		return HAL_OK;
	}
	else
		return HAL_ERROR;
}
//---------------------------------------------------------------------------------
void Flash_Reset_Rewrite_Timer(struct Flash_Rewrite_Timer *self){
	/**
	 * Reset Rewrite_Timer
	 */
	self->Flash_Timer_Count=0;
	self->Flash_Timer_State=Flash_Timer_State_ON;
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_it(struct Flash_Rewrite_Timer *Flash_Rewrite_Timer, struct Temperature_Presets *Temperature_Presets,uint32_t Flash_Key, uint8_t Flash_Page){
	/**
	 * Falsh rewrite timer iteration
	 */
	if(Flash_Rewrite_Timer->Flash_Timer_State==Flash_Timer_State_ON){					//if timer turn ON
		if(Flash_Rewrite_Timer->Flash_Timer_Count++==Flash_Rewrite_Timer_Max_Count){	//count++ & equal Flash_Rewrite_Timer_Max_Count
			Write_Manual_Temperature(Temperature_Presets, Flash_Key, Flash_Page);		//write manual temperature
			Flash_Rewrite_Timer->Flash_Timer_State=Flash_Timer_State_OFF;				//Turn OFF timer
		}
	}
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_tim_it(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){
	/**
	 * Falsh rewrite timer iteration for ALL Soldering_Station parts
	 */
	Flash_Rewrite_Timer_it(&Soldering_Iron->Flash_Rewrite_Timer, &Soldering_Iron->Temperature_Presets_C, Soldering_Iron_Manual_Temperature_Flash_Key, Soldering_Iron_Manual_Temperature_Flash_Page);
	Flash_Rewrite_Timer_it(&Soldering_Heat_Gun->Flash_Rewrite_Timer, &Soldering_Heat_Gun->Temperature_Presets_C, Soldering_Heat_Gun_Manual_Temperature_Flash_Key, Soldering_Heat_Gun_Manual_Temperature_Flash_Page);
	Flash_Rewrite_Timer_it(&Soldering_Separator->Flash_Rewrite_Timer, &Soldering_Separator->Temperature_Presets_C, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_ini(struct Flash_Rewrite_Timer *self){
	/**
	 * Rewrite timer ini
	 */
	self->Flash_Timer_Count=0;
	self->Flash_Timer_State=Flash_Timer_State_OFF;
}
//---------------------------------------------------------------------------------







