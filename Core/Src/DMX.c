#include "DMX.h"
#include "Encoder.h"
#include "lcd.h"

DMX_TypeDef Univers = {0};

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
	hdmx->RxBuffer[513] = 1;		//Zeichen um neues Paket zu identifizieren (in der Datei)

	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_RESET);	//Treiber deaktivieren um Signal durchzuleiten

	CLEAR_BIT(htim11.Instance->CR1, TIM_CR1_UDIS);	//Timer Update interrupt aktivieren
	SET_BIT(htim11.Instance->DIER, TIM_DIER_UIE);	//Update Interrupt aktivieren
}

static void DMX_zeroes(uint8_t* array)
{
	for(int i = 0; i < 512; i++)
	{
		array[i] = 0;
	}
}

void DMX_sendonechannel(DMX_TypeDef* hdmx, uint16_t channel, uint8_t value)
{
//		uint8_t temp = hdmx->TxBuffer[channel];	//Ursprünglichen Wert zwischenspeichern
		hdmx->TxBuffer[channel] = value;
		DMX_Transmit(hdmx, 513);
}
void DMX_Transmit(DMX_TypeDef* hdmx, uint16_t size)
{
	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_SET);		//Treiber aktivieren
	DMX_set_TX_Pin_manual();											//Modus vom Ausgangspin �ndern
	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);		//Ausgang auf IDLE Pegel setzen (HIGH)
	SET_BIT(htim11.Instance->CR1, TIM_CR1_CEN);							//Timer starten
	while(!READ_BIT(htim11.Instance->SR, TIM_SR_UIF));					//Warten auf überlauf des Timers
	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);
	DMX_set_TX_Pin_auto();
	HAL_UART_Transmit_IT(Univers.uart, Univers.TxBuffer, 513);
}
void DMX_Receive(DMX_TypeDef* hdmx, uint16_t size)
{
	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_RESET);	//Treiber in Empfangsmodus versetzen
	HAL_UART_Receive_IT(hdmx->uart, hdmx->RxBuffer, size);				//Bytes über UART Schnittstelle empfangen
}

void DMX_set_TX_Pin_manual()
{
	DMX_TX_GPIO_Port->MODER = (DMX_TX_GPIO_Port->MODER &~GPIO_MODER_MODE0_Msk) | (1 << (GPIO_MODER_MODE0_Pos));
}

void DMX_set_TX_Pin_auto()
{

    DMX_TX_GPIO_Port->MODER = (DMX_TX_GPIO_Port->MODER &~GPIO_MODER_MODE0_Msk) | (2 << (GPIO_MODER_MODE0_Pos));
}

/**
	*@brief 	Interruptfunktion der UART Schnittstelle. Wird aufgerufen wenn der Empfangsvorgang beendet ist
	*@param 	huart: Pointer auf aufrufenden UART handle
	*@retval 	none
	*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //Aufruf wenn DMX Paket vollst�ndig empfangen wurde
{
	if(Univers.recording == 1)							//Überprüfen ob Aufnahme aktiv ist
	{
		Univers.RxComplete = 1;
		Univers.received_packets++;
		HAL_GPIO_TogglePin(LED_RX_GPIO_Port, LED_RX_Pin);
	}
	HAL_GPIO_TogglePin(LED_STATE_GPIO_Port, LED_STATE_Pin);
}

/**
	*@brief 	Interruptfunktion der UART Schnittstelle. Wird aufgerufen wenn ein Sendevorgang abgeschlossen ist
	*@param 	huart: Pointer auf aufrufenden UART handle
	*@retval 	none
	*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
//	if(Univers.sending == 1)
//	{
	DMX_set_TX_Pin_manual();
	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);	//Ausgangspin mit BRAKE Pegel beschreiben (LOW)
	HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);					//Rückmelung für den User
//	}
}

void DMX_Rec_variable(Lcd_HandleTypeDef *lcd)
{
	uint16_t time = 0;	//10ms Intervall

	Lcd_clear(lcd);
	HAL_Delay(2);
	Lcd_string(lcd, "Zeit waehlen");
	Lcd_cursor(lcd, 1, 0);
	Lcd_int(lcd, time);
	while(HAL_GPIO_ReadPin(BTN_BACK_GPIO_Port, BTN_BACK_Pin) == GPIO_PIN_SET)
	{
		Lcd_cursor(lcd, 1, 0);
		Lcd_int(lcd, enc_position);
		Lcd_string(lcd, "   ");
		HAL_Delay(100);
	}
	Lcd_clear(lcd);
}

void DMX_Rec_endless()
{

}

void DMX_Rec_step()
{

}
