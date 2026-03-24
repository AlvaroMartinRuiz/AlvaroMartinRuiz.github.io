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
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
unsigned short estado = 1; // Variable to control the state (stop, turning right...) of the car
unsigned short buzzer = 1; //Variable to turn on/off the buzzer
unsigned short button_pressed; // Variable to check if the robot has been turned on
short DC = 40; //Duty cycle//30
short pin2_state; //IR sensors
short pin1_state;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TS_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// FUNCTIONS FOR MEOVEMENT
void stop(){
  GPIOC->BSRR = (1<<6)<<16;
  TIM3->CCR2 = 0; // Save new DC in CCR3

  GPIOC->BSRR = (1<<9)<<16;
  TIM3->CCR3 = 0; // Save new DC in CCR3

}

void backwards(){
  GPIOC->BSRR = (1<<6);
  TIM3->CCR2 = DC; // Save new DC in CCR3

  TIM3->CCR3 = DC; // Save new DC in CCR3
  GPIOC->BSRR = (1<<9);
}

void forward(){
  GPIOC->BSRR = (1<<6)<<16;
  //GPIOC->BSRR = (1<<7);
  TIM3->CCR2 = 50; // Save new DC in CCR3


 // GPIOC->BSRR = (1<<8);
  GPIOC->BSRR = (1<<9)<<16;
  TIM3->CCR3 = 50; // Save new DC in CCR3

}

void left(){
  GPIOC->BSRR = (1<<6)<<16;
 // GPIOC->BSRR = (1<<7);
  TIM3->CCR2 = 0; // Save new DC in CCR3


  //GPIOC->BSRR = (1<<8)<<16;
  TIM3->CCR3 = DC; // Save new DC in CCR3
  GPIOC->BSRR = (1<<9)<<16; //poner 16
}

void right(){
  GPIOC->BSRR = (1<<6)<<16;//poner <<16
  //GPIOC->BSRR = (1<<7)<<16;
  TIM3->CCR2 = DC; // Save new DC in CCR3

  //GPIOC->BSRR = (1<<8);
  GPIOC->BSRR = (1<<9)<<16;
  TIM3->CCR3 = 0; // Save new DC in CCR3

}
// end functions FOR MEOVEMENT

// IRQ HANDLERS
void EXTI0_IRQHandler(void) // ISR for EXTI0.
	// PC jumps here when the EXTI0 event occurs
{
	if (EXTI->PR!=0) {
		button_pressed=0;
		TIM4->CNT = 0; // Initialize the counter to 0
		TIM4->CR1 |= 0x0001; // CEN = 1 -> Start counter
		//The timer starts when the button is pressed

		EXTI->PR |= 0x01; // Clear the EXTI0 flag
		}
}



//IRQ HANDLER FUNCTIONS
void EXTI1_IRQHandler(void) {
    // Check if EXTI line 1's pending bit is set and the button has been pressed
    if ((EXTI->PR & (1 << 1)) && (button_pressed == 1)) {

    	pin2_state = ((GPIOC->IDR & 0x04) == 0); // True if PC2 is LOW (0) Another option: pin2_state = (GPIOC->IDR & (1 << 2)) == 0;
        pin1_state = ((GPIOC->IDR & 0x02) == 0); // True if PC1 is LOW (0) Another option: pin1_state = (GPIOC->IDR & (1 << 1)) == 0;
        // Determine action based on pin states
        if (pin1_state) { // FALLING edge for PC1
            if (pin2_state) { // Check if PC2 is detecting white/black
                estado = 1;
                forward();
            } else {
                estado = 3;
                left();
            }
        } else { // RISING edge for PC1
            if (pin2_state) {
                estado = 2;
                right();
            } else {
                estado = 0;
                stop();
            }
        }

        // Clear the EXTI1 flag
        EXTI->PR = (1 << 1); // Clear by writing 1 to PR[1]

    }
}

void EXTI2_IRQHandler(void) {
    // Ensure the interrupt was triggered for EXTI2 and button is pressed
    if ((EXTI->PR & (1 << 2)) && (button_pressed == 1)) {
        // Simplify the handling by combining conditions
    	pin2_state = ((GPIOC->IDR & 0x04) == 0); // True if PC2 is LOW (0)
    	pin1_state = ((GPIOC->IDR & 0x02) == 0); // True if PC1 is LOW (0)

        // Determine action based on pin states
        if (pin2_state) { // FALLING edge for PC2
            if (pin1_state) {
                estado = 1;
                forward();
            } else {
                estado = 2;
                right();
            }
        } else { // RISING edge for PC2
            if (pin1_state) {
                estado = 3;
                left();
            } else {
                estado = 0;
                stop();
            }
        }

        // Clear the EXTI2 flag
        EXTI->PR = (1 << 2); // Clear by writing 1 to PR[2]
    }
}



// Handler function for the BUZZER (TOC)
void TIM4_IRQHandler(void) {
	if ((TIM4->SR & 0x0002)!=0) // If the comparison is successful,
	//	then the IRQ is launched and this ISR is executed.
	//	This line check which event launched the ISR
	{
		// Check if the IRQ handler was launched because the button was pressed
		if( button_pressed ==0){
			button_pressed = 1;
			forward();
		}


		// Buzzer functionality
		TIM4->CCR1 += 500; // Update the comparison value, adding 1000 steps = 1 second
		// It is updated to 500 so the buzzer beeps every 500
		// It is on 500 cycles, and off 500 cycles, and so on
		TIM4->SR = 0x0000; // Clear all flags

		//BUZZER
		if ((estado == 2) || (estado == 3)) {
			//ZUMBADOR INTERMITENTE *****
			if(buzzer==1) { //si estaba ecendido, apagarlo (para conseguir intermitencia)
				//BUZZER ON
				GPIOB->BSRR=(1<<8)<<16; //set
				buzzer=0;
			}else{
				//PB8(buzzer) OFF
				GPIOB->BSRR=(1<<8); //reset
				buzzer=1;
			}

		}else if(estado ==0){ //pitido continuo
			//BUZZER ON
			GPIOB->BSRR=(1<<8)<<16; //encender buzzer, se enciende con un 0

		}else{
			//PA1(buzzer) OFF
			GPIOB->BSRR=(1<<8); //reset//buzzer off
		}

	}
}
// end IRQ HANDLERS


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
  MX_ADC_Init();
  MX_TS_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  //PINS FOR THE WHEELS
  //GPIOA->MODER &= ~(0X0000FFFF); //FORCE ALL BITS TO BE 0
  GPIOC->MODER &= ~(1 << (2*6+1));
  GPIOC->MODER |= (1<<2*6); // C6 OUTPUT

  GPIOC->MODER|=0x00000001 << (2*7+1); // MODER = 10 (AF) for PC7
  GPIOC->MODER&=~(0x00000001 << (2*7));
  GPIOC->AFR[0]|=(0x02 << (7*4)); // AFR[0] to associate PC0-PC7 to AF2 (TIM3)

  GPIOC->MODER|=0x00000001 << (2*8+1);
  GPIOC->MODER&=~(0x00000001 << (2*8));
  GPIOC->AFR[1]|=(0x02 << (4*2)); // AFR[1] to associate PC8-PC15 to AF2 (TIM3)

  GPIOC->MODER &= ~(1 << (2*9+1));
  GPIOC->MODER |=  (1<<2*9); //CHANGE C9 TO BE OUTPUT MODE
  //end PINS for the wheels

  // PINS FOR IR Sensors
  GPIOC->MODER &= ~(1 << 2); //1*2 PC1
  GPIOC->MODER &= ~(1 << 3); //1*2 +1 PC1
  GPIOC->MODER &= ~(1 << 4);// 2*2 PC2
  GPIOC->MODER &= ~(1 << 5); //2*2 +1 PC2
  //end for pin for sensors

  // PIN BUZZER (PB8)
  GPIOB->MODER &= ~(1<<17); // Clear current mode. Another option:  GPIOB->MODER &= ~(1 << (8 * 2 +1));
  GPIOB->MODER |= (1<<16);  // Set mode to output for PB8. Another option: GPIOB->MODER |= (1 << (8 * 2));
  GPIOB->BSRR = (1<<8); //inicialmente encendido
  // end buzzer


  // CONFIGURATION OF THE EXTIs FOR THE SENSORS
  EXTI->FTSR |= 0x06; // Enables falling edge trigger for EXTI1 and EXTI2
  EXTI->RTSR |= 0x06; // Enables rising edge trigger for EXTI1 and EXTI2

  SYSCFG->EXTICR[0] &= ~((0xF << 4) | (0xF << 8)); // Clear the previous settings for EXTI1 and EXTI2
  SYSCFG->EXTICR[0] |= ((0x2 << 4) | (0x2 << 8)); // Set EXTI1 and EXTI2 to be connected to GPIOC

  EXTI->IMR |= 0x06; // Enables EXTI1 and EXTI2 interrupts
//  NVIC->ISER[0] |= (1<<7);
//  NVIC->ISER[0] |= (1 << 8);// Enables NVIC IRQ for EXTI1 and EXTI2
  // end EXTI configuration

  //CONFIGURATION OF THE EXTI FOR THE USER BUTTON
  GPIOA->MODER &= ~(1 << (0*2 +1)); // PA0 (USER button) as digital input (00)
  GPIOA->MODER &= ~(1 << (0*2)); // PA0 (USER button) as digital input (00)
  EXTI->FTSR |= 0x01; // Enables falling edge in EXTI0
  EXTI->RTSR &= ~(0x01); // Disables rising edge in EXTI0
  SYSCFG->EXTICR[0] &= ~(0xF); // EXTI0 linked to GPIOA (i.e. USER button = PA0)
  EXTI->IMR |= 0x01; // Enables EXTI0
  NVIC->ISER[0] |= (1 << 6); // Enables IRQ for EXTI0 (6th position)
  NVIC->ISER[0] |= (1<<7);
  NVIC->ISER[0] |= (1 << 8);// Enables NVIC IRQ for EXTI1 and EXTI2
  // end EXTI of the button configuration


  //TOC CONFIGURATION
  // Select the internal clock: CR1, CR2, SMRC
  TIM4->CR1 = 0x0000; // ARPE = 0 -> No PWM, it is TOC  CEN = 0; Counter OFF
  TIM4->CR2 = 0x0000; // Always "0" in this course
  TIM4->SMCR = 0x0000; // Always "0" in this course
  // Counter behavior setting: PSC, CNT, ARR and CCRx
  TIM4->PSC = 31999; // Pre-scaler=32000 -> F_counter=32000000/32000 = 1000 steps/second
  TIM4->CNT = 0; // Initialize the counter to 0
  TIM4->ARR = 0xFFFF; // Recommended value = FFFF (maximum)
  TIM4->CCR1 = 2000; // Record that stores the value for the comparison.2000=2s
  // The fisrt time is initialize to 2000, so after the button is pressed it waits 2 seconds
  TIM4->DIER = 0x0002; // An IRQ is generated as the comparison is successful -> CCyIE = 1
  TIM4->CCMR1 = 0x0000; // CCyS = 0 (TOC)
  TIM4->CCER = 0x0000; // CCyP = 0 (always in TOC)   CCyE = 0 (external output disabled)

  TIM4->EGR |= 0x0001; // UG = 1 -> Generate an update event to update all registers
  TIM4->SR = 0; // Clear counter flags
  NVIC->ISER[0] |= (1 << 30); //30 is TIM4

  /**********************************
  //LAB 3
  **********************************/

   //PWM3 INITIALIZATION
   TIM3 -> CR1 = 0x0080; //ACTIVATE PWM MODE
   TIM3 -> CR2 = 0x0000;
   TIM3 -> SMCR = 0x0000;
   TIM3 -> PSC = 31999;
   TIM3 -> CNT = 0;
   TIM3 -> ARR = 100;// MAXIMUM VALUE OF THE COUNTER (cuando llega vuelve a 0s)
   TIM3 -> CCR3 = 0;
   TIM3 -> CCR2 = 0;
   TIM3 -> DIER = 0x0000;//no interrupts

   TIM3 -> CCMR2 = 0x0068;//IN BOTH CHANNELS GOES THE SAME CONFIGURATION
   TIM3 -> CCMR1 = 0x6800;//IN BOTH CHANNELS GOES THE SAME CONFIGURATION
   //Starting the cycle at 1
   TIM3 -> CCER = 0x0110;//0x1100;(mal)// ALWAYS IN PWM AND HARDWARE OUTPUT ACTIVE
   TIM3 -> CR1 |= 0x0001;
   TIM3 -> EGR |= 0x0001;
   TIM3 -> SR = 0;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC3;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
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
  * @brief TS Initialization Function
  * @param None
  * @retval None
  */
static void MX_TS_Init(void)
{

  /* USER CODE BEGIN TS_Init 0 */

  /* USER CODE END TS_Init 0 */

  /* USER CODE BEGIN TS_Init 1 */

  /* USER CODE END TS_Init 1 */
  /* USER CODE BEGIN TS_Init 2 */

  /* USER CODE END TS_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SEG14_Pin SEG15_Pin SEG16_Pin SEG17_Pin
                           SEG18_Pin SEG21_Pin SEG22_Pin SEG23_Pin */
  GPIO_InitStruct.Pin = SEG14_Pin|SEG15_Pin|SEG16_Pin|SEG17_Pin
                          |SEG18_Pin|SEG21_Pin|SEG22_Pin|SEG23_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin COM0_Pin
                           COM1_Pin COM2_Pin SEG12_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|COM0_Pin
                          |COM1_Pin|COM2_Pin|SEG12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG6_Pin SEG7_Pin SEG8_Pin SEG9_Pin
                           SEG10_Pin SEG11_Pin SEG3_Pin SEG4_Pin
                           SEG5_Pin SEG13_Pin COM3_Pin */
  GPIO_InitStruct.Pin = SEG6_Pin|SEG7_Pin|SEG8_Pin|SEG9_Pin
                          |SEG10_Pin|SEG11_Pin|SEG3_Pin|SEG4_Pin
                          |SEG5_Pin|SEG13_Pin|COM3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_LCD;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
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
