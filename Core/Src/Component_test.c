#include "Component_test.h"
#include "main.h"



void test_LED()
{
	// Helligkeitwert zwischenspeichern
	// Helligkeit auf maximum zum testen

	HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);

	// Helligkeit auf 0% herunterfahren, danach zum ursprünglichen Wert zurück

	HAL_GPIO_WritePin(LED_SD_GPIO_Port, LED_SD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
	return;
}

//void test_SD()
//{
//	FATFS tmp_fatfs;
//	FIL tmp_file;
//	char tmp_path[4];
//	UINT tmp_bw;
//	f_mount(&tmp_fatfs, tmp_path, 0);
//	f_open(&tmp_file, "test_file.txt", FA_OPEN_ALWAYS | FA_CREATE_ALSWAYS);
//	f_write(&tmp_file, "test", 4, &tmp_bw);
//	f_sync(&tmp_file);
//	f_close(&tmp_file);
//	return;
//}
//
//void test_Encoder()
//{
//	if(HAL_GPIO_ReadPin(ENC_A_Port, ENC_A_pin) == GPIO_PIN_SET)
//	{
//		if(HAL_GPIO_ReadPin(ENC_B_Port, ENC_B_Pin) == GPIO_PIN_SET)
//		{
//			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
//		}
//		else
//		{
//
//			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
//		}
//	}
//}
//
//void test_LCD()
//{
//
//}
