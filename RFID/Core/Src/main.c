/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "lcd.h"
#include "rc522.h"
#include <stdbool.h>
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
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char screen1[16]="";
char screen2[16]="";
u_char status = 0;
u_char cardStr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t serNum[5];
uint8_t key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t cardBlock[16];
uint8_t digit1 = 0;
uint8_t digit2 = 0;
uint8_t digit3 = 0;
uint32_t credit = 0;
bool cardState = false;

void singleBip()
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

void multiBip(uint8_t cnt)
{
	for(uint8_t i=0; i<cnt; i++)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
}

void checkButtons()
{
	if(!HAL_GPIO_ReadPin(GPIOA, BTN_1_Pin))
	{
		if(digit1 != 9) {
			digit1++;
		}
		else {
			digit1 = 0;
		}

		credit = digit3 * 1000 + digit2 * 100 + digit1 * 10;

		lcdClear();
		lcdPrint(1,1,"KREDi YUKLE");
		sprintf(screen2,"KREDi : %d ",credit);
		lcdPrint(2,1,screen2);

		HAL_Delay(100);
	}

	if(!HAL_GPIO_ReadPin(GPIOA, BTN_2_Pin))
	{
		if(digit2 != 9) {
			digit2++;
		}
		else {
			digit2 = 0;
		}

		credit = digit3 * 1000 + digit2 * 100 + digit1 * 10;

		lcdClear();
		lcdPrint(1,1,"KREDi YUKLE");
		sprintf(screen2,"KREDi : %d ",credit);
		lcdPrint(2,1,screen2);

		HAL_Delay(100);
	}

	if(!HAL_GPIO_ReadPin(GPIOA, BTN_3_Pin))
	{
		if(digit3 != 9) {
			digit3++;
		}
		else {
			digit3 = 0;
		}

		credit = digit3 * 1000 + digit2 * 100 + digit1 * 10;

		lcdClear();
		lcdPrint(1,1,"KREDi YUKLE");
		sprintf(screen2,"KREDi : %d ",credit);
		lcdPrint(2,1,screen2);

		HAL_Delay(100);
	}

	if(!HAL_GPIO_ReadPin(GPIOA, BTN_4_Pin))
	{
		status = MFRC522_Request(PICC_REQIDL, cardStr);
		status = MFRC522_Anticoll(cardStr);
		memcpy(serNum,cardStr,5);


		if(status == MI_OK)
		{
			MFRC522_SelectTag(cardStr);
			uint8_t result = MFRC522_Auth(PICC_AUTHENT1A,2,key,serNum);

			status = MFRC522_Read(2,cardBlock);

			if(status == MI_OK)
			{
				cardBlock[0] = (credit & 0xFF);
				cardBlock[1] = ((credit >> 8) & 0xFF);
				cardBlock[2] = ((credit >> 16) & 0xFF);
				cardBlock[3] = ((credit >> 24) & 0xFF);

				status = MFRC522_Write(2, cardBlock);

				if(status == MI_OK)
				{
					multiBip(2);

					lcdClear();
					lcdPrint(1,1,"YUKLEME");
					lcdPrint(2,1,"TAMAMLANDI");

					HAL_Delay(2000);

					lcdClear();
					lcdPrint(1,1,"LUTFEN");
					lcdPrint(2,1,"KART OKUTUN");

					cardState = false;

					MFRC522_Halt();
					MFRC522_Reset();
					MFRC522_Init();
				}
				else
				{
					multiBip(3);

					lcdClear();
					lcdPrint(1,1,"HATA OLUSTU");

					HAL_Delay(2000);

					lcdClear();
					lcdPrint(1,1,"LUTFEN");
					lcdPrint(2,1,"KART OKUTUN");

					cardState = false;

					MFRC522_Halt();
					MFRC522_Reset();
					MFRC522_Init();
				}
			}
		}
		else
		{
			multiBip(3);

			lcdClear();
			lcdPrint(1,1,"HATA OLUSTU");

			HAL_Delay(2000);

			lcdClear();
			lcdPrint(1,1,"LUTFEN");
			lcdPrint(2,1,"KART OKUTUN");

			cardState = false;

			MFRC522_Halt();
			MFRC522_Reset();
			MFRC522_Init();
		}
	}
}

void checkCard()
{
	if(!cardState)
	{
		status = MFRC522_Request(PICC_REQIDL, cardStr);
		status = MFRC522_Anticoll(cardStr);
		memcpy(serNum,cardStr,5);


		if(status == MI_OK)
		{
			MFRC522_SelectTag(cardStr);
			uint8_t result = MFRC522_Auth(PICC_AUTHENT1A,2,key,serNum);

			status = MFRC522_Read(2,cardBlock);

			if(status == MI_OK)
			{
				singleBip();

				int credit1 = cardBlock[3];
				int credit2 = cardBlock[2];
				int credit3 = cardBlock[1];
				int credit4 = cardBlock[0];

				credit = ((credit4 << 0) & 0xFF) + ((credit3 << 8) & 0xFFFF) + ((credit2 << 16) & 0xFFFFFF) + ((credit1 << 24) & 0xFFFFFFFF);

				if(credit > 9999)
				{
					credit = 9999;
				}

				digit3 = credit / 1000;
				digit2 = (credit % 1000) / 100;
				digit1 = (credit % 100) / 10;

				lcdClear();
				lcdPrint(1,1,"KREDi YUKLE");
				sprintf(screen2,"KREDi : %d",credit);
				lcdPrint(2,1,screen2);

				cardState = true;

				MFRC522_Halt();
				MFRC522_Reset();
				MFRC522_Init();
			}
			else
			{
				multiBip(3);

				lcdClear();
				lcdPrint(1,1,"HATA OLUSTU");

				HAL_Delay(2000);

				lcdClear();
				lcdPrint(1,1,"LUTFEN");
				lcdPrint(2,1,"KART OKUTUN");

				cardState = false;

				MFRC522_Halt();
				MFRC522_Reset();
				MFRC522_Init();
			}
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  lcdInit(_LCD_4BIT,_LCD_FONT_5x8,_LCD_2LINE);

  HAL_GPIO_WritePin(RC522_RST_GPIO_Port, RC522_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  MFRC522_Init();

  status = Read_MFRC522(VersionReg);

  singleBip();

  lcdClear();
  lcdPrint(1,1,"LUTFEN");
  lcdPrint(2,1,"KART OKUTUN");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  checkCard();
	  checkButtons();
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
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
	Error_Handler();
	}
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RC522_CS_Pin|RC522_RST_Pin|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_RS_Pin|LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin
                          |LCD_D6_Pin|LCD_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RC522_CS_Pin RC522_RST_Pin BUZZER_Pin */
  GPIO_InitStruct.Pin = RC522_CS_Pin|RC522_RST_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_EN_Pin LCD_D4_Pin LCD_D5_Pin
                           LCD_D6_Pin LCD_D7_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin
                          |LCD_D6_Pin|LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_4_Pin BTN_3_Pin BTN_2_Pin BTN_1_Pin */
  GPIO_InitStruct.Pin = BTN_4_Pin|BTN_3_Pin|BTN_2_Pin|BTN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
