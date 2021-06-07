#include "settings.h"
#include "tim.h"
#include "fatfs.h"
#include "DMX.h"
#include "button.h"
#include "Encoder.h"

const char filename[] = "setting.set";
FIL setting_file;



void settings_lcd(Lcd_HandleTypeDef *lcd)
{
	uint16_t temp = htim1.Instance->CCR4;
	enc_position = htim1.Instance->CCR4;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 3);
	Lcd_string_length(lcd, "LCD Kontrast", 12);
	Lcd_cursor(lcd, 2, 8);
	Lcd_int(lcd, htim1.Instance->CCR4);
	Lcd_string_length(lcd, "% ", 2);

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		else if(enc_position > 100)
			enc_position = 100;

		if(enc_position != htim1.Instance->CCR4)
		{
			htim1.Instance->CCR4 = enc_position;
			Lcd_cursor(lcd, 2, 8);
			Lcd_int(lcd, htim1.Instance->CCR4);
			Lcd_string_length(lcd, "% ", 2);
		}
		if(Button_pressed(ENTER))
		{
			write_settings();
			return;
		}
	}
	htim1.Instance->CCR4 = temp;
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
