#include "button.h"
#include "gpio.h"

#define DELAYTIME 200

volatile uint8_t presses = 0;
volatile uint32_t last_updated = 0;

uint8_t Button_pressed(uint8_t button)
{
	if(!button)
	{
		Button_reset();
		return 0;
	}
	uint8_t temp = 0;
	temp = presses & button;
	presses &= ~button;
	return temp;
}

void Button_reset()
{
	presses = 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if((last_updated + DELAYTIME) > HAL_GetTick())
		return;
	last_updated = HAL_GetTick();
	switch(GPIO_Pin)
	{
	case ENC_A_Pin:
		return;
	case BTN_BACK_Pin:
	{
		presses |= BACK;
		return;
	}
	case BTN_DOWN_Pin:
	{
		presses |= DOWN;
		return;
	}
	case BTN_UP_Pin:
	{
		presses |= UP;
		return;
	}
	case BTN_ENTER_Pin:
	{
		presses |= ENTER;
		return;
	}
	default:
		return;
	}
}
