#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "stdint.h"
#include "tim.h"
#include "fatfs.h"
#include "string.h"

static volatile uint16_t 	seconds_passed = 0,
							m_seconds_passed = 0;

//Dmx struct
typedef struct
{
	FIL DMXFile, DMXInfoFile;
	char DMXFile_name[9], DMXInfoFile_name[13];
	uint16_t intervall, received_packets;
	uint8_t RxComplete, newpacketcharacter, recording, sending;
	uint8_t TxBuffer[513]; 	//512 Bytes data + 1 byte Startcode
	uint8_t RxBuffer[514];	//512 bytes data + 1 byte startcode + 1 byte termination character for saving
	UART_HandleTypeDef *uart;
}DMX_TypeDef;

//Function Prototypes
void DMX_Init(DMX_TypeDef* hdmx, UART_HandleTypeDef* huart, char *DMXFile_name, char *DMXInfoFile_name);
static void DMX_TxInit();
static void DMX_RxInit();
static void DMX_zeroes(uint8_t* array);
void DMX_sendonechannel(DMX_TypeDef* hdmx, uint16_t channel, uint8_t value);
void DMX_Transmit(DMX_TypeDef* hdmx, uint16_t size);
void DMX_Receive(DMX_TypeDef* hdmx, uint16_t size);
static void delay(uint16_t delaytime, uint8_t mode);
static void delay_MAB(uint16_t delaytime);
void delayms(uint16_t delaytime);
HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);




