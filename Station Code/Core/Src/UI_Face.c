#include "UI_Face.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Face_UI_Draw_Name_List(){
	UI_Clear_Line(0);
	UI_Draw_String(" SoldIron ", 0, 0, Inverse_Display_MODE);
	UI_Clear_Line(1);
	UI_Draw_String("State:", 0, 1, Direct_Display_MODE);
	UI_Clear_Line(2);
	UI_Draw_String("TSetC:", 0, 2, Direct_Display_MODE);
	UI_Clear_Line(3);
	UI_Draw_String("TNowC:", 0, 3, Direct_Display_MODE);
	UI_Clear_Line(4);
	UI_Draw_String("AdPow:", 0, 4, Direct_Display_MODE);
	UI_Clear_Line(5);


	UI_Draw_String("Freq:", 0, 5, Inverse_Display_MODE);
	UI_Clear_Line(6);
	UI_Draw_String("Count:", 0, 6, Direct_Display_MODE);


	UI_Draw_String(" SoldFAN ", 11, 0, Inverse_Display_MODE);
	UI_Draw_String("State:", 11, 1, Direct_Display_MODE);
	UI_Draw_String("TSetC:", 11, 2, Direct_Display_MODE);
	UI_Draw_String("TNowC:", 11, 3, Direct_Display_MODE);


	UI_Draw_String(" Separator", 11, 4, Direct_Display_MODE);
	UI_Draw_String("State:", 11, 5, Direct_Display_MODE);
	UI_Draw_String("TSetC:", 11, 6, Direct_Display_MODE);
	UI_Draw_String("TNowC:", 11, 7, Direct_Display_MODE);

}
//---------------------------------------------------------------------------------
void Face_UI_Draw_Parameters_List(){
							//Draw Solder_Iron Parameters
	UI_Draw_Parmetr(Solder_Iron_Get_State(&Soldering_Iron), 6, 1, BOOL);
	UI_Draw_Parmetr(Soldering_Iron_Get_Temperature_Set_C(&Soldering_Iron), 6, 2, UINT16);
	if(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_ON)
		UI_Draw_Parmetr(Soldering_Iron_Get_Current_Temperature_C(&Soldering_Iron), 6, 3, UINT16);
	else
		UI_Draw_String("----",6,3,Direct_Display_MODE);
	UI_Draw_Parmetr(Solder_Iron_Get_Full_Power_State(&Soldering_Iron), 6, 4, BOOL);

							//Draw Phase Angle Control Parameters
	if(PAC_Get_State(&PAC)==ZCD_STATE_ON){
		UI_Draw_Parmetr(PAC_Get_Frequency(&PAC), 6, 5, UINT8);
		UI_Draw_Parmetr(PAC_Get_Max_Control_Value(&PAC), 6, 6, UINT16);
		if(Soldering_Heat_Gun_Get_PAC_Controll_State(&Soldering_Heat_Gun)==PAC_Control_ON)
			UI_Draw_Parmetr(Soldering_Heat_Gun_Get_PAC_Controll_Value(&Soldering_Heat_Gun), 1, 7, UINT16);
		else
			UI_Draw_String("----",1,7,Direct_Display_MODE);
		if(Soldering_Separator_Get_PAC_Controll_State(&Soldering_Separator)==PAC_Control_ON)
			UI_Draw_Parmetr(Soldering_Separator_Get_PAC_Controll_Value(&Soldering_Separator), 6, 7, UINT16);
		else
			UI_Draw_String("----",6,7,Direct_Display_MODE);
	}
	else{
		UI_Draw_String("----",6,5,Direct_Display_MODE);
		UI_Draw_String("----",6,6,Direct_Display_MODE);
		UI_Draw_String("----",1,7,Direct_Display_MODE);
		UI_Draw_String("----",6,7,Direct_Display_MODE);
	}

							//Draw Soldering_Heat_Gun Parameters
	UI_Draw_Parmetr(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun), 17, 1, BOOL);
	UI_Draw_Parmetr(Soldering_Heat_Gun_Get_Temperature_Set_C(&Soldering_Heat_Gun), 17, 2, UINT16);
	if(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun)!=Heat_Gun_OFF)
		UI_Draw_Parmetr(Soldering_Heat_Gun_Get_Current_Temperature_C(&Soldering_Heat_Gun), 17, 3, UINT16);
	else
		UI_Draw_String("----",17,3,Direct_Display_MODE);

							//Draw Soldering_Separator Parameters
	UI_Draw_Parmetr(Soldering_Separator_Get_State(&Soldering_Separator), 17, 5, BOOL);
	UI_Draw_Parmetr(Soldering_Separator_Get_Temperature_Set_C(&Soldering_Separator), 17, 6, UINT16);
	if(Soldering_Separator_Get_State(&Soldering_Separator)==Separator_ON)
		UI_Draw_Parmetr(Soldering_Separator_Get_Current_Temperature_C(&Soldering_Separator), 17, 7, UINT16);
	else
		UI_Draw_String("----",17,7,Direct_Display_MODE);

}
//---------------------------------------------------------------------------------Button_Handler
void Get_Button_State(struct Button_Vector *Button_Vector, struct Button* Full_Power_Button){
	/**
	 * //Work with buttons buffers and set modes for all part of soldering station witch ocure
	 */

	struct Button_Vector *now=Button_Vector;
	do{
		if(Button_Get_Event_State(now->Button)){	//Сheck the current button for a state change
			switch(now->Button->ID){				//Identify the button by ID
				case Solder_Iron_Set_Preset2_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_ON)){	//check for a falling edge event
						Solder_Iron_Set_Preset2_Temperature(&Soldering_Iron);	//Set temperature Preset
					}
					break;
				case Soldering_Heat_Gun_Set_Preset2_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun)!=Heat_Gun_OFF)){
						Soldering_Heat_Gun_Set_Preset2_Temperature(&Soldering_Heat_Gun);
					}
					break;
				case Soldering_Separator_Set_Preset2_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Soldering_Separator_Get_State(&Soldering_Separator)==Separator_ON))
						Soldering_Separator_Set_Preset2_Temperature(&Soldering_Separator);
					break;
				case Solder_Iron_Set_Preset1_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_ON)){
						Solder_Iron_Set_Preset1_Temperature(&Soldering_Iron);
					}
					break;
				case Soldering_Heat_Gun_Set_Preset1_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun)!=Heat_Gun_OFF)){
						Soldering_Heat_Gun_Set_Preset1_Temperature(&Soldering_Heat_Gun);
					}
					break;
				case Soldering_Separator_Set_Preset1_Button_ID:
					if((Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)&&(Soldering_Separator_Get_State(&Soldering_Separator)==Separator_ON)){
						Soldering_Separator_Set_Preset1_Temperature(&Soldering_Separator);
					}
					break;
				case Button_Gerkon_ID:										// if Gerkon
					if(Button_Get_Pin_State(now->Button)==GPIO_PIN_RESET)
						Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
					else
						Soldering_Heat_Gun_ON(&Soldering_Heat_Gun);

					break;

			}
		}
		now=now->next;			//Go to the next button
	}while(now!=NULL);



	if(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_ON){			//Check Full_Power_Button for a state change if Soldering_Iron is Turn on
		if(Button_Get_Event_State(Full_Power_Button)){
			if(Button_Get_Pin_State(Full_Power_Button)==GPIO_PIN_RESET)
				Solder_Iron_Full_Power_ON(&Soldering_Iron);
			else
				Solder_Iron_Full_Power_OFF(&Soldering_Iron);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void Face_UI_it(struct Encoder *Encoder_P1, struct Encoder *Encoder_P2, struct Button_Vector *Button_Vector){
	/**
	 * This interface need to control all main elements of Soldering Station and to display all current parameters witch user need to work with/
	 * Elements of station such ass Solder_Irin can work only when MC transfer control to this function
	 */
	enum{
		Interface_Contoll_Soldering_Heat_Gun,
		Interface_Contoll_Soldering_Separator
	}Interface_Contoll=Interface_Contoll_Soldering_Heat_Gun;			//this parameter contain witch interface is active now


	Soldering_Station_Temperature_Presets_Corection_Converting(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);	//Transfer Thermal correction Celsius to ADC's Levels
																																// It needs because Thermal correction with PID work with ADC's Levels
	Face_UI_Draw_Name_List();													// Clear display and draw Name of parameters on the display

	UI_Encoder_Reset_Button_State(Encoder_P2);
	UI_Encoder_Reset_Button_State(Encoder_P1);									//Clear encoders

	do{																			//Start work cycle

		if(Encoder_Get_Button_State(Encoder_P1)==Encoder_Button_long){			//Select which element will be control by Encoder
			UI_Encoder_Reset_Button_State(Encoder_P1);							//Clear Encoder Buffer
			if(Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun){		// Soldering Separator
				Interface_Contoll=Interface_Contoll_Soldering_Separator;
				UI_Draw_String(" SoldFAN ", 11, 0, Direct_Display_MODE);		//Redraw part of name interface ton mark witch part are selected
				UI_Draw_String(" Separator", 11, 4, Inverse_Display_MODE);
			}
			else{																//Soldering heat gun
				Interface_Contoll=Interface_Contoll_Soldering_Heat_Gun;
				UI_Draw_String(" SoldFAN ", 11, 0, Inverse_Display_MODE);		//Redraw part of name interface ton mark witch part are select
				UI_Draw_String(" Separator", 11, 4, Direct_Display_MODE);
			}
		}

		Get_Button_State(Button_Vector, &Soldering_Iron.Full_Power_Button);		//Work with buttons buffers

		if(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_ON){			//Change Solder_Iron Manual Temperature if Solder_Iron is turn ON
			Solder_Iron_Set_Manual_Preset_Temperature_C(
					(uint16_t)UI_Change_Value(Soldering_Iron_Get_Manual_Preset_Temperature_C(&Soldering_Iron), 5, UINT16, Soldering_Iron_MIN_Manual_Preset_Temperature_C, MAX_ADC_Value, Encoder_P2),
					&Soldering_Iron);
			Solder_Iron_Set_Manual_Temperature(&Soldering_Iron);
		}
		else
			UI_Encoder_Reset_Rotary_Switch_Buffer(Encoder_P2);					//Else clear Encoder Buffer


		if(Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun){				//Soldering_Heat_Gun manual temperature changing
			if(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun)!=Heat_Gun_OFF){	//Change Soldering_Heat_Gun Temperature if Soldering_Heat_Gun is turn ON
				Soldering_Heat_Gun_Set_Manual_Preset_Temperature_C(
						(uint16_t)UI_Change_Value(Soldering_Heat_Gun_Get_Manual_Preset_Temperature_C(&Soldering_Heat_Gun), 5, UINT16, Soldering_Heat_Gun_MIN_Manual_Preset_Temperature_C, MAX_ADC_Value, Encoder_P1),
						&Soldering_Heat_Gun);
				Soldering_Heat_Gun_Set_Manual_Temperature(&Soldering_Heat_Gun);
			}
			else
				UI_Encoder_Reset_Rotary_Switch_Buffer(Encoder_P1);				//Else clear Encoder Buffer
		}
		else{																		//Soldering_Separator manual temperature changing
			if(Soldering_Separator_Get_State(&Soldering_Separator)==Separator_ON){	//Change Soldering_Separator Temperature if Soldering_Separator is turn ON
				Soldering_Separator_Set_Manual_Preset_Temperature_C(
						(uint16_t)UI_Change_Value(Soldering_Separator_Get_Manual_Preset_Temperature_C(&Soldering_Separator), 5, UINT16, Soldering_Separator_MIN_Manual_Preset_Temperature_C, MAX_ADC_Value, Encoder_P1),
						&Soldering_Separator);
				Soldering_Separator_Set_Manual_Temperature(&Soldering_Separator);
			}
			else
				UI_Encoder_Reset_Rotary_Switch_Buffer(Encoder_P1);					//Else clear Encoder Buffer
		}


		if(Encoder_Get_Button_State(Encoder_P2)==Encoder_Button_Short){                 //Solder_Iron_ON/OFF
			UI_Encoder_Reset_Button_State(Encoder_P2);									//Clear Encoder Buffer
			if(Solder_Iron_Get_State(&Soldering_Iron)==Soldering_Iron_OFF){				//Switch Solder_Iron state
				Solder_Iron_ON(&Soldering_Iron);
			}else{
				Solder_Iron_OFF(&Soldering_Iron);
			}
		}

		if(Encoder_Get_Button_State(Encoder_P1)==Encoder_Button_Short&&Interface_Contoll==Interface_Contoll_Soldering_Heat_Gun){			//Soldering_Heat_Gun_ON/OFF
			UI_Encoder_Reset_Button_State(Encoder_P1);								//Clear Encoder Buffer
			if(Soldering_Heat_Gun_Get_State(&Soldering_Heat_Gun)!=Heat_Gun_ON)		//Switch Soldering_Heat_Gun state
				Soldering_Heat_Gun_ON(&Soldering_Heat_Gun);
			else
				Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
		}
		else if(Encoder_Get_Button_State(Encoder_P1)==Encoder_Button_Short&&Interface_Contoll==Interface_Contoll_Soldering_Separator){		//Soldering_Separator_ON/OFF
			UI_Encoder_Reset_Button_State(Encoder_P1);								//Clear Encoder Buffer
				if(Soldering_Separator_Get_State(&Soldering_Separator)!=Separator_ON)
					Soldering_Separator_ON(&Soldering_Separator);
				else
					Soldering_Separator_OFF(&Soldering_Separator);
		}
		Face_UI_Draw_Parameters_List();

	}while(Encoder_Get_Button_State(Encoder_P2)!=Encoder_Button_long); //Exit from Face_Ui

	Solder_Iron_OFF(&Soldering_Iron);				//Turn OFF ALL Devices
	Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);
	Soldering_Separator_OFF(&Soldering_Separator);

	UI_Encoder_Reset_Button_State(Encoder_P2);		//Reset Encoders buffer state
	UI_Encoder_Reset_Button_State(Encoder_P1);
}
