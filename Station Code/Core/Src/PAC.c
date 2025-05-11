/*
 * ZCD.c
 *
 *  Created on: Oct 1, 2024
 *      Author: I
 */
#include "PAC.h"
//---------------------------------------------------------------------------------
void PAC_Reset(struct PAC *self);
//---------------------------------------------------------------------------------
struct PAC *PAC_Base_Create(struct PAC_Init *self){

	struct PAC *PAC = malloc(sizeof(struct PAC));
	PAC->ZCD.tim = malloc(sizeof(TIM_HandleTypeDef));

	PAC->ZCD.DMA = self->DMA;
	PAC->ZCD.DMA_Chanel = self->DMA_Chanel;
	PAC->ZCD.TIM_Instance = self->TIM_Instance;
	PAC->ZCD.IC_Trigger_Channel_GPIO = self->IC_Trigger_Channel_GPIO;
	PAC->ZCD.IC_Trigger_Channel_PIN = self->IC_Trigger_Channel_PIN;
	PAC->ZCD.TIM_CHANNEL = self->TIM_CHANNEL;
	PAC->ZCD.State = ZCD_STATE_OFF;

	PAC_Reset(PAC);

	PAC->PAC_Devices_Control_Vector_Head = NULL;

	return PAC;
}
//---------------------------------------------------------------------------------
struct PAC_Devices_Control_Vector *PAC_Devise_Control_Create(struct PAC *self, struct PAC_Devices_Control_Init *element){
	/**
	 * Create new PAC_Control_Unit to PAC_Devices_Control_Vector into PAC
	 * return pointer to new PAC_Control_Unit
	 */
	struct PAC_Devices_Control_Vector *NEW_Element = malloc(sizeof(struct PAC_Devices_Control_Vector));	//allocate memory for a new element
	NEW_Element->Channel_PIN = element->Channel_PIN;									//copy data to a new element
	NEW_Element->Channel_GPIO = element->Channel_GPIO;
	NEW_Element->TIM_CHANNEL = element->TIM_CHANNEL;
	NEW_Element->tim = self->ZCD.tim;
	NEW_Element->State = PAC_Device_OFF;
	NEW_Element->next = NULL;
	NEW_Element->Parent = self;
	if(self->PAC_Devices_Control_Vector_Head == NULL){									//Check. is this a first element
		self->PAC_Devices_Control_Vector_Head = NEW_Element;							// New element is head
	}else{																				// else skip to last element
		struct PAC_Devices_Control_Vector *now = self->PAC_Devices_Control_Vector_Head;
		while(now->next!=NULL){
			now=now->next;
		}
		now->next=NEW_Element;															//next last element is new element
	}
	return NEW_Element;																	//return pointer for a new element
}
//----------------------------------------------------------------------------------------------
void PAC_IC_Init(struct PAC *self){
	/**
	 * Initializing TIM, DMA and PIN into input capture mode
	 */

	PAC_Reset(self);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = self->ZCD.IC_Trigger_Channel_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(self->ZCD.IC_Trigger_Channel_GPIO, &GPIO_InitStruct);

	struct PAC_Devices_Control_Vector *now = self->PAC_Devices_Control_Vector_Head;

	while(now != NULL){
		GPIO_InitStruct.Pin = now->Channel_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(now->Channel_GPIO, &GPIO_InitStruct);
		HAL_GPIO_WritePin(now->Channel_GPIO, now->Channel_PIN, GPIO_PIN_RESET);
		now=now->next;
	}


	self->ZCD.DMA->Instance = self->ZCD.DMA_Chanel;
	self->ZCD.DMA->Init.Direction = DMA_PERIPH_TO_MEMORY;
	self->ZCD.DMA->Init.PeriphInc = DMA_PINC_DISABLE;
	self->ZCD.DMA->Init.MemInc = DMA_MINC_ENABLE;
	self->ZCD.DMA->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	self->ZCD.DMA->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	self->ZCD.DMA->Init.Mode = DMA_NORMAL;
	self->ZCD.DMA->Init.Priority = DMA_PRIORITY_LOW;
	HAL_DMA_Init(self->ZCD.DMA);


	self->ZCD.tim->hdma[TIM_DMA_ID_CC1] = self->ZCD.DMA;			//__HAL_LINKDMA
	self->ZCD.DMA->Parent = self->ZCD.tim;

	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);



	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};

	__HAL_RCC_TIM3_CLK_ENABLE();

	self->ZCD.tim->Instance = self->ZCD.TIM_Instance;
	self->ZCD.tim->Init.Prescaler = 799;
	self->ZCD.tim->Init.CounterMode = TIM_COUNTERMODE_UP;
	self->ZCD.tim->Init.Period = 65535;
	self->ZCD.tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	self->ZCD.tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(self->ZCD.tim);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(self->ZCD.tim, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(self->ZCD.tim, &sMasterConfig);

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(self->ZCD.tim, &sConfigIC, TIM_CHANNEL_1);

	HAL_TIM_IC_Init(self->ZCD.tim);



}
//----------------------------------------------------------------------------------------------
void PAC_IC_DeInit(struct PAC *self){

	__HAL_RCC_TIM3_CLK_DISABLE();

	//HAL_GPIO_DeInit(self->ZCD.IC_Trigger_Channel_GPIO, self->ZCD.IC_Trigger_Channel_PIN);

	HAL_DMA_DeInit(self->ZCD.tim->hdma[TIM_DMA_ID_CC1]);
	HAL_TIM_IC_DeInit(self->ZCD.tim);

	HAL_TIM_Base_DeInit(self->ZCD.tim);

	//HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);

}
//---------------------------------------------------------------------------------
void PAC_PWM_Init(struct PAC *self){
	/**
	 * Initializing TIM and PINs into PWM mode
	 */

	GPIO_InitTypeDef GPIO_InitStruct = {0};


    __HAL_RCC_TIM3_CLK_ENABLE();

	GPIO_InitStruct.Pin = self->ZCD.IC_Trigger_Channel_PIN;				//Initializing TIMs TI1FP1 PIN
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(self->ZCD.IC_Trigger_Channel_GPIO, &GPIO_InitStruct);


	struct PAC_Devices_Control_Vector *now = self->PAC_Devices_Control_Vector_Head;		//Initializing TIM's CHANNELs PINs
	while(now != NULL){
		GPIO_InitStruct.Pin = now->Channel_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(now->Channel_GPIO, &GPIO_InitStruct);
		now=now->next;
	}

	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};


	self->ZCD.tim->Instance = self->ZCD.TIM_Instance;					// Initializing TIM into TIM_SLAVEMODE_RESET for PWM synchronization
	self->ZCD.tim->Init.Prescaler = 799;
	self->ZCD.tim->Init.CounterMode = TIM_COUNTERMODE_UP;
	self->ZCD.tim->Init.Period = self->ZCD.TIM_MAX_Count;
	self->ZCD.tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	self->ZCD.tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_Base_Init(self->ZCD.tim);

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
	sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING;
	sSlaveConfig.TriggerFilter = 0;
	HAL_TIM_SlaveConfigSynchro(self->ZCD.tim, &sSlaveConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(self->ZCD.tim, &sMasterConfig);

	HAL_TIM_PWM_Init(self->ZCD.tim);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	now = self->PAC_Devices_Control_Vector_Head;
	while(now != NULL){
		HAL_TIM_PWM_ConfigChannel(now->tim, &sConfigOC, now->TIM_CHANNEL);
		now=now->next;
	}

}
//---------------------------------------------------------------------------------
void PAC_PWM_DeInit(struct PAC *self)
{

    __HAL_RCC_TIM3_CLK_DISABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    struct PAC_Devices_Control_Vector *now = self->PAC_Devices_Control_Vector_Head;

	while(now != NULL){
		GPIO_InitStruct.Pin = now->Channel_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(now->Channel_GPIO, &GPIO_InitStruct);
		HAL_GPIO_WritePin(now->Channel_GPIO, now->Channel_PIN, GPIO_PIN_RESET);
		now=now->next;
	}

	HAL_TIM_Base_DeInit(self->ZCD.tim);

	HAL_TIM_PWM_DeInit(self->ZCD.tim);

}
//---------------------------------------------------------------------------------
uint16_t PAC_Device_Control_Get_Control_Value(struct PAC_Devices_Control_Vector *self){
	/**
	 * Return Current PAC_Device_Control_Unit's Control Value
	 */
	switch(self->TIM_CHANNEL){
		case TIM_CHANNEL_1: return self->Parent->ZCD.TIM_MAX_Count - self->tim->Instance->CCR1; break;
		case TIM_CHANNEL_2: return self->Parent->ZCD.TIM_MAX_Count - self->tim->Instance->CCR2; break;
		case TIM_CHANNEL_3: return self->Parent->ZCD.TIM_MAX_Count - self->tim->Instance->CCR3; break;
		case TIM_CHANNEL_4: return self->Parent->ZCD.TIM_MAX_Count - self->tim->Instance->CCR4; break;
	};
	return 0;
}
//---------------------------------------------------------------------------------
void PAC_Device_Control_Set_Control_Value(struct PAC_Devices_Control_Vector *self, uint16_t value){
	/**
	 * Set control value for current PAC_Control_Unit
	 */
	switch(self->TIM_CHANNEL){
		case TIM_CHANNEL_1: self->tim->Instance->CCR1 = self->Parent->ZCD.TIM_MAX_Count - value; break;
		case TIM_CHANNEL_2: self->tim->Instance->CCR2 = self->Parent->ZCD.TIM_MAX_Count - value; break;
		case TIM_CHANNEL_3: self->tim->Instance->CCR3 = self->Parent->ZCD.TIM_MAX_Count - value; break;
		case TIM_CHANNEL_4: self->tim->Instance->CCR4 = self->Parent->ZCD.TIM_MAX_Count - value; break;
	};
}
//---------------------------------------------------------------------------------
void PAC_IC_Capture_INT(struct PAC *self){
	/**
	 * Input capture internal interrupt.
	 * Mast be called after IC DMA. When CNT data has been received.
	 * Transfer PAC from Setup to ON state.
	 */

	self->ZCD.TIM_MAX_Count = (self->ZCD.CNT_DATA[2] - self->ZCD.CNT_DATA[1]);

	if(self->ZCD.TIM_MAX_Count != 0){

		//self->ZCD.Frequency = self->ZCD.TIM_MAX_Count / 2;
		if(self->ZCD.tim->Instance == TIM3){
			self->ZCD.Frequency = HAL_RCC_GetPCLK1Freq() / (self->ZCD.tim->Instance->PSC+1);
		}
		else if(self->ZCD.tim->Instance == TIM1)
			self->ZCD.Frequency = HAL_RCC_GetPCLK2Freq() / (self->ZCD.tim->Instance->PSC+1);

		self->ZCD.Frequency = self->ZCD.Frequency / self->ZCD.TIM_MAX_Count;

		self->ZCD.TIM_MAX_Count*=KOEF;

		PAC_IC_DeInit(self);
		PAC_PWM_Init(self);

		struct PAC_Devices_Control_Vector *now = self->PAC_Devices_Control_Vector_Head;

		while(now != NULL){
			if(now->State == PAC_Device_ON){
				PAC_Device_Control_Set_Control_Value(now, 0);
				HAL_TIM_PWM_Start(now->tim, now->TIM_CHANNEL);
			}
			now = now->next;
		}

		self->ZCD.State = ZCD_STATE_ON;
		PAC_ON_Callback(self);
	}
	else
		HAL_TIM_IC_Start_DMA(self->ZCD.tim, self->ZCD.TIM_CHANNEL, (uint32_t *)self->ZCD.CNT_DATA, 3);
}
//---------------------------------------------------------------------------------
void ZCD_ON(struct PAC *self){
	/**
	 * Turn ON ZCD
	 */
	if(PAC_Get_State(self) == ZCD_STATE_OFF){
		self->ZCD.State = ZCD_STATE_SETUP;
		PAC_IC_Init(self);
		HAL_TIM_IC_Start_DMA(self->ZCD.tim, self->ZCD.TIM_CHANNEL, (uint32_t *)self->ZCD.CNT_DATA, 3);
	}
}
//---------------------------------------------------------------------------------
void ZCD_OFF(struct PAC *self){
	/**
	 * ZCD Turns OFF
	 */
	if(PAC_Get_State(self) != ZCD_STATE_OFF){	//if ZCD already is Turns OFF do nothing
		PAC_PWM_DeInit(self);
		PAC_Reset(self);
		self->ZCD.State = ZCD_STATE_OFF;
	}
}
//---------------------------------------------------------------------------------
void PAC_Device_Control_ON(struct PAC_Devices_Control_Vector *self){
	/**
	 * Turn ON PAC_Device_Control_Unit if ZCD_STATE_ON else Turn ON ZCD
	 */
	self->State = PAC_Device_ON;
	if(PAC_Get_State(self->Parent) == ZCD_STATE_ON){
		PAC_Device_Control_Set_Control_Value(self, 0);
		HAL_TIM_PWM_Start(self->tim, self->TIM_CHANNEL);
	}
	else if(PAC_Get_State(self->Parent) == ZCD_STATE_OFF)
		ZCD_ON(self->Parent);
}
//---------------------------------------------------------------------------------
void PAC_Device_Control_OFF(struct PAC_Devices_Control_Vector *self){
	/**
	 * Turn OFF PAC_Device_Control_Unit if it is the last one PAC_Device_Control_Unit than Turn OFF ZCD
	 */
	if(self->State == PAC_Device_ON)
		HAL_TIM_PWM_Stop(self->tim, self->TIM_CHANNEL);

	self->State = PAC_Device_OFF;

	struct PAC_Devices_Control_Vector *now = self->Parent->PAC_Devices_Control_Vector_Head;

	enum PAC_Device_Control_State State = PAC_Device_OFF;

 	while(now != NULL){									// Check ALL PAC_Device_Control_Units states
 		State |= now->State;
 		now = now->next;
 	}

 	if(State == PAC_Device_OFF){						// if ALL PAC_Device_Control_Units are turn OFF then turn off ZCD;
 		ZCD_OFF(self->Parent);
 	}
}
//---------------------------------------------------------------------------------
enum PAC_Device_Control_State PAC_Device_Control_Get_State(struct PAC_Devices_Control_Vector *self){
	/**
	 * Return PAC_Device_Control_Unit State if ZCD turn ON
	 */
	if(self->State == PAC_Device_ON && PAC_Get_State(self->Parent) == ZCD_STATE_ON)
		return PAC_Device_ON;
	else
		return PAC_Device_OFF;
}
//---------------------------------------------------------------------------------
enum ZCD_State PAC_Get_State(struct PAC *self){
	/**
	 * Return ZCD STATE
	 */
	return self->ZCD.State;
}
//---------------------------------------------------------------------------------
uint8_t PAC_Get_Frequency(struct PAC *self){
	/**
	 * Return Frequency
	 */
	return self->ZCD.Frequency; //Frequency
}
//---------------------------------------------------------------------------------
uint16_t PAC_Get_Max_Control_Value(struct PAC *self){
	/**
	 * MAX control value for PAC_Control_Unit
	*/
	return self->ZCD.TIM_MAX_Count;
}
//---------------------------------------------------------------------------------
void PAC_Reset(struct PAC *self){
	/**
	 * Reset all variables
	 */
	self->ZCD.CNT_DATA[0] = 0;
	self->ZCD.CNT_DATA[1] = 0;
	self->ZCD.CNT_DATA[3] = 0;
	self->ZCD.TIM_MAX_Count=0;
	self->ZCD.Frequency = 0;
}






