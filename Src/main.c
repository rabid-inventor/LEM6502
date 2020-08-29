/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include <stdio.h>
#include "rom.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RAM_SIZE 16384
#define DEBUG_INFO printf( "DEBUG:::: LINE %d FILE %s in function %s \n", __LINE__, __FILE__ , __func__)
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#define GETCHAR_PROTOTYPE int __io_getchar(void) 
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f) 
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t curkey = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t getkey() ;
void serout(uint8_t val);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t RAM[RAM_SIZE];



const unsigned char BIOStop[256]  = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,0xA2,0xFF,0x9A,
	0xA0,0x1C,0xB9,0xBB,0xFF,0x99,0x04,0x02,0x88,0xD0,0xF7,
	0xB9,0xD8,0xFF,0xF0,0x06,0x20,0xED,0xE0,0xC8,0xD0,0xF5,
	0x20,0xEA,0xE0,0x90,0xFB,0x29,0xDF,0xC9,0x57,0xF0,0x07,
	0xC9,0x43,0xD0,0xD7,0x4C,0x00,0xC0,0x4C,0x00,0x00,0x8D,
	0x01,0xF0,0x60,0xAD,0x04,0xF0,0xF0,0x02,0x38,0x60,0x18,
	0x60,0xB3,0xFF,0xAF,0xFF,0xBB,0xFF,0xBB,0xFF,0x48,0xA5,
	0xDF,0x4A,0x05,0xDF,0x85,0xDF,0x68,0x40,0x48,0xA5,0xDC,
	0x4A,0x05,0xDC,0x85,0xDC,0x68,0x40,0x0D,0x0A,0x36,0x35,
	0x30,0x32,0x20,0x45,0x68,0x42,0x41,0x53,0x49,0x43,0x20,
	0x5B,0x43,0x5D,0x6F,0x6C,0x64,0x2F,0x5B,0x57,0x5D,0x61,
	0x72,0x6D,0x20,0x3F,0x00,0x00,0x00,0x00,0x17,0x02,0x80,
	0xFF,0x0D,0x02
};

uint8_t read6502(uint16_t address) {
    uint16_t BIOSaddr;
	//printf("%x\n",address);
    if (address == 0xF004) { //EhBASIC simulated ASIC input
        return getkey();
    }

    if (address >= 0xC000) {
        BIOSaddr = address - 0xC000;
        if (BIOSaddr < sizeof BIOS)
            return((uint8_t)(BIOS [BIOSaddr]));
        if (BIOSaddr >= 0x3F00)
            return((uint8_t)(BIOStop [ BIOSaddr - 0x3F00]));
    }

    if (address < RAM_SIZE) return(RAM[address]);
    return(0);
}

void write6502(uint16_t address, uint8_t value) {
	//printf("%x , %x\n",address,value);
    if (address < RAM_SIZE) RAM[address] = value;
    if (address == 0xF001) { //EhBASIC simulated ASIC output
        serout(value);
    }
}
	

/*Custom printf debuging*/ 
PUTCHAR_PROTOTYPE   /**RETARGET PRINTF TO UART1***/
{	
	
   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

GETCHAR_PROTOTYPE
{
	uint8_t ch;
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 0xff);
	return ch;
}


   void serout(uint8_t val) {
        printf("%c",val);
    }
		
	uint8_t getkey() {
				
				HAL_UART_Receive(&huart1, (uint8_t *)&curkey, 1, 0x2);
        uint8_t key = curkey & 0x7F;
        curkey = 0;
        return key;
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
  //MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	reset6502();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	//step6502();
	exec6502(1000);

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC14 PC15 PC0 
                           PC1 PC2 PC3 PC4 
                           PC5 PC6 PC7 PC8 
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 
                           PA4 PA5 PA6 PA7 
                           PA8 PA11 PA12 PA13 
                           PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 
                          |GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
