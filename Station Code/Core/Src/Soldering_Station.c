#include "Soldering_Station.h"
//---------------------------------------------------------------------------------
uint16_t Set_Temperature(uint16_t *Temperature_Set, enum MODE *MODE,struct Temperature_Pressets *Temperature_Pressets, struct Temperature_Corection *Temperature_Corection){
	/**
	 * Temperature_Setresul value;
	 * MODE - MANUAL PRESSET1 PRESSET2 SLEEP
	 * struct Temperature_Pressets *Temperature_Pressets struct wich contain all MODE presets;
	 */

	switch(*MODE){
			case MANUAL:
				*Temperature_Set=Temperature_Pressets->Manual;
				return (uint16_t)*Temperature_Set+Temperature_Corection->Manual;
				break;
			case PRESSET1:
				*Temperature_Set=Temperature_Pressets->Presset1;
				return (uint16_t)*Temperature_Set+Temperature_Corection->Presset1;
				break;
			case PRESSET2:
				*Temperature_Set=Temperature_Pressets->Presset2;
				return (uint16_t)*Temperature_Set+Temperature_Corection->Presset2;
				break;
			case SLEEP:
				*Temperature_Set=Temperature_Pressets->Sleep;
				return (uint16_t)*Temperature_Set;
				break;
		}
	return 0;
}
//---------------------------------------------------------------------------------
void Vibration_Sensor_it(struct Soldering_Iron *self);
//---------------------------------------------------------------------------------
void Solder_Iron_it(struct Soldering_Iron *self){
	/**
	 * PID and Filter iteration for Soldering_Iron
	 */
	Vibration_Sensor_it(self);			// We can read state of Vibration Sensor only when PWM is stopped
	Button_it(&self->Full_Power_Button); // We can read state of button only when PWM is stopped
	if(self->State==1){
	  HAL_TIM_PWM_Start(self->PWM_htim, TIM_CHANNEL_4);
	  PID_Set_Point_Now(&self->PID,(int)(self->Temperature=Filter_Combined(&self->Filter))); //PID it
	  self->PWM_htim->Instance->CCR4=PID_it(&self->PID);
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
	static uint8_t Button_prescaler=0;
	static uint8_t TIM_prescaler=0;

	if(TIM_prescaler++==10){
		if(self->State){
			if(self->Timer++==self->Sleep_time){						//if Time more than Sleep time value set sleep mode
				self->PREW_MODE=self->MODE;
				Solder_Iron_Set_Temperature(self, SLEEP);						//Set Sleep Mode Temperature preset
			}else if(self->Timer==self->Disable_time+self->Sleep_time)//if time more than Disable_time+Sleep_time Turn OFF Soldering_Iron
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
}

//---------------------------------------------------------------------------------
void Solder_Iron_ON(struct Soldering_Iron *self){
	Solder_Iron_Sleep_Time_Resset(self);  	//Reset Sleep_timer
	Solder_Iron_Set_Temperature(self,self->MODE);		//Set Temperature
	self->State=Soldering_Iron_ON;
}
//---------------------------------------------------------------------------------
void Solder_Iron_OFF(struct Soldering_Iron *self){
	self->State=Soldering_Iron_OFF;
	Solder_Iron_Sleep_Time_Resset(self); 	//Reset Sleep_timer
	Solder_Iron_ini(self);					//Reset All Spldering Iron parameters
}
//---------------------------------------------------------------------------------
void Solder_Iron_Set_Temperature(struct Soldering_Iron *self, enum MODE MODE){
	/**
	 * Solder_Iron_Set_Temperature based on MODE and Full_Power_Button
	 * Set Point for PID
	 */
	uint16_t Temperature_Corected;
	self->MODE=MODE;
	Temperature_Corected=Set_Temperature(&self->Temperature_Set, &self->MODE,&self->Temperature_Pressets, &self->Temperature_Corection);
	if(self->Full_Power_State==1){
		if((self->Temperature_Set+self->Add_Temperature)<MAX_ADC_Value){
			self->Temperature_Set+=self->Add_Temperature;
			Temperature_Corected+=self->Add_Temperature;
		}
		else{
			self->Temperature_Set=MAX_ADC_Value;
			Temperature_Corected=MAX_ADC_Value;
		}
	}
	PID_Set_Point(&self->PID, Temperature_Corected);				//Set Point for PID
}
//---------------------------------------------------------------------------------
void Vibration_Sensor_it(struct Soldering_Iron *self){
	/**
	 * Vibration sensor have not any check
	 * Main function reset Sleep timer when rising / falling edge
	 */
	self->VS.State=HAL_GPIO_ReadPin(self->VS.GPIO,self->VS.EXTI_PIN);					//Read Pin State
		if(self->VS.State!=self->VS.Prew_State){											//IF ReadState!=Prew_Stae
			Solder_Iron_Sleep_Time_Resset(self);											//Sleep Timer Reset and current MODE(sleep) set previous mode(Manual/Preset1/Preset2)
			Solder_Iron_Set_Temperature(self,self->MODE);												//Set previous mode temperature before sleep mode temperature;
		}
		self->VS.Prew_State=self->VS.State;												//Prew State = Read State
}
//---------------------------------------------------------------------------------				// Solder_Iron Flash
uint32_t Solder_Iron_Flash_Write_Struct(struct Soldering_Iron *self,uint32_t addr){
	/**
	 * Using to write main structure Soldering_Iron into Flash Memory for an address
	 * return address of the end of writing
	 */
	return Flash_Write (addr, self, sizeof(*self));
}
//---------------------------------------------------------------------------------
uint8_t Solder_Iron_Flash_Read_Struct(struct Soldering_Iron *self,uint32_t addr){
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
uint8_t Solder_Iron_Flash_Read_Manual_Temperature(struct Soldering_Iron *self){
	/**
	 * Using to read structure Temperature_Pressets from Flash Memory for an Page
	 * return HAL_OK or HAL_ERROR
	 */
	return Flash_Read_Manual_Temperature(&self->Temperature_Pressets, Soldering_Iron_Manual_Temperature_Flash_Key, Soldering_Iron_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Solder_Iron_Flash_Reset_Rewrite_Timer(struct Soldering_Iron *self){
	/**
	 * This function reset rewrite timer.
	 */
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//-----------------------------------------------------------------------------------------------------------------------------------// Heat gun
void Soldering_Heat_Gun_ini(struct Soldering_Heat_Gun *self){
	/**
	 * Basic initialization of all part for starting all processes
	 */
	PID_ini(&self->PID);
	Filter_ini(&self->Filter);
	Flash_Rewrite_Timer_ini(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_ON(struct Soldering_Heat_Gun *self){
	/**
	 * Turn ON Heat Gun
	 */
	PAC_ON(self->PAC);							//Turn ON Phase Angle Control
	self->PAC_Control->State=PAC_Control_ON;	//Turn ON Control value for Phase Angle Control
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
	self->State=Heat_Gun_COOLING;					//Set COOLING State. It needs to cool heat gun to Temperature_Coolling. After set Turn OFF state
	self->PAC_Control->State=PAC_Control_OFF;		//Turn OFF Control value for Phase Angle Control
	PAC_OFF(self->PAC);								// Turn OFF Phase Angle Control
	HAL_GPIO_WritePin(self->PAC_Control->GPIO, self->PAC_Control->PIN, GPIO_PIN_RESET); //Turn ON Full Speed FAN rotation
	HAL_GPIO_WritePin(self->MANUAL_GPIO, self->MANUAL_PIN, GPIO_PIN_RESET);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Set_Temperature(struct Soldering_Heat_Gun *self,enum MODE MODE){
	/**
	 * Heat_Gun set temperature for PID based on mode
	 * Needs set mode struct.mode(MANUAL/PRESET1..) Before use
	 */
	self->MODE=MODE;
	PID_Set_Point(&self->PID, Set_Temperature(&self->Temperature_Set, &self->MODE,&self->Temperature_Pressets, &self->Temperature_Corection));
}

//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_it(struct Soldering_Heat_Gun *self){
	/**
	 * Main function PID and Filter iteration for Soldering_Heat_Gun
	 * Secondary Cooling Heat GUN
	 */
	if(self->State==Heat_Gun_ON&&self->PAC->ZCD->State==ZCD_STATE_ON){									//if Heat GUN turn ON && ZCD ON
		self->Temperature=Filter_Combined(&self->Filter);												//Filter and set current Temperature
		PID_Set_Point_Now(&self->PID,(int)(self->Temperature));											//Heat_Gun PID IT;
		self->PAC_Control->Control_Value=(uint16_t)PID_it(&self->PID);									//Set PAC control value
	}else if(self->State==Heat_Gun_COOLING){															//if Heat GUN Cooling
		self->Temperature=Filter_Combined(&self->Filter);												//and set current Temperature
		if(self->Temperature<=self->Temperature_Coolling){												//if the current temperature is less than  Temperature_Coolling
			self->State=Heat_Gun_OFF;																	//Turn off heat gun
			HAL_GPIO_WritePin(self->OFF_GPIO, self->OFF_PIN, GPIO_PIN_SET);								// Turn OF FAN rotating
		}
	}
}
//---------------------------------------------------------------------------------						// Heat_Gun Flash
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
uint8_t Soldering_Heat_Gun_Flash_Read_Manual_Temperature(struct Soldering_Heat_Gun *self){
	/**
	 * Using to read structure Temperature_Pressets from Flash Memory for an Page
	 * return HAL_OK or HAL_ERROR
	 */
	return Flash_Read_Manual_Temperature(&self->Temperature_Pressets, Soldering_Heat_Gun_Manual_Temperature_Flash_Key, Soldering_Heat_Gun_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Soldering_Heat_Gun_Flash_Reset_Rewrite_Timer(struct Soldering_Heat_Gun *self){
	/**
	 * This function reset rewrite timer.
	 */
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------------------------------------------------------//Soldering_Separator
void Soldering_Separator_ini(struct Soldering_Separator *self){
	/**
	 * Basic initialization of all part for starting all processes
	 */
	PID_ini(&self->PID);
	Filter_ini(&self->Filter);
	Flash_Rewrite_Timer_ini(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_ON(struct Soldering_Separator *self){
	/**
	 * Turn ON Soldering_Separator
	 */
	PAC_ON(self->PAC);								//Turn ON Phase Angle Control
	self->PAC_Control->State=PAC_Control_ON;		//Turn ON Control value for Phase Angle Control
	PID_ini(&self->PID);							//Reset PID
	Filter_ini(&self->Filter);						//Reset Filter
	self->State=Separator_ON;						//Set Turn ON State
}
//---------------------------------------------------------------------------------
void Soldering_Separator_OFF(struct Soldering_Separator *self){
	/**
	 * Turn OFF Soldering Separator
	 */
	self->PAC_Control->State=PAC_Control_OFF; 		//Turn OFF Control value for Phase Angle Control
	PAC_OFF(self->PAC);								// Turn OFF Phase Angle Control
	self->State=Separator_OFF;						// Set Soldering_Separator Turn OFF State
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Set_Temperature(struct Soldering_Separator *self, enum MODE MODE){
	/**
	 * Soldering_Separator set temperature for PID based on mode
	 * Needs set mode struct.mode(MANUAL/PRESET1..) Before use
	 */
	self->MODE=MODE;
	PID_Set_Point(&self->PID, Set_Temperature(&self->Temperature_Set, &self->MODE,&self->Temperature_Pressets, &self->Temperature_Corection));
}

//---------------------------------------------------------------------------------
void Soldering_Separator_it(struct Soldering_Separator *self){
	/**
	 * Main function PID and Filter iteration for Soldering_Separator
	 *
	 */
	if(self->State==Separator_ON&&self->PAC->ZCD->State==ZCD_STATE_ON){				//if Heat GUN turn ON && ZCD ON
		self->Temperature=Filter_Combined(&self->Filter);							//Filter and set current Temperature
		PID_Set_Point_Now(&self->PID,(int)(self->Temperature));						//Soldering_Separator PID IT;
		self->PAC_Control->Control_Value=(uint16_t)PID_it(&self->PID);				//Set PAC control value
	}
}
//---------------------------------------------------------------------------------						// Soldering_Separator Flash
uint32_t Soldering_Separator_Flash_Write_Struct(struct Soldering_Separator *self,uint32_t addr){
	return Flash_Write (addr, self, sizeof(*self));
}
//---------------------------------------------------------------------------------
uint8_t Soldering_Separator_Flash_Read_Struct(struct Soldering_Separator *self,uint32_t addr){
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
	return Write_Manual_Temperature(&self->Temperature_Pressets, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
uint8_t Soldering_Separator_Flash_Read_Manual_Temperature(struct Soldering_Separator *self){
	return Flash_Read_Manual_Temperature(&self->Temperature_Pressets, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Soldering_Separator_Flash_Reset_Rewrite_Timer(struct Soldering_Separator *self){
	Flash_Reset_Rewrite_Timer(&self->Flash_Rewrite_Timer);
}
//---------------------------------------------------------------------------------------------------------------------------------//FLASH Read ALL_Struct
uint8_t Soldering_Station_Read_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){

	uint8_t Status=HAL_OK;
	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Soldering_Station_ALL_Struct_Flash_Page;
	Status|=Solder_Iron_Flash_Read_Struct(Soldering_Iron,addr);
	addr+=sizeof(*Soldering_Iron)+2;
	Status|=Soldering_Heat_Gun_Flash_Read_Struct(Soldering_Heat_Gun,addr);
	addr+=sizeof(*Soldering_Heat_Gun)+2;
	Status|=Soldering_Separator_Flash_Read_Struct(Soldering_Separator,addr);

	return Status;
}
//---------------------------------------------------------------------------------
uint8_t Soldering_Station_Write_Struct(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){

	uint8_t Status=HAL_OK;

	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Soldering_Station_ALL_Struct_Flash_Page;

	Flash_Erase (addr,sizeof(*Soldering_Iron)+2+sizeof(*Soldering_Heat_Gun)+2+sizeof(*Soldering_Separator));

	if(Solder_Iron_Flash_Write_Struct(Soldering_Iron,addr)==HAL_ERROR)
		Status|=HAL_ERROR;
	addr+=sizeof(*Soldering_Iron)+2;
	if(Soldering_Heat_Gun_Flash_Write_Struct(Soldering_Heat_Gun,addr)==HAL_ERROR)
		Status|=HAL_ERROR;
	addr+=sizeof(*Soldering_Heat_Gun)+2;
	if(Soldering_Separator_Flash_Write_Struct(Soldering_Separator,addr)==HAL_ERROR)
		Status|=HAL_ERROR;

	return Status;
}
//---------------------------------------------------------------------------------
uint32_t Write_Manual_Temperature(struct Temperature_Pressets *self, uint32_t Flash_Key, uint8_t Flash_Page){

	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Flash_Page;

	Flash_Erase (addr,sizeof(self->Manual)+sizeof(Flash_Key));

	Flash_Write (addr,&self->Manual, sizeof(self->Manual));

	addr+=sizeof(self->Manual)+2;

	return Flash_Write (addr,&Flash_Key, sizeof(Flash_Key));

}
//---------------------------------------------------------------------------------
uint8_t Flash_Read_Manual_Temperature(struct Temperature_Pressets *self, uint32_t Flash_Key, uint8_t Flash_Page){
	uint32_t addr = Flash_Base_Addr + Flash_Addr_Page_Size*Flash_Page;
	uint32_t Flash_Read_Key=0;
	uint16_t Temperature;

	Flash_Read (addr, &Temperature, sizeof(self->Manual));
	addr+=sizeof(self->Manual)+2;
	Flash_Read (addr, &Flash_Read_Key,  sizeof(Flash_Read_Key));

	if(Flash_Read_Key==Flash_Key){
		self->Manual=Temperature;
		return HAL_OK;
	}
	else
		return HAL_ERROR;
}
//---------------------------------------------------------------------------------
void Flash_Reset_Rewrite_Timer(struct Flash_Rewrite_Timer *self){
	self->Flash_Timer_Count=0;
	self->Flash_Timer_State=Flash_Timer_State_ON;
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_it(struct Flash_Rewrite_Timer *Flash_Rewrite_Timer, struct Temperature_Pressets *Temperature_Pressets,uint32_t Flash_Key, uint8_t Flash_Page){
	if(Flash_Rewrite_Timer->Flash_Timer_State==Flash_Timer_State_ON){
		if(Flash_Rewrite_Timer->Flash_Timer_Count++==Flash_Rewrite_Timer_Max_Count){
			Write_Manual_Temperature(Temperature_Pressets, Flash_Key, Flash_Page);
			Flash_Rewrite_Timer->Flash_Timer_State=Flash_Timer_State_OFF;
		}
	}
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_tim_it(struct Soldering_Iron *Soldering_Iron, struct Soldering_Heat_Gun *Soldering_Heat_Gun, struct Soldering_Separator *Soldering_Separator){
	Flash_Rewrite_Timer_it(&Soldering_Iron->Flash_Rewrite_Timer, &Soldering_Iron->Temperature_Pressets, Soldering_Iron_Manual_Temperature_Flash_Key, Soldering_Iron_Manual_Temperature_Flash_Page);
	Flash_Rewrite_Timer_it(&Soldering_Heat_Gun->Flash_Rewrite_Timer, &Soldering_Heat_Gun->Temperature_Pressets, Soldering_Heat_Gun_Manual_Temperature_Flash_Key, Soldering_Heat_Gun_Manual_Temperature_Flash_Page);
	Flash_Rewrite_Timer_it(&Soldering_Separator->Flash_Rewrite_Timer, &Soldering_Separator->Temperature_Pressets, Soldering_Separator_Manual_Temperature_Flash_Key, Soldering_Separator_Manual_Temperature_Flash_Page);
}
//---------------------------------------------------------------------------------
void Flash_Rewrite_Timer_ini(struct Flash_Rewrite_Timer *self){
	self->Flash_Timer_Count=0;
	self->Flash_Timer_State=Flash_Timer_State_OFF;
}
//---------------------------------------------------------------------------------







