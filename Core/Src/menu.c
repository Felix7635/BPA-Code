#include "menu.h"
#include "Encoder.h"
#include "button.h"
#include "DMX.h"
#include "settings.h"

void play_menu(Lcd_HandleTypeDef *lcd);
void rec_menu(Lcd_HandleTypeDef *lcd);
void settings_menu(Lcd_HandleTypeDef *lcd);

char arrow = 0x7E;

  char menu_main[3][19] =
  {
		  "Aufnahme           ",
		  "Wiedergabe         ",
		  "Einstellungen      "
  };

  char menu_rec[4][19] =
  {
		  "Kontinuierlich     ",
		  "Trigger            ",
		  "Step               ",
		  "Endlos             "
  };

  char menu_play[19] =
  {
		  "Play               "
  };

  char menu_setting[5][19] =
  {
		  "LCD-Kontrast       ",
		  "LED-Hellgkeit      ",
		  "Trigger            ",
		  "NPC aendern        ",
		  "Aufnahme loeschen  "
  };

void main_menu(Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 0;
	uint8_t arraysize = 3;
	enc_position = 0;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 5);
	Lcd_string_length(lcd, "Hauptmenue", 10);

	for(int i = 0; i < 3; i++)
	{

		if(i + position < arraysize)
		{
			Lcd_cursor(lcd, i+1, 1);
			Lcd_string_length(lcd, menu_main[i + position], 19);
		}
	}

	position = 1;
	while(!Button_pressed(BACK))
	{
		if(enc_position > arraysize - 1)
			enc_position = arraysize - 1;
		else if(enc_position < 0)
			enc_position = 0;

		if(enc_position != position)
		{
			Lcd_cursor(lcd, position + 1, 0);
			Lcd_string_length(lcd, " ", 1);
			position = enc_position;
			Lcd_cursor(lcd, position + 1, 0);
			Lcd_string_length(lcd, &arrow, 1);
		}
		if(Button_pressed(ENTER))
		{
			switch (position) {
				case 0:
					rec_menu(lcd);	break;
				case 1:
					play_menu(lcd);	break;
				case 2:
					settings_menu(lcd);	break;
				default:
					break;
			}
			HAL_Delay(1000);
			return;
		}
	}

}

void play_menu(Lcd_HandleTypeDef *lcd)
{
	DMX_Playback(lcd);
}

void rec_menu(Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 1;
	uint8_t arraysize = 4;
	enc_position = 0;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 6);
	Lcd_string_length(lcd, "Aufnahme", 8);
	Lcd_cursor(lcd, 1, 0);
	Lcd_string_length(lcd, &arrow, 1);

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		else if(enc_position > arraysize - 1)
			enc_position = arraysize - 1;

		if(enc_position != position)
		{
			Lcd_cursor(lcd, 1, 0);
			Lcd_string_length(lcd, &arrow, 1);
			position = enc_position;
			for(int i = 0; i < 3; i++)
			{
				if(position + i < arraysize)
				{
					Lcd_cursor(lcd, i + 1, 1);
					Lcd_string_length(lcd, menu_rec[position + i], 19);
				}
				else
				{
					lcd_clear_row(lcd, i + 1);
				}
			}
		}

		if(Button_pressed(ENTER))
		{
			switch (position) {
				case 0:
					DMX_Rec_variable(lcd);	break;
				case 1:
					DMX_Rec_Trigger(lcd);	break;
				case 2:
					DMX_Rec_step(lcd);	break;
				case 3:
					DMX_Rec_endless(lcd);	break;
				default:
					break;
			}
			Lcd_clear(lcd);
			Lcd_cursor(lcd, 0, 6);
			Lcd_string_length(lcd, "Aufnahme", 8);
			Lcd_cursor(lcd, 1, 0);
			Lcd_string_length(lcd, &arrow, 1);
			enc_position = position;
			position++;
			HAL_Delay(1000);
		}
	}
}

void settings_menu(Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 1;
	uint8_t arraysize = 5;
	enc_position = 0;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 3);
	Lcd_string_length(lcd, "Einstellungen", 13);

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		else if(enc_position >= arraysize)
			enc_position = arraysize - 1;

		if(enc_position != position)
		{
			Lcd_cursor(lcd, 1, 0);
			Lcd_string_length(lcd, &arrow, 1);
			position = enc_position;
			for(int i = 0; i < 3; i++)
			{
				if(position + i < arraysize)
				{
					Lcd_cursor(lcd, i + 1, 1);
					Lcd_string_length(lcd, menu_setting[position + i], 19);
				}
				else
				{
					lcd_clear_row(lcd, i + 1);
				}
			}
		}
		if(Button_pressed(ENTER))
		{
			switch (position) {
				case 0:
					settings_lcd(lcd);	break;
				case 1:
					settings_led(lcd);	break;
				case 2:
					settings_trigger(lcd);	break;
				case 3:
					settings_npc(lcd);	break;
				case 4:
					delete_file(lcd);	break;
				default:
					break;
			}
			enc_position = position;
			position++;
			Lcd_clear(lcd);
			Lcd_cursor(lcd, 0, 3);
			Lcd_string_length(lcd, "Einstellungen", 13);
			HAL_Delay(1000);
		}
	}
}
