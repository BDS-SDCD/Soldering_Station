/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim3_ch1_trig;

/* USER CODE BEGIN PV */
//---------------------------------------------------------------------------------
struct OLED OLED1;
struct Encoder Encoder_P2, Encoder_P1;
struct Button_Vector Button_Vector;
//struct Button  Full_Power_Button;
struct Vibration_Sensor VS;
struct Menu_List_Element_Vector Soldering_Iron_Menu_Vector, Soldering_Heat_Gun_Menu_Vector, Soldering_Separator_Menu_Vector;
struct Menu_List_Vector Menu_List_Vector;
struct Soldering_Iron Soldering_Iron;
struct Soldering_Heat_Gun Soldering_Heat_Gun;
struct Soldering_Separator Soldering_Separator;
struct PAC *PAC;


//---------------------------------------------------------------------------------
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

uint16_t ADC_Data[17];

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------------------------------------------------------------------------
void PAC_ON_Callback(struct PAC *self){

	/**
	 * Set max MAX_Control_Value for all PID which controlling PAG
	 */
	Soldering_Heat_Gun_Set_PID_MAX_Control_Value(PAC_Get_Max_Control_Value(self), &Soldering_Heat_Gun);
	Soldering_Separator_Set_PID_MAX_Control_Value(PAC_Get_Max_Control_Value(self), &Soldering_Separator);
}
//---------------------------------------------------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	PAC_IC_Capture_INT(PAC);
}
//--------------------------------------------------------------------------------- ADC_Callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	/**
	 * After DAC_Read Analog signal into ADC_Data array
	 */
	Solder_Iron_it(&Soldering_Iron);				//PID_IT and PWM Start
	Soldering_Heat_Gun_it(&Soldering_Heat_Gun);		//PID_IT
	Soldering_Separator_it(&Soldering_Separator);	//PID_IT
}
//--------------------------------------------------------------------------------- EXTI
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
		Button_Vector_EXTI(&Button_Vector,&GPIO_PIN);
		Encoder_EXTI(&Encoder_P2, &GPIO_PIN);
		Encoder_EXTI(&Encoder_P1, &GPIO_PIN);
}
//---------------------------------------------------------------------------------TIM_Callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	static uint8_t prescaler=1;


        if(htim->Instance == TIM1) //check if the interrupt comes from TIM1									//ADC DMA
        {
        	if(prescaler==1){													//First we needed stop PWM to read cotect temperature from ADC
        		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);						//
        		prescaler=0;
        	}
        	else{																//in next tim interrupt
        		HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Data[0],(uint32_t)16);	//Read data from ADC by DMA
        		prescaler=1;
        	}
        }else if(htim->Instance == TIM3){																	//Service  TIM3

        }else if(htim->Instance == TIM4){						//Timer only for PAC
        	//PAC_it(&PAC);
        	Flash_Rewrite_Timer_tim_it(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);
        	Solder_Iron_tim_it(&Soldering_Iron);
        	Button_Vector_it(&Button_Vector);
        	Encoder_it(&Encoder_P1);
        	Encoder_it(&Encoder_P2);
        }
}
//--------------------------------------------------------------------------------- INI Blocks
void BUTTON_INI(void){
	/**
	 * Button initialization
	 */
	struct Button Button_ini;

	Button_ini.ID=Button_ID_SW8;		//ID
	Button_ini.count_max=2;				//Contact bounce in tim interrupts
	Button_ini.EXTI_PIN=GPIO_PIN_14;	//in my schematic different buttons have the same EXTI pin
	Button_ini.PIN=GPIO_PIN_8;			//Actual state pin
	Button_ini.GPIO=GPIOA;				//Actual pin state GPIO
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;//MODE
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_ID_SW6;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_14;
	Button_ini.PIN=GPIO_PIN_9;
	Button_ini.GPIO=GPIOA;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_ID_SW5;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_14;
	Button_ini.PIN=GPIO_PIN_10;
	Button_ini.GPIO=GPIOA;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_ID_SW2;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_3;
	Button_ini.PIN=GPIO_PIN_4;
	Button_ini.GPIO=GPIOB;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_ID_SW3;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_3;
	Button_ini.PIN=GPIO_PIN_5;
	Button_ini.GPIO=GPIOB;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_ID_SW7;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_3;
	Button_ini.PIN=GPIO_PIN_8;
	Button_ini.GPIO=GPIOB;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);

	Button_ini.ID=Button_Gerkon_ID;
	Button_ini.count_max=2;
	Button_ini.EXTI_PIN=GPIO_PIN_2;
	Button_ini.PIN=GPIO_PIN_2;
	Button_ini.GPIO=GPIOA;
	Button_ini.MODE=Button_Mode_Regular_With_EXTI;
	Button_Vector_Create(&Button_Vector,&Button_ini);


}
//---------------------------------------------------------------------------------
void Encoder_INI(void){
	Encoder_P2.ID=Encoder_ID_P2;
	Encoder_P2.Button.count_max=2;
	Encoder_P2.Button.EXTI_PIN=GPIO_PIN_14;
	Encoder_P2.Button.PIN=GPIO_PIN_15;
	Encoder_P2.Button.GPIO=GPIOB;
	Encoder_P2.Button.Presed_counter_max=3;
	Encoder_P2.Button.MODE=Button_Mode_Encoder;

	Encoder_P2.Rotary_Switch.State=0;
	Encoder_P2.Rotary_Switch.EXTI_PIN=GPIO_PIN_12;
	Encoder_P2.Rotary_Switch.PIN=GPIO_PIN_13;
	Encoder_P2.Rotary_Switch.GPIO=GPIOB;
	Encoder_P2.Rotary_Switch.Rotary_Switch_Direcion=Reverse;

	Encoder_ini(&Encoder_P2);


	Encoder_P1.ID=Encoder_ID_P1;
	Encoder_P1.Button.count_max=2;
	Encoder_P1.Button.EXTI_PIN=GPIO_PIN_3;
	Encoder_P1.Button.PIN=GPIO_PIN_12;
	Encoder_P1.Button.GPIO=GPIOA;
	Encoder_P1.Button.Presed_counter_max=3;
	Encoder_P1.Button.MODE=Button_Mode_Encoder;

	Encoder_P1.Rotary_Switch.State=0;
	Encoder_P1.Rotary_Switch.EXTI_PIN=GPIO_PIN_15;
	Encoder_P1.Rotary_Switch.PIN=GPIO_PIN_11;
	Encoder_P1.Rotary_Switch.GPIO=GPIOA;
	Encoder_P1.Rotary_Switch.Rotary_Switch_Direcion=Direct;

	Encoder_ini(&Encoder_P1);

}
//----------------------------------------------------------------------------
void MENU_INI(void){

	struct Menu_List_Element_Vector Menu_List_Element_Vector_ini;


	Menu_List_Element_Vector_ini.name="T_Corect_Manual";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Corection_C.Manual;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Presets_C.Preset1;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Corection_C.Preset1;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Presets_C.Preset2;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Corection_C.Preset2;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Sleep_mode_T";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Temperature_Presets_C.Sleep;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Sleep_timer";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Sleep_time;
	Menu_List_Element_Vector_ini.mode=UINT8;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Disable_timer";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.Disable_time;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="State";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Iron.State;
	Menu_List_Element_Vector_ini.mode=BOOL;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Iron_Menu_Vector,&Menu_List_Element_Vector_ini);


	UI_Menu_List_Create(&Menu_List_Vector,&Soldering_Iron_Menu_Vector, " Soldering_Iron ");


	Menu_List_Element_Vector_ini.name="T_Corect_Manual";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Corection_C.Manual;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Presets_C.Preset1;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Corection_C.Preset1;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Presets_C.Preset2;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Corection_C.Preset2;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Cooling_C";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Heat_Gun.Temperature_Coolling.C;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Heat_Gun_Menu_Vector,&Menu_List_Element_Vector_ini);



	UI_Menu_List_Create(&Menu_List_Vector,&Soldering_Heat_Gun_Menu_Vector, " Heat_Gun ");



	Menu_List_Element_Vector_ini.name="T_Corect_Manual";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Separator.Temperature_Corection_C.Manual;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Separator_Menu_Vector, &Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Separator.Temperature_Presets_C.Preset1;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Separator_Menu_Vector, &Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres1";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Separator.Temperature_Corection_C.Preset1;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Separator_Menu_Vector, &Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="Preset2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Separator.Temperature_Presets_C.Preset2;
	Menu_List_Element_Vector_ini.mode=UINT16;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Separator_Menu_Vector,&Menu_List_Element_Vector_ini);

	Menu_List_Element_Vector_ini.name="T_Corect_Pres2";
	Menu_List_Element_Vector_ini.parametr=&Soldering_Separator.Temperature_Corection_C.Preset2;
	Menu_List_Element_Vector_ini.mode=INT;
	Menu_List_Element_Vector_ini.step=1;
	UI_Menu_List_Element_Create(&Soldering_Separator_Menu_Vector, &Menu_List_Element_Vector_ini);


	UI_Menu_List_Create(&Menu_List_Vector,&Soldering_Separator_Menu_Vector, " Separator ");

}
//----------------------------------------------------------------------------
void OLED_INI(void){
	  OLED1.hi2c=&hi2c1;
	  OLED_Search_Adress(&OLED1);
	  OLED_ini(&OLED1);
}
//----------------------------------------------------------------------------
void Soldering_Iron_INI(uint8_t Flash_Read_Status){

	Soldering_Iron.PID.KP=Soldering_Iron_PID_KP;
	Soldering_Iron.PID.KI=Soldering_Iron_PID_KI;
	Soldering_Iron.PID.KD=Soldering_Iron_PID_KD;
	Soldering_Iron.PID.dt=0.1;
	Soldering_Iron.PID.MAX_Control=350;

	Soldering_Iron.Filter.Filter_Mode=Nine_Samples;
	Soldering_Iron.Filter.k_min=0.1;
	Soldering_Iron.Filter.k_max=0.8;
	Soldering_Iron.Filter.Val_Delata=30;
	Soldering_Iron.Filter.mass=&ADC_Data[4];

	Soldering_Iron.PWM_htim=&htim2;

	Soldering_Iron.Full_Power_Button.ID=Full_Power_Button_ID;
	Soldering_Iron.Full_Power_Button.count_max=2;
	Soldering_Iron.Full_Power_Button.EXTI_PIN=GPIO_PIN_4;
	Soldering_Iron.Full_Power_Button.PIN=GPIO_PIN_4;
	Soldering_Iron.Full_Power_Button.GPIO=GPIOA;
	Soldering_Iron.Full_Power_Button.MODE=Button_Mode_Regular_Without_EXTI;

	Soldering_Iron.VS.EXTI_PIN=GPIO_PIN_11;
	Soldering_Iron.VS.GPIO=GPIOB;
	Soldering_Iron.VS.State=3;
	Soldering_Iron.VS.Prew_State=4;

	Soldering_Iron.MODE=MANUAL;

	Soldering_Iron.Temperature_Converting.Coeff=Soldering_Iron_Temperature_Converting_Coeff;

	if(Flash_Read_Status!=HAL_OK){							//if Data was read incorrect from flash reinitialize all user parameters
		Soldering_Iron.Temperature_Presets_C.Manual=100;
		Soldering_Iron.Temperature_Presets_C.Preset1=150;
		Soldering_Iron.Temperature_Presets_C.Preset2=300;

		Soldering_Iron.Temperature_Presets_C.Sleep=100;
		Soldering_Iron.Sleep_time=200;
		Soldering_Iron.Disable_time=400;

		Soldering_Iron.Flash_Key=0x0801F802;
	}
	Solder_Iron_Flash_Read_Manual_Temperature(&Soldering_Iron);	//read Last manual temperature
	Solder_Iron_ini(&Soldering_Iron);
}
//----------------------------------------------------------------------------
void PAC_INI(){
	struct PAC_Init PAC_ini;

	PAC_ini.DMA = &hdma_tim3_ch1_trig;
	PAC_ini.DMA_Chanel = DMA1_Channel6;
	PAC_ini.TIM_Instance = TIM3;
	PAC_ini.IC_Trigger_Channel_GPIO = GPIOA;
	PAC_ini.IC_Trigger_Channel_PIN = GPIO_PIN_6;
	PAC_ini.TIM_CHANNEL=TIM_CHANNEL_1;

	PAC = PAC_Base_Create(&PAC_ini);

}
//----------------------------------------------------------------------------
void Soldering_Heat_Gun_INI(uint8_t Flash_Read_Status){

	Soldering_Heat_Gun.PID.KP=Soldering_Heat_Gun_PID_KP;
	Soldering_Heat_Gun.PID.KI=Soldering_Heat_Gun_PID_KI;
	Soldering_Heat_Gun.PID.KD=Soldering_Heat_Gun_PID_KD;
	Soldering_Heat_Gun.PID.dt=0.1;


	Soldering_Heat_Gun.Filter.Filter_Mode=Three_Samples;
	Soldering_Heat_Gun.Filter.k_min=0.03;
	Soldering_Heat_Gun.Filter.k_max=0.8;
	Soldering_Heat_Gun.Filter.Val_Delata=60;
	Soldering_Heat_Gun.Filter.mass=&ADC_Data[1];


	Soldering_Heat_Gun.MODE=MANUAL;

	Soldering_Heat_Gun.OFF_PIN=GPIO_PIN_15;
	Soldering_Heat_Gun.OFF_GPIO=GPIOC;
	Soldering_Heat_Gun.MANUAL_PIN=GPIO_PIN_9;
	Soldering_Heat_Gun.MANUAL_GPIO=GPIOB;

	Soldering_Heat_Gun.PAC = PAC;

	struct PAC_Devices_Control_Init element_ini;

	element_ini.Channel_GPIO = GPIOB;
	element_ini.Channel_PIN = GPIO_PIN_1;
	element_ini.TIM_CHANNEL = TIM_CHANNEL_4;

	Soldering_Heat_Gun.PAC_Control = PAC_Devise_Control_Create(PAC, &element_ini);


	Soldering_Heat_Gun.Temperature_Converting.Coeff=Soldering_Heat_Gun_Temperature_Converting_Coeff;

	if(Flash_Read_Status!=HAL_OK){									//if Data was read incorrect from flash reinitialize all user parameters
		Soldering_Heat_Gun.Temperature_Presets_C.Preset1=100;
		Soldering_Heat_Gun.Temperature_Presets_C.Preset2=200;
		Soldering_Heat_Gun.Temperature_Coolling.C=50;
		Soldering_Heat_Gun.Flash_Key=Soldering_Heat_Gun_Struct_Key;
	}

	Soldering_Heat_Gun_Flash_Read_Manual_Temperature(&Soldering_Heat_Gun); //read Last manual temperature

	Soldering_Heat_Gun_ini(&Soldering_Heat_Gun);

	Soldering_Heat_Gun_OFF(&Soldering_Heat_Gun);


}
//----------------------------------------------------------------------------
void Soldering_Separator_INI(uint8_t Flash_Read_Status){
	Soldering_Separator.PID.KP=Soldering_Separator_PID_KP;
	Soldering_Separator.PID.KI=Soldering_Separator_PID_KI;
	Soldering_Separator.PID.KD=Soldering_Separator_PID_KD;
	Soldering_Separator.PID.dt=0.1;


	Soldering_Separator.Filter.Filter_Mode=Three_Samples;
	Soldering_Separator.Filter.k_min=0.1;
	Soldering_Separator.Filter.k_max=0.3;
	Soldering_Separator.Filter.Val_Delata=60;
	Soldering_Separator.Filter.mass=&ADC_Data[13];


	Soldering_Separator.MODE=MANUAL;

	Soldering_Separator.PAC = PAC;

	struct PAC_Devices_Control_Init element_ini;

	element_ini.Channel_GPIO = GPIOA;
	element_ini.Channel_PIN = GPIO_PIN_7;
	element_ini.TIM_CHANNEL = TIM_CHANNEL_2;

	Soldering_Separator.PAC_Control = PAC_Devise_Control_Create(PAC, &element_ini);

	Soldering_Separator.Temperature_Converting.Coeff=Soldering_Separator_Temperature_Converting_Coeff;

	if(Flash_Read_Status!=HAL_OK){									//if Data was read incorrect from flash reinitialize all user parameters
		Soldering_Separator.Temperature_Presets_C.Preset1=150;
		Soldering_Separator.Temperature_Presets_C.Preset2=200;
		Soldering_Separator.Flash_Key=Soldering_Separator_Struct_Key;
	}

	Soldering_Separator_Flash_Read_Manual_Temperature(&Soldering_Separator);	//read Last manual temperature

	Soldering_Separator_ini(&Soldering_Separator);

	Soldering_Separator_OFF(&Soldering_Separator);
}
//----------------------------------------------------------------------------

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  //---------------------------------------------------------------------------------
  uint8_t Flash_Read_Status;

  Flash_Read_Status = Soldering_Station_Read_Struct(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);


  BUTTON_INI();
  Encoder_INI();
  MENU_INI();
  OLED_INI();
  PAC_INI();
  Soldering_Iron_INI(Flash_Read_Status);
  Soldering_Heat_Gun_INI(Flash_Read_Status);
  Soldering_Separator_INI(Flash_Read_Status);

  if(Flash_Read_Status!=HAL_OK)							//if Struct was read incorrect
	  Soldering_Station_Write_Struct(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);	//Rewrite all structures

  //---------------------------------------------------------------------------------TIM
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim1);
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_SR_UIF);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);


	__HAL_TIM_CLEAR_FLAG(&htim4, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim4);

  //---------------------------------------------------------------------------------



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  Face_UI_it(&Encoder_P1, &Encoder_P2, &Button_Vector);
	  UI_Menu_it(&Menu_List_Vector, &Encoder_P2, &Button_Vector);
	  Soldering_Station_Write_Struct(&Soldering_Iron, &Soldering_Heat_Gun, &Soldering_Separator);	//Write all struct in flash after changing in UI_Menu

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 16;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_11;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_12;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_13;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_14;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_15;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_16;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 799;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 499;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 399;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 799;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 499;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA8 PA9 PA10
                           PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB15 PB4 PB5
                           PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
