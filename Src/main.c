/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */
//basic C libraries
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//TIMER
int tim2_flag;									//interrupt flag
int count =1;									//counter int

//USART
char Rx_indx1, Rx_data1[2], Rx_Buffer1[100];
char Rx_indx2, Rx_data2[2], Rx_Buffer2[100];
char inputReceived[100];
char messageReceived[100];
int flag_uart;
int flag_message;

//UI
int state = 0;									//state machine

//ADC
int adc_reading = 0;							//ADC value to quantize connection

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void transmitString(UART_HandleTypeDef *huart, char message[]);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void new_line();
void clearString();
void open_prompt();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//Interrupt callback routine
{
	//this callback routine allows the user to parse their data and submit it using a the ENTER key
	//assigns each bit of Rx_data to a char the user input
    uint8_t i;//reusable counter
    uint8_t j;//reusable counter
    if (huart->Instance == USART1)  							//USART1: for getting data to send from user
        {
        if (Rx_indx1==0) {for (i=0;i<100;i++) Rx_Buffer1[i]=0;} //clear Rx_Buffer before receiving new data

        if (Rx_data1[0]!=13) 									//if received data different from ascii 13 (enter)
            {
            Rx_Buffer1[Rx_indx1++]=Rx_data1[0];    				//add data to Rx_Buffer
            }
        else //if received data = 13 (a.k.a. ENTER)
            {
        	flag_uart = 1;										//transfer complete, data is ready to read
        	strcpy(inputReceived, Rx_Buffer1);					//copy the string for use outside the function
        	Rx_Buffer1[Rx_indx1] = 0;							//begin clearing buffer
        	Rx_indx1=0;
			for (i = 0; i < 100; i++) Rx_Buffer1[i] = 0;		// clear buffer
            }
        HAL_UART_Receive_IT(&huart1, Rx_data1, 1);   			//activate UART receive interrupt
        }

    else if (huart->Instance == USART2)  						//USART2: for message reception
		{
    	if (Rx_indx2==0) {for (j=0;j<100;j++) Rx_Buffer2[j]=0;} //clear Rx_Buffer before receiving new data

		if (Rx_data2[0]!=10) 									//if data different from ascii 10 (a.k.a. '\n')
			{
			Rx_Buffer2[Rx_indx2++]=Rx_data2[0];    				//add data to Rx_Buffer
			}
		else
			{
			flag_message = 1;									//transfer complete, data is ready to read
			strcpy(messageReceived, Rx_Buffer2); 				//copy the string for use outside the function
			Rx_Buffer2[Rx_indx2] = 0; 							//begin clearing buffer
			Rx_indx2=0;
			for (j = 0; j < 100; j++) Rx_Buffer2[j] = 0; 		// clear buffer
			}

		HAL_UART_Receive_IT(&huart2, Rx_data2, 1);   			//activate UART receive interrupt every time
		}
}
void transmitString(UART_HandleTypeDef *huart, char* message)
{
	/*
	 * This function takes a string of characters and transmits it via Tx of the STM32F0.
	 * 	- Tx of STM32F0 is connected to the laser diode
	 */
	char buffer[100];											//array for the string
	char blank_line[100];										//array for a blank line
	int tmp;													//length of transmission
	tmp = sprintf(buffer,message);								//format string
    HAL_UART_Transmit(huart, (uint8_t *)message, tmp, 1000);	//display on serial port
    return;
}
void new_line()
{
	char blank_line[100];										//array for a blank line
	int tmp;													//length of transmission
    tmp=sprintf(blank_line,"\n");								//format new line
    HAL_UART_Transmit(&huart1, (uint8_t *)blank_line, tmp, 1000);//display on serial port
    return;
}
void receiveString()
{
    while(flag_uart == 0) //holds for 'Press any key to begin.' feature
    {
    }
    flag_uart = 0;
    return;
}
void clearString( char * string,  int size)  	//This function is used to clear a string
{
	int i = 0;
    for (i = 0; i < size; i++) string[i] = 0; 	// clear buffer
    return;
}
void open_prompt()
{
	char txPromp[100] = 	"  You: ";
	new_line();									//new line for home-screen
	transmitString(&huart1,txPromp);
	return;
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
    int config_flag = 0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  	//start peripherals
    HAL_UART_Receive_IT(&huart1, Rx_data1, 1);	//activate UART Rx  w/ interrupts
    HAL_ADC_Start_IT(&hadc);					//enable the ADC w/ interrupts
    HAL_TIM_Base_Start_IT(&htim2);				//start the timer
    HAL_UART_Receive_IT(&huart2, Rx_data2, 1);	//activate laser
    //user interface strings
    char txSTARZ[100]  =   "******************************************************************************* \r\n";
    char txGreet[100]  =   "*************************Hi User, welcome to Lazer-Chat!*********************** \r\n";
    char txReady[100]  =   "***** Press any key to begin. *************************************************";
    char txUname[100]  =   "***** Please enter your name: ";
    char txCalib0[100] =   "***** CALIBRATING... ********************************************************** \r\n";
    char txCalib1[100] =   "***** CALIBRATION COMPLETE. *************************************************** \r\n";
    char txInstr[100]  =   "***** Instructions: to send a message, hit ENTER. For more info type (h) ****** \r\n";
    char txERROR[100]  =   "************************ERROR - RECALIBRATION REQUIRED.************************ \r\n";
    char txHelp1[100]  =   "********************* Welcome to our ECE362 Mini-Project. ********************* \r\n";
    char txHelp2[100]  =   "***** Team Members: *********************************************************** \r\n";
    char txHelp3[100]  =   "******** Bradford Thorne ****************************************************** \r\n";
    char txHelp4[100]  =   "******** Kevin Hunckler ******************************************************* \r\n";
    char txHelp5[100]  =   "******** Sahil Bhalla ********************************************************* \r\n";
    char txHelp6[100]  =   "******** Andrew St. Pierre **************************************************** \r\n";
    //char txHelp6[100]  =   "******************************************************************************* \r\n";
    char name[100];
    char temp[100];
    char blank[100]= "\n";
    char padding[100]= "  ";
    int len;
    int starting = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  	//STATE MACHINE
		if (state == 0)							//turn key functionality
		{
		  transmitString(&huart1,txSTARZ);
		  transmitString(&huart1,txGreet);
		  transmitString(&huart1,txSTARZ);
		  transmitString(&huart1,txReady);
		  receiveString();						//wait for key press
		  new_line();
		  transmitString(&huart1,txSTARZ);

		  state =1;								//advance state
		}
		else if (state==1)						//get user's name
		{
			transmitString(&huart1,txUname);	//prompt for user's name
			receiveString();					//wait until name entered
			new_line();							//new line formatting
			strcpy(name,padding);				//pad w/ two spaced
			strcat(name,inputReceived);			//add user name to variable
			strcat(name,": ");					//formatting new line
			len = strlen(inputReceived);		//get length for clearing
			clearString(inputReceived,len);		//clear buffer
			state = 2;
		}
		else if (state == 2)					//configure laser
		{
			transmitString(&huart1,txCalib0);
		  if (adc_reading >= 2000)				//check for good laser-photodiode connection
		  {
			transmitString(&huart1,txCalib1);
			transmitString(&huart1,txSTARZ);
			state = 3;							//advance state
		  }
		  else
		  {
			transmitString(&huart1,txCalib0);
			transmitString(&huart1,txSTARZ);
			HAL_Delay(750);
			state = 2;							//retain state
		  }
		}
		else if (state == 3)					//initialize chat
		{
			transmitString(&huart1,txSTARZ);
			transmitString(&huart1,txInstr);
			transmitString(&huart1,txSTARZ);
			while (adc_reading <= 2000)
			{}									//wait for good connection to begin chat
			transmitString(&huart2,blank);
			HAL_Delay(200);
			state =5;
		}
		else if (state==4)						//begin chat
		{
			open_prompt();
			state = 5;
		}
		else if (state == 5)
		{
			//user enters a message
			if (flag_uart==1)
			{

				flag_uart = 0;							//clear flag

				//check for help
				if ((inputReceived[0]==104) && (inputReceived[1]==00))	//char 'h' ascII value
				{
					transmitString(&huart1,txSTARZ);
					transmitString(&huart1,txHelp1);
					transmitString(&huart1,txSTARZ);
					transmitString(&huart1,txHelp2);
					transmitString(&huart1,txHelp3);
					transmitString(&huart1,txHelp4);
					transmitString(&huart1,txHelp5);
					transmitString(&huart1,txHelp6);
					transmitString(&huart1,txSTARZ);
					len = strlen(inputReceived);		//get length for clearing
					clearString(inputReceived,len);		//clear buffer
					state = 4;							//prompt for input again
				}
				else
				{
				strcpy(temp,name);				//store in temp
				strcat(temp,inputReceived);		//add user message
				strcat(temp,"\n");				//formatting new line
				transmitString(&huart2,temp);	//send via laser

				len = strlen(inputReceived);	//get length for clearing
				clearString(inputReceived,len);	//clear buffer
				len = strlen(temp);				//get length for clearing
				clearString(temp,len);			//clear buffer
				state = 4;						//repeat state 3
				}
			}
			//user receives a message
			else if (flag_message == 1)
			{
				flag_message = 0;				//clear flag
				strcpy(temp,messageReceived);	//store in temp
				new_line();						//formatting new line
				transmitString(&huart1,temp);	//print to user's screen

				len = strlen(messageReceived);	//get length for clearing
				clearString(messageReceived,len);//clear buffer
				len = strlen(temp);				//get length for clearing
				clearString(temp,len);			//clear buffer
				state = 4;						//repeat state 3
			}
			else state = 5;
		}
		if (config_flag==1)
		{
			config_flag=0;
			new_line();
			transmitString(&huart1,txERROR);	//print error
			state = 2; 							//wait for adc_reading to return to high val
		}
		if (tim2_flag==1)						//interrupt driven
		{
		  tim2_flag=0;							//clear flag
		  if (count == 15)						//slowed down blinking
		  {
			  	count = 0;						//reset counter to 0
				if (adc_reading <= 2000)		//if laser was on photodiode, 2000 was easily broken
				{
				  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);//toggle LED
				  config_flag = 1;
				}
				else if (adc_reading >= 1999)	//connected properly
				{
				  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//set red led to ON
				  config_flag = 0;
				}
		  }
		  else count = count+1;					//increment timer
	   }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC init function */
static void MX_ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 48000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 49;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 1600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 1600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
