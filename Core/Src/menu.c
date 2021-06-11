#include "menu.h"
#include "Encoder.h"
#include "button.h"

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

  char menu_rec[3][19] =
  {
		  "Kontinuierlich     ",
		  "Trigger            ",
		  "Step               "
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
		  "NPC ändern         ",
		  "Aufnahme löschen   "
  };

void main_menu(Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 0;
	uint8_t arraysize = 3;
	char arrow = 0x7E;
	enc_position = 0;

	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 5);
	Lcd_string_length(lcd, "Hauptmenue", 9);

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
				{
					rec_menu(lcd);
					break;
				}
				case 1:
				{
					play_menu(lcd);
					break;
				}
				case 2:
				{
					settings_menu(lcd);
					break;
				}
				default:
					break;
			}
		}
	}

}

void play_menu(Lcd_HandleTypeDef *lcd)
{

}

void rec_menu(Lcd_HandleTypeDef *lcd)
{

}

void settings_menu(Lcd_HandleTypeDef *lcd)
{

}
