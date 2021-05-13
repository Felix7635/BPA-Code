#include "Encoder.h"

// 0 -- Timeout
// 1 -- Up
// 2 -- Down


//Timeout implementieren
uint8_t enc_move(uint16_t timeout)
{
	while(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET)
	{
		if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_RESET)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
}

