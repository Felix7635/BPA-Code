#include "lcd.h"

void settings_lcd(Lcd_HandleTypeDef *lcd);
void settings_led(Lcd_HandleTypeDef *lcd);

uint8_t read_settings();
uint8_t write_settings();
