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



	
void DMX_Init(DMX_TypeDef* hdmx, UART_HandleTypeDef* huart, char *DMXFile_name, char *DMXInfoFile_name)
{
	memcpy(hdmx->DMXFile_name, DMXFile_name, 9);
	memcpy(hdmx->DMXInfoFile_name, DMXInfoFile_name, 13);
	hdmx->sending = 0;
	hdmx->recording = 0;
	hdmx->received_packets = 0;
	hdmx->newpacketcharacter = 1;
	hdmx->uart = huart;
	hdmx->RxComplete = 0;
	DMX_zeroes(hdmx->TxBuffer);		//TxBuffer mit 0 beschreiben	
	DMX_zeroes(hdmx->RxBuffer);		//RxBuffer mit 0 beschreiben
	hdmx->RxBuffer[513] = 1;			//Zeichen um neues Paket zu identifizieren (in der Datei)
	DMX_Transmit(hdmx, 513);			//Alle Kanäle mit 0 beschreiben
	HAL_GPIO_WritePin(DMX_Driver_Enable_GPIO_Port, DMX_Driver_Enable_Pin, GPIO_PIN_RESET);	//Treiber deaktivieren um Signal durchzuleiten
}

static void DMX_zeroes(uint8_t* array)
{
	for(int i = 0; i < 512; i++)
	{
		array[i] = 0;
	}
}

static void DMX_TxInit(DMX_TypeDef* hdmx)
{
	DMX_zeroes(hdmx->TxBuffer);
	HAL_GPIO_WritePin(DMX_Driver_Enable_GPIO_Port, DMX_Driver_Enable_Pin, GPIO_PIN_SET);
}

static void DMX_RxInit(DMX_TypeDef* hdmx)
{
	DMX_zeroes(hdmx->RxBuffer);
	HAL_GPIO_WritePin(DMX_Driver_Enable_GPIO_Port, DMX_Driver_Enable_Pin, GPIO_PIN_RESET);
}
void DMX_sendonechannel(DMX_TypeDef* hdmx, uint16_t channel, uint8_t value)	
{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		uint8_t temp = hdmx->TxBuffer[channel];
		hdmx->TxBuffer[channel] = value;
		DMX_Transmit(hdmx, 513);
		hdmx->TxBuffer[channel] = temp;
}
void DMX_Transmit(DMX_TypeDef* hdmx, uint16_t size)
{
	HAL_GPIO_WritePin(DMX_Driver_Enable_GPIO_Port, DMX_Driver_Enable_Pin, GPIO_PIN_SET);													//Treiber aktivieren												
	__HAL_UART_DISABLE(hdmx->uart);																																								//UART deaktivieren	
	DMX_TX_GPIO_Port->MODER = (DMX_TX_GPIO_Port->MODER &~GPIO_MODER_MODE6_Msk) | (1<< (GPIO_MODER_MODE6_Pos));		//Modus vom Ausgangspin ändern
	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);																								//Ausgang auf IDLE Pegel setzen (HIGH)
	htim11.Instance->CNT = 0;																																											//IDLE Timer zurücksetzen
	HAL_TIM_Base_Start_IT(&htim11);																																								//Timer starten		
}
void DMX_Receive(DMX_TypeDef* hdmx, uint16_t size)
{	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);			//Treiber in Empfangsmodus versetzen
	HAL_UART_Receive_IT(hdmx->uart, hdmx->RxBuffer, size);		//Bytes über UART Schnittstelle empfangen
}
