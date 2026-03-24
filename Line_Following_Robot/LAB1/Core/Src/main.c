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
#include "Utiles_SDM.h"
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

LCD_HandleTypeDef hlcd;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
unsigned short estado = 0;
unsigned short buzzer = 1; //podemos usar la libreria uint8_t??

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_LCD_Init(void);
static void MX_TS_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void stop(void);
void forward(void);
void backwards(void);
void left(void);
void right(void);
void espera(int);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void EXTI1_IRQHandler(void) {
    // Check if EXTI line 0's pending bit is set (an event has happened)
	if (EXTI->PR != 0) {

    	// FALLING EVENT:
        // Check the current state of the pin to determine the type of event
    	//If there is currently a 0, a falling event has happened
		if ((GPIOC->IDR & (1 << 1)) == 0) {

    		// The pin is low, indicating a falling edge event
    		// Insert logic to handle a falling edge event
    		// Now, check the state of PC2
			if ((GPIOC->IDR & (1 << 2)) == 0) {

				estado = 1; //estado is use to call to the movememtn functions in the main
			} else {
				estado =3;
			}
		} else {
			// The pin is high, indicating a rising edge event
			// check the state of PC2
			if ((GPIOC->IDR & (1 << 2)) == 0) {
				estado = 2;
			} else {
				estado = 0;
			}
		}

        EXTI->PR = 0x02; //  clear the EXTI1 flag
    }
}


void EXTI2_IRQHandler(void) {
	// Check PC2 to see if we are in a falling or rising event
	if (EXTI->PR != 0) {

		// Check the current state of the pin to determine the type of event
		if ((GPIOC->IDR & (1 << 2)) == 0) {
			// FALLING edge event
			// Now, check the state of PC1
			if ((GPIOC->IDR & (1 << 1)) == 0) {
				estado = 1;

			} else {
				estado = 2;
			}
		} else {
			// RISING  edge event
			// Again, check the state of PC1
			if ((GPIOC->IDR & (1 << 1)) == 0) { //pin is 0
				estado = 3;
			} else {
				estado = 0;
			}
		}

		EXTI->PR = 0x04; //  clear the EXTI2 flag
	}
}


// Handler function for the TOC
void TIM4_IRQHandler(void) {
	if ((TIM4->SR & 0x0002)!=0) // If the comparison is successful, then the IRQ is launched
	// and this ISR is executed. This line check which event
	// launched the ISR
	{/*
		if(buzzer!=0){
			buzzer=0;
		}else{
			buzzer=1;
		}*/
		TIM4->CCR1 += 500; // Update the comparison value, adding 1000 steps = 1 second
		TIM4->SR = 0x0000; // Clear all flags



		//BUZZER
		if ((estado == 2) || (estado == 3)) {
			//ZUMBADOR INTERMITENTE *****
			if(buzzer==1) {
				//BUZZER ON
				GPIOB->BSRR=(1<<8)<<16; //set
				buzzer=0;
			}else{
				//PB8(buzzer) OFF
				GPIOB->BSRR=(1<<8); //reset
				buzzer=1;
			}

		}else if(estado ==0){
			//BUZZER ON
			GPIOB->BSRR=(1<<8)<<16; //encender buzzer, se enciende con un 0

		}else{
			//PA1(buzzer) OFF
			GPIOB->BSRR=(1<<8); //reset//buzzer off
		}

	}
}

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
  MX_LCD_Init();
  MX_TS_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */


  //GPIOA->MODER &= ~(0X0000FFFF); //FORCE ALL BITS TO BE 0
  GPIOC->MODER &= ~(1 << 13);
  GPIOC->MODER &= ~(1 << 15);
  GPIOC->MODER |= ((1<<12) | (1<<14)); //CHANGE BITS 22 AND 24 (A11,A12) TO 1, TO BE OUTPUT MODE)

   //GPIOB->MODER &= ~(0X0000FFFF); //FORCE ALL BITS TO BE 0
  GPIOC->MODER &= ~(1 << 17);
  GPIOC->MODER &= ~(1 << 19);
  GPIOC->MODER |= ((1<<16) | (1<<18)); //CHANGE BITS 12 AND 14 (B6,B7) TO 1, TO BE OUTPUT MODE)

  // Pins IR Sensors
  GPIOC->MODER &= ~(1 << 2); //1*2
  GPIOC->MODER &= ~(1 << 3); //1*2 +1
  GPIOC->MODER &= ~(1 << 4);// 2*2
  GPIOC->MODER &= ~(1 << 5); //2*2 +1
  /* otra forma de programar los IR
  //opcion prueba
  GPIOC->MODER &= ~(3 << (1 * 2)); // Clear both bits for pin 1
  GPIOC->MODER &= ~(3 << (2 * 2)); // Clear both bits for pin 2
*/
  // Pins buzzer
  GPIOB->MODER &= ~(1<<17); // Clear current mode. Another option:  GPIOB->MODER &= ~(1 << (8 * 2 +1));
  GPIOB->MODER |= (1<<16);  // Set mode to output for PB8. Another option: GPIOB->MODER |= (1 << (8 * 2));
  GPIOB->BSRR = (1<<8); //inicialmente apagado




  EXTI->FTSR |= 0x06; // Enables falling edge trigger for EXTI1 and EXTI2
  EXTI->RTSR |= 0x06; // Enables rising edge trigger for EXTI1 and EXTI2


  SYSCFG->EXTICR[0] &= ~((0xF << 4) | (0xF << 8)); // Clear the previous settings for EXTI1 and EXTI2
  SYSCFG->EXTICR[0] |= ((0x2 << 4) | (0x2 << 8)); // Set EXTI1 and EXTI2 to be connected to GPIOC

  //SYSCFG->EXTICR[0] = 0x02;


  EXTI->IMR |= 0x06; // Enables EXTI1 and EXTI2 interrupts
  NVIC->ISER[0] |= (1<<7);
  NVIC->ISER[0] |= (1 << 8);// Enables NVIC IRQ for EXTI1 and EXTI2

  //TOC
  // Select the internal clock: CR1, CR2, SMRC
  TIM4->CR1 = 0x0000; // ARPE = 0 -> No PWM, it is TOC
  // CEN = 0; Counter OFF
  TIM4->CR2 = 0x0000; // Always "0" in this course
  TIM4->SMCR = 0x0000; // Always "0" in this course
  // Counter behaviour setting: PSC, CNT, ARR and CCRx
  TIM4->PSC = 31999; // Pre-scaler=32000 -> F_counter=32000000/32000 = 1000 steps/second
  TIM4->CNT = 0; // Initialize the counter to 0
  TIM4->ARR = 0xFFFF; // Recommended value = FFFF
  TIM4->CCR1 = 500; // Record that stores the value for the comparison.
  // It is initialized to the first value = 1000 steps = 1 second
  // Selecting IRQ or not: DIER
  TIM4->DIER = 0x0002; // An IRQ is generated as the comparison is successful -> CCyIE = 1
  // Output mode
  TIM4->CCMR1 = 0x0000; // CCyS = 0 (TOC)
  // OCyM = 000 (no external output)
  // OCyPE = 0 (no preload)
  TIM4->CCER = 0x0000; // CCyP = 0 (always in TOC)
  // CCyE = 0 (external output disabled)
  // Counter enabling
  TIM4->EGR |= 0x0001; // UG = 1 -> Generate an update event to update all registers
  TIM4->SR = 0; // Clear counter flags
  // Enabling TIM4_IRQ at NVIC (position 30).
  TIM4->CR1 |= 0x0001; //¿lo hace isa?
  NVIC->ISER[0] |= (1 << 30);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if (estado == 0) {
	      stop();

	  } else if (estado == 1) {
	      forward();
	  } else if (estado == 2) {
		  left();
	  } else {
	      right();
	  }

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LCD;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  * @brief LCD Initialization Function
  * @param None
  * @retval None
  */
static void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_16;
  hlcd.Init.Duty = LCD_DUTY_1_4;
  hlcd.Init.Bias = LCD_BIAS_1_4;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_0;
  hlcd.Init.DeadTime = LCD_DEADTIME_0;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_0;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

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

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

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
