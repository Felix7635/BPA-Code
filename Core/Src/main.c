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
/**
 *
 * TIM7 	-- milliseconds
 * TIM13	-- seconds
 * TIM11	-- 10us
*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "DMX.h"
#include "Component_test.h"
#include "lcd.h"
#include "Encoder.h"
#include "button.h"

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

/* USER CODE BEGIN PV */
//LCD
Lcd_HandleTypeDef lcd;
//FatFs & SD
FATFS filesystem;
FIL testfil;
FRESULT fres;
char path[8];
uint8_t testvalue = 0;
uint8_t testarray[5] = {0, 0, 0, 0, 0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

uint16_t convert8to16(uint8_t highbyte, uint8_t lowbyte);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Record_DMX_sec(uint16_t seconds, DMX_TypeDef *hdmx);
void DMX_Playback(DMX_TypeDef *hdmx);
void notify(uint8_t cycles, uint16_t delay);

void LCD_init();
void LED_init();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  HAL_Delay(1000);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_TIM9_Init();
  MX_TIM14_Init();
  MX_TIM11_Init();
  MX_TIM13_Init();
  MX_TIM7_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	htim1.Instance->CCR4 = 40;		//Standard Kontrastwert

	Lcd_PinType pins[8] = {
			LCD_D0_Pin,
			LCD_D1_Pin,
			LCD_D2_Pin,
			LCD_D3_Pin,
			LCD_D4_Pin,
			LCD_D5_Pin,
			LCD_D6_Pin,
			LCD_D7_Pin
	};

	Lcd_PortType ports[8] = {
			LCD_D0_GPIO_Port,
			LCD_D1_GPIO_Port,
			LCD_D2_GPIO_Port,
			LCD_D3_GPIO_Port,
			LCD_D4_GPIO_Port,
			LCD_D5_GPIO_Port,
			LCD_D6_GPIO_Port,
			LCD_D7_GPIO_Port
	};

	lcd = Lcd_create(ports, pins, LCD_RS_GPIO_Port, LCD_RS_Pin, LCD_E_GPIO_Port, LCD_E_Pin, LCD_8_BIT_MODE);
	HAL_Delay(2);
	Lcd_clear(&lcd);
	HAL_Delay(2);
  LED_init();
  DMX_Init(&Univers, &huart4, "DMX1.txt", "DMX1Info.txt");

  Lcd_string(&lcd, "test");


//  test_LCD();
//  DMX_Rec_variable(&lcd);
  notify(10, 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(Button_pressed(BACK))
			HAL_GPIO_TogglePin(LED_SD_GPIO_Port, LED_SD_Pin);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
	*@brief Konvertiert zwei uint8_t in ein uint16_t
	*@param lowbyte: 	8-Bit Lowbyte
	*@param highbyte:	8-Bit Highbyte
	*@retval 16-Bit result
	*/
uint16_t convert8to16(uint8_t highbyte, uint8_t lowbyte)
{
	uint16_t result;
	result = highbyte << 8;
	result |= lowbyte;
	return result;
}

 /**
  *@brief Spielt Aufgenommene DMX-Sequenz von SD Karte ab
	*@retval None
  */
void DMX_Playback(DMX_TypeDef *hdmx)
{
	uint8_t temp[4], value = 0;
	uint16_t	rec_time, channel = 0, readpackets = 0;
	UINT bytesread = 0;

	//Enable MAX485 Write-Mode
	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_SET);

	//Getting intervall time from SD
	while(f_open(&hdmx->DMXInfoFile, hdmx->DMXInfoFile_name, FA_OPEN_ALWAYS | FA_READ));
	f_read(&hdmx->DMXInfoFile, &temp, 4, &bytesread);
	f_close(&hdmx->DMXInfoFile);
	hdmx->received_packets = convert8to16(temp[3], temp[2]);
	rec_time = convert8to16(temp[1], temp[0]);
	hdmx->intervall = (rec_time * 1000) / hdmx->received_packets; //Convert seconds to ms

	//Open dmx data file
	while(f_open(&hdmx->DMXFile, hdmx->DMXFile_name, FA_READ | FA_OPEN_ALWAYS) != FR_OK);

	//Initialize variables
	hdmx->sending = 1;
	htim14.Instance->CNT = 0;
	HAL_TIM_Base_Start_IT(&htim14);

	//endless playback
	while(1)
	{
		while(value != 1)
		{
			f_read(&hdmx->DMXFile, &value, 1, &bytesread);
			bytesread = 0;
			hdmx->TxBuffer[channel] = value;
			channel++;
		}
		readpackets ++;
		while(m_seconds_passed < hdmx->intervall);
		m_seconds_passed = 0;
		HAL_GPIO_TogglePin(LED_STATE_GPIO_Port, LED_STATE_Pin);
		DMX_Transmit(hdmx, 513);
		if(readpackets >= hdmx->received_packets)
		{
			f_lseek(&hdmx->DMXFile, 0);
			readpackets = 0;
		}
		channel = 0;
		value = 0;
	}
}

/**
	*@brief 	Nimmt eine DMX-Sequenz auf SD Karte auf
	*@param 	seconds: Aufnahmezeit in Sekunden
	*@param		*hdmx: Pointer zu DMX Objekt
	*@retval 	None
	*/
void Record_DMX_sec(uint16_t seconds, DMX_TypeDef *hdmx)
{
	UINT byteswritten = 0;

	//open file on SD
	while(f_open(&hdmx->DMXFile, hdmx->DMXFile_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK);
	while(f_open(&hdmx->DMXInfoFile, hdmx->DMXInfoFile_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK);

	//Enable MAX485 Read-Mode
	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_RESET);

	//Initialize variables
	hdmx->received_packets = 0;
	seconds_passed = 0;

	//Wait for dmx-brake
	DMX_Receive(hdmx, 514);  //Receive Data + 1 byte to trigger framing error

	//Timer start to measure recording time
	HAL_TIM_Base_Start_IT(&htim13);
	hdmx->recording = 1;
	while(seconds_passed <= seconds)
	{
		if(hdmx->RxComplete == 1)
		{
			hdmx->RxComplete = 0;
			f_write(&hdmx->DMXFile, Univers.RxBuffer, 513, &byteswritten);	//Auf SD-Karte schreiben
			f_write(&hdmx->DMXFile, &hdmx->newpacketcharacter, 1, &byteswritten);
			f_sync(&hdmx->DMXFile);
		}
	}
	hdmx->recording = 0;


	f_close(&hdmx->DMXFile);
	f_write(&hdmx->DMXInfoFile, &seconds, 2, &byteswritten);
	f_write(&hdmx->DMXInfoFile, &hdmx->received_packets, 2, &byteswritten);
	f_close(&hdmx->DMXInfoFile);

	notify(10, 500);
}


/**
	*@brief		Benutzerr�ckmeldung �ber die integrierte LED (blinken)
	*@param		cycles: Anzahl der LED Zustandswechsel
	*@param 	delay: Pause zwischen den Zustandwechseln
	*@retval	none
	*/
void notify(uint8_t cycles, uint16_t delay)
{
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
	for(int i = 0; i < cycles; i++)
	{
		HAL_GPIO_TogglePin(LED_STATE_GPIO_Port, LED_STATE_Pin);
		HAL_Delay(delay);
	}
}

/**
	*@brief 	Interruptfuntion f�r alle Timer. Wird aufgerufen wenn der Timer �berl�uft
	*@param 	htim: Timer handle
	*@retval 	none
	*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim10)
	{

	}
	else if(htim == &htim11)	//Idle Line send
	{
		DMX_send();
	}
	else if(htim == &htim13)	//Timer f�r Aufnahmefunktion - Taktung 1 s
	{
		seconds_passed++;
		return;
	}
	else if(htim == &htim7)		//Timer f�r Wiedergabefunktion - Taktung 1 ms
	{
		m_seconds_passed++;
		return;
	}
}

void LCD_init(Lcd_HandleTypeDef *lcd_object)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	htim1.Instance->CCR4 = 40;		//Standard Kontrastwert

	Lcd_PinType pins[8] = {
			LCD_D0_Pin,
			LCD_D1_Pin,
			LCD_D2_Pin,
			LCD_D3_Pin,
			LCD_D4_Pin,
			LCD_D5_Pin,
			LCD_D6_Pin,
			LCD_D7_Pin
	};

	Lcd_PortType ports[8] = {
			LCD_D0_GPIO_Port,
			LCD_D1_GPIO_Port,
			LCD_D2_GPIO_Port,
			LCD_D3_GPIO_Port,
			LCD_D4_GPIO_Port,
			LCD_D5_GPIO_Port,
			LCD_D6_GPIO_Port,
			LCD_D7_GPIO_Port
	};

	*lcd_object = Lcd_create(ports, pins, LCD_RS_GPIO_Port, LCD_RS_Pin, LCD_E_GPIO_Port, LCD_E_Pin, LCD_8_BIT_MODE);
	HAL_Delay(2);
	Lcd_clear(&lcd);
	HAL_Delay(2);
}

void LED_init()
{
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
	htim9.Instance->CCR2 = 65535;				//Maximale LED-Helligkeit
}

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
