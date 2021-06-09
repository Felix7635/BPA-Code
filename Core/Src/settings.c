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

void settings_npc(Lcd_HandleTypeDef *lcd)
{
	Lcd_clear(lcd);
	enc_position = Univers.newpacketcharacter;
	uint8_t temp = Univers.newpacketcharacter, tempx = Univers.exchangecharacter;
	uint8_t position = 0;
	char arrow = 0x5E;

	Lcd_cursor(lcd, 0, 4);
	Lcd_string_length(lcd, "NPC aendern", 11);
	Lcd_cursor(lcd, 3, 0);
	Lcd_string_length(lcd, &arrow, 1);
	temp--;

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		else if(enc_position > 255)
			enc_position = 255;

		if(enc_position != (position == 0 ? temp : tempx))
		{
			if(position == 0)
				temp = enc_position;
			else if(position == 1)
				tempx = enc_position;

			Lcd_cursor(lcd, 2, 0);
			Lcd_int(lcd, temp);
			Lcd_string_length(lcd, "   ", 2);
			Lcd_cursor(lcd, 2, 4);
			Lcd_string_length(lcd, "ersetzen mit ", 13);
			Lcd_int(lcd, tempx);
			Lcd_string_length(lcd, "  ", 2);
		}

		if(Button_pressed(UP) && position == 1)
		{
			position = 0;
			enc_position = temp;
			Lcd_cursor(lcd, 3, 0);
			Lcd_string_length(lcd, &arrow, 1);
			Lcd_cursor(lcd, 3, 17);
			Lcd_string_length(lcd, " ", 1);
		}
		else if(Button_pressed(DOWN) && position == 0)
		{
			position++;
			enc_position = tempx;
			Lcd_cursor(lcd, 3, 17);
			Lcd_string_length(lcd, &arrow, 1);
			Lcd_cursor(lcd, 3, 0);
			Lcd_string_length(lcd, " ", 1);
		}

		if(Button_pressed(ENTER))
		{
			if(temp != tempx)
			{
				Univers.newpacketcharacter = temp;
				Univers.exchangecharacter = tempx;
				write_settings();
				Lcd_clear(lcd);
				Lcd_cursor(lcd, 1, 3);
				Lcd_string_length(lcd, "NPC geaendert", 13);
				while(!Button_pressed(ENTER));
				return;
			}
			else
			{
				Lcd_clear(lcd);
				Lcd_string_length(lcd, "Fehler", 6);
				while(!Button_pressed(BACK));
			}
		}
	}
	return;
}

void settings_trigger(Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 0;
	uint16_t tempc = Univers.triggerchhannel;
	uint8_t tempv = Univers.triggervalue;
	char arrow = 0x7E;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 2);
	Lcd_string_length(lcd, "Trigger aendern", 15);
	Lcd_cursor(lcd, 2, 0);
	Lcd_string_length(lcd, &arrow, 1);

	enc_position = Univers.triggerchhannel;
	tempc++;

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0 && position == 0)
			enc_position = 0;
		else if(enc_position < 1 && position == 1)
			enc_position = 1;
		else if(enc_position > 512 && position == 0)
			enc_position = 512;
		else if(enc_position > 255 && position == 1)
			enc_position = 255;

		if(enc_position != (position == 0 ? tempc : tempv))
		{
			if(position == 0)
				tempc = enc_position;
			else
				tempv = enc_position;
			Lcd_cursor(lcd, 2, 1);
			Lcd_string_length(lcd, "Kanal: ", 7);
			Lcd_int(lcd, tempc);
			Lcd_cursor(lcd, 3, 1);
			Lcd_string_length(lcd, "Wert: ", 6);
			Lcd_int(lcd, tempv);
		}

		if(Button_pressed(UP) && position == 1)
		{
			Lcd_cursor(lcd, 2, 0);
			Lcd_string_length(lcd, &arrow, 1);
			Lcd_cursor(lcd, 3, 0);
			Lcd_string_length(lcd, " ", 1);
			enc_position = tempc;
			position--;
		}
		else if(Button_pressed(DOWN) && position == 0)
		{
			Lcd_cursor(lcd, 3, 0);
			Lcd_string_length(lcd, &arrow, 1);
			Lcd_cursor(lcd, 2, 0);
			Lcd_string_length(lcd, " ", 1);
			enc_position = tempv;
			position++;
		}

		if(Button_pressed(ENTER))
		{
			Univers.triggerchhannel = tempc;
			Univers.triggervalue = tempv;
			write_settings();
			Lcd_clear(lcd);
			Lcd_cursor(lcd, 1, 4);
			Lcd_string_length(lcd, "Erfolgreich!", 12);
			while(!Button_pressed(ENTER));
			return;
		}

	}
	return;
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
	f_read(&setting_file, Univers.exchangecharacter, 1, &bytesread);
	f_read(&setting_file, &Univers.triggerchhannel, 2, &bytesread);
	f_read(&setting_file, &Univers.triggervalue, 1, &bytesread);
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
	f_write(&setting_file, Univers.exchangecharacter, 1, &byteswritten);	//Exchange Character
	f_write(&setting_file, &Univers.triggerchhannel, 2, &byteswritten);
	f_write(&setting_file, Univers.triggervalue, 1, &byteswritten);
	f_close(&setting_file);
	return 1;
}
