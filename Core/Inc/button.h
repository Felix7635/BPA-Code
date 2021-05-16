#include "main.h"

//extern voltile uint8_t presses;

#define BACK 	0x1
#define ENTER 	0x2
#define UP 		0x4
#define DOWN	0x8

uint8_t Button_pressed(uint8_t button);
void Button_reset();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
