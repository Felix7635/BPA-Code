#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "stdint.h"
#include "tim.h"
#include "fatfs.h"
#include "string.h"
#include "lcd.h"
#include "bsp_driver_sd.h"

extern volatile uint8_t time_passed[4];
extern volatile uint32_t seconds_passed;
extern volatile uint32_t m_seconds_passed;
extern const uint8_t hour;
extern const uint8_t minute;
extern const uint8_t second;
extern const uint8_t m_second;

#define MAX_FN_LENGTH 12


//Dmx struct
typedef struct
{
	TCHAR DMXFile_name[MAX_FN_LENGTH];
	TCHAR DMXInfoFile_name[MAX_FN_LENGTH];
	FATFS filesystem;
	FIL DMXFile;
	FRESULT fres;
	char path[10];
	uint32_t received_packets, rec_time;
	volatile uint8_t RxComplete, recording, sending;
	uint8_t newpacketcharacter, exchangecharacter;
	uint8_t TxBuffer[513]; 	//512 Bytes data + 1 byte Startcode
	uint8_t RxBuffer[514];	//512 bytes data + 1 byte startcode + 1 byte termination character for saving
	UART_HandleTypeDef *uart;
	uint8_t triggerchhannel, triggervalue;
}DMX_TypeDef;

extern DMX_TypeDef Univers;

//Function Prototypes
void DMX_Init(DMX_TypeDef* hdmx, UART_HandleTypeDef* huart);
void DMX_zeroes(uint8_t* array);
void DMX_sendonechannel(DMX_TypeDef* hdmx, uint16_t channel, uint8_t value);
void DMX_Transmit(DMX_TypeDef* hdmx, uint16_t size);
void DMX_Receive(DMX_TypeDef* hdmx, uint16_t size);
void delayms(uint16_t delaytime);
void DMX_set_TX_Pin_manual();
void DMX_set_TX_Pin_auto();

void DMX_Rec_variable(Lcd_HandleTypeDef * lcd);
void DMX_Rec_endless(Lcd_HandleTypeDef *lcd);
void DMX_Rec_step(Lcd_HandleTypeDef *lcd);
void DMX_Rec_Trigger(Lcd_HandleTypeDef *lcd);
void DMX_Playback(Lcd_HandleTypeDef *lcd);
uint8_t delete_file(Lcd_HandleTypeDef * lcd);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void BSP_SD_ReadCpltCallback(void);
void BSP_SD_WriteCpltCallback(void);
