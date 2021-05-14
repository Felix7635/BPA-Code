#include "Component_test.h"
#include "main.h"
#include "lcd.h"
#include "fatfs.h"
#include "usart.h"



void test_LED()
{
	// Helligkeitwert zwischenspeichern
	// Helligkeit auf maximum zum testen
	uint16_t pins[4] = {LED_SD_Pin, LED_STATE_Pin, LED_RX_Pin, LED_TX_Pin};

	while(1)
	{
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				HAL_GPIO_WritePin(LED_SD_GPIO_Port, pins[i], GPIO_PIN_SET);
				HAL_Delay(250);
				HAL_GPIO_WritePin(LED_SD_GPIO_Port, pins[i], GPIO_PIN_RESET);
				HAL_Delay(250);
			}
		}
	}

	return;
}

void test_SD()
{
	FATFS tmp_fatfs;
	FIL tmp_file;
	char tmp_path[4];
	UINT tmp_bw;
	f_mount(&tmp_fatfs, tmp_path, 0);
	f_open(&tmp_file, "test_file.txt", FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&tmp_file, "test", 4, &tmp_bw);
	f_sync(&tmp_file);
	f_close(&tmp_file);
	return;
}

void test_Encoder()
{
	while(1)
	{
		if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_RESET)
		{
			if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_RESET)
			{
				HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
			}
			else
			{

				HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
			}
			HAL_Delay(500);
			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
		}
	}
}

void test_LCD()
{
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

	Lcd_HandleTypeDef lcd = Lcd_create(ports, pins, LCD_RS_GPIO_Port, LCD_RS_Pin, LCD_E_GPIO_Port, LCD_E_Pin, LCD_8_BIT_MODE);


	HAL_Delay(1);
	Lcd_clear(&lcd);
	HAL_Delay(1);
	Lcd_string(&lcd, "first row");
	Lcd_cursor(&lcd, 1, 0);
	Lcd_string(&lcd, "second row");
	Lcd_cursor(&lcd, 2, 0);
	Lcd_string(&lcd, "third row");
	Lcd_cursor(&lcd, 3, 0);
	Lcd_string(&lcd, "fourth row");
}

//void test_uart()
//{
////	char test = "test";
//
//	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_SET);
//	while(1)
//	{
//		HAL_UART_Transmit(&huart4, "test", 4, 100);
//		HAL_Delay(1000);
//	}
//}
