#include "main.h"


#define FUNCTIONSET 0x38
#define DISPLAY_SETTING 0xC //Display on, Cursor off

#define CLEAR_DISPLAY 0x1
#define RETURN_HOME 0x2
#define SET_CGRAM_ADDRESS 0x40
#define SETDDRAM_ADRESS 0x80
#define WRITE_DATA_TO_RAM 0x200

#define TIMER_UPDATE_FLAG 0x1

void LCD_init(GPIO_TypeDef *RS_Port, uint16_t RS_pin, uint16_t E_pin,
		GPIO_TypeDef *d0_port, uint16_t d0_pin,
		GPIO_TypeDef *d1_port, uint16_t d1_pin,
		GPIO_TypeDef *d2_port, uint16_t d2_pin,
		GPIO_TypeDef *d3_port, uint16_t d3_pin,
		GPIO_TypeDef *d4_port, uint16_t d4_pin,
		GPIO_TypeDef *d5_port, uint16_t d5_pin,
		GPIO_TypeDef *d6_port, uint16_t d6_pin,
		GPIO_TypeDef *d7_port, uint16_t d7_pin);

void LCD_print(char *string);
void LCD_clear();
