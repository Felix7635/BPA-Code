#include "LCD.h"
#include "tim.h"

static void _init_delay();
static void _send_data(char* data);
static void _sendpulse(uint8_t duration);
static void _us_delay(uint8_t duration);
static void _set_datapins(uint8_t data);
static void _write_command(uint8_t);

GPIO_TypeDef *_data_ports[8], *_control_port;
uint16_t _data_pins[8], _rs_pin, _e_pin;

uint8_t pulse_duration = 10;

void LCD_init(GPIO_TypeDef *rs_e_port, uint16_t rs_pin, uint16_t e_pin,
		GPIO_TypeDef *d0_port, uint16_t d0_pin,
		GPIO_TypeDef *d1_port, uint16_t d1_pin,
		GPIO_TypeDef *d2_port, uint16_t d2_pin,
		GPIO_TypeDef *d3_port, uint16_t d3_pin,
		GPIO_TypeDef *d4_port, uint16_t d4_pin,
		GPIO_TypeDef *d5_port, uint16_t d5_pin,
		GPIO_TypeDef *d6_port, uint16_t d6_pin,
		GPIO_TypeDef *d7_port, uint16_t d7_pin)
{
	_data_ports[0] = d0_port;
	_data_ports[1] = d1_port;
	_data_ports[2] = d2_port;
	_data_ports[3] = d3_port;
	_data_ports[4] = d4_port;
	_data_ports[5] = d5_port;
	_data_ports[6] = d6_port;
	_data_ports[7] = d7_port;

	_data_pins[0] = d0_pin;
	_data_pins[1] = d1_pin;
	_data_pins[2] = d2_pin;
	_data_pins[3] = d3_pin;
	_data_pins[4] = d4_pin;
	_data_pins[5] = d5_pin;
	_data_pins[6] = d6_pin;
	_data_pins[7] = d7_pin;

	_control_port = rs_e_port;
	_rs_pin = rs_pin;
	_e_pin = e_pin;

	HAL_GPIO_WritePin(rs_e_port, rs_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(rs_e_port, e_pin, GPIO_PIN_RESET);

	_write_command(FUNCTIONSET);
	_write_command(DISPLAY_SETTING);
}



static void _init_delay()
{

}

static void _send_data(char* data)
{
	uint8_t length = strlen(data);
	uint8_t bytes_send = 0;
	while(length--)
	{
		_set_datapins(data[bytes_send]);
		bytes_send++;
	}
	_sendpulse(pulse_duration);
}

static void _sendpulse(uint8_t duraion)
{
	HAL_GPIO_WritePin(_control_port, _e_pin, GPIO_PIN_SET);
	_us_delay(duraion);
	HAL_GPIO_WritePin(_control_port, _e_pin, GPIO_PIN_RESET);
}

static void _us_delay(uint8_t duration)
{
	htim14.Instance->SR &= ~(0x1);
	htim14.Instance->ARR = duration - 1;
	htim14.Instance->CR1 |= 1UL;
	while(!(htim14.Instance->SR & TIMER_UPDATE_FLAG));
}

static void _set_datapins(uint8_t data)
{
	uint8_t mask = 1;
	for(int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(_data_ports[i], _data_pins[i], data&0x1<<i ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

static void _write_command(uint8_t command)
{

}
