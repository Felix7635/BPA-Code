#include "settings.h"
#include "tim.h"
#include "fatfs.h"
#include "DMX.h"
#include "button.h"
#include "Encoder.h"

#define SET_LED 0
#define SET_LCD 1

void change_pwm(Lcd_HandleTypeDef *lcd, uint8_t mode);

const char filename[] = "setting.set";
FIL setting_file;



void settings_lcd(Lcd_HandleTypeDef *lcd)
{
	change_pwm(lcd, SET_LCD);
}

void settings_led(Lcd_HandleTypeDef *lcd)
{
	change_pwm(lcd, SET_LED);
}

void change_pwm(Lcd_HandleTypeDef *lcd, uint8_t mode)
{
	volatile uint32_t *regi;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 3);

	if(mode == SET_LED)
	{
		regi = &htim9.Instance->CCR2;
		Lcd_string_length(lcd, "LED Heligkeit", 13);
		HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
	}
	else if(mode == SET_LCD)
	{
		regi = &htim1.Instance->CCR4;
		Lcd_string_length(lcd, "LCD Kontrast", 12);
	}

	uint32_t temp = *regi;
	enc_position = *regi;

	Lcd_cursor(lcd, 2, 8);
	Lcd_int(lcd, *regi);
	Lcd_string_length(lcd, "% ", 2);

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		else if(enc_position > 100)
			enc_position = 100;

		if(enc_position != *regi)
		{
			*regi = enc_position;
			Lcd_cursor(lcd, 2, 8);
			Lcd_int(lcd, *regi);
			Lcd_string_length(lcd, "% ", 2);
		}
		if(Button_pressed(ENTER))
		{
			HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
			write_settings();
			return;
		}
	}
	*regi = temp;
	HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_SET);
	return;
}

uint8_t read_settings()
{
	if(f_open(&setting_file, filename, FA_OPEN_ALWAYS | FA_READ) != FR_OK)
		return 0;

	UINT bytesread;
	uint32_t tmp;
	f_lseek(&setting_file, 0);
	f_read(&setting_file, &tmp, 4, &bytesread);
	htim9.Instance->CCR2 = tmp;
	f_read(&setting_file, &tmp, 4, &bytesread);
	htim1.Instance->CCR4 = tmp;
	f_read(&setting_file, &Univers.newpacketcharacter, 1, &bytesread);
	f_close(&setting_file);
	return 1;
}

uint8_t write_settings()
{
	if(f_open(&setting_file, filename, FA_WRITE | FA_OPEN_EXISTING) != FR_OK)
		if(f_open(&setting_file, filename, FA_WRITE | FA_CREATE_NEW) != FR_OK)
			return 0;

	UINT byteswritten;
	uint32_t tmp;
	f_lseek(&setting_file, 0);
	tmp = htim9.Instance->CCR2;
	f_write(&setting_file, &tmp, 4, &byteswritten);		//LED
	tmp = htim1.Instance->CCR4;
	f_write(&setting_file, &tmp, 4, &byteswritten);		//LCD
	f_write(&setting_file, &Univers.newpacketcharacter, 1, &byteswritten);	//New Packet Character
	f_close(&setting_file);
	return 1;
}
