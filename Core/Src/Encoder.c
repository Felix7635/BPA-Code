#include "Encoder.h"

// 0 -- Timeout
// 1 -- Up
// 2 -- Down

//#define debug
#define delaytime 100

volatile uint16_t enc_position = 0;
volatile uint32_t last_update = 0;


//Timeout implementieren
uint8_t enc_move(uint32_t timeout)
{
	uint32_t time = HAL_GetTick() + timeout;
	while(HAL_GetTick() < time)
	{
		if(HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET)
		{
			if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_RESET)
			{
				#ifdef debug
					HAL_GPIO_TogglePin(LED_RX_GPIO_Port, LED_RX_Pin);
					HAL_Delay(100);
					HAL_GPIO_TogglePin(LED_RX_GPIO_Port, LED_RX_Pin);
				#endif
				return ENC_DOWN;
			}
			else if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_SET && HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_RESET)
			{
				#ifdef debug
					HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);
					HAL_Delay(100);
					HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);
				#endif
				return ENC_UP;
			}
		}
	}
	return ENC_TIMEOUT;
}

void enc_Interrupt()
{
	if((last_update + delaytime) > HAL_GetTick())
		return;
	last_update = HAL_GetTick();
	if(HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin))
	{
		if(enc_position > 0)
			enc_position--;
	}
	else
	{
		enc_position++;
	}
	return;
}
