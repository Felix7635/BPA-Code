#include "DMX.h"
#include "Encoder.h"
#include "button.h"


DMX_TypeDef Univers = {0};

uint8_t time[4] = {0, 0, 0, 0};
volatile uint8_t time_passed[4] = {0, 0, 0, 0};
volatile uint32_t seconds_passed = 0;
volatile uint32_t m_seconds_passed = 0;
const uint8_t hour = 0;
const uint8_t minute = 1;
const uint8_t second = 2;
const uint8_t m_second = 3;

uint8_t DMX_setRecTime(DMX_TypeDef *hdmx, Lcd_HandleTypeDef *lcd);
uint8_t DMX_setFilename(DMX_TypeDef *hdmx, Lcd_HandleTypeDef *lcd);
uint8_t write_infofile(DMX_TypeDef *hdmx);
uint8_t read_infofile(DMX_TypeDef *hdmx);
uint8_t select_file(Lcd_HandleTypeDef *lcd);
void show_progressbar(Lcd_HandleTypeDef *lcd, uint8_t lcd_row, uint32_t received, uint32_t read);
void check_newpacketcharacter();
UINT save_packet();

void DMX_Init(DMX_TypeDef* hdmx, UART_HandleTypeDef* huart)
{
//	memcpy(hdmx->DMXFile_name, DMXFile_name, 13);
//	memcpy(hdmx->DMXInfoFile_name, DMXInfoFile_name, 13);
	hdmx->sending = 0;
	hdmx->recording = 0;
	hdmx->received_packets = 0;
	hdmx->newpacketcharacter = 1;
	hdmx->exchangecharacter = 0;
	hdmx->uart = huart;
	hdmx->RxComplete = 0;
	hdmx->triggerchhannel = 1;
	hdmx->triggervalue = 1;
	DMX_zeroes(hdmx->TxBuffer);		//TxBuffer mit 0 beschreiben
	DMX_zeroes(hdmx->RxBuffer);		//RxBuffer mit 0 beschreiben
	hdmx->RxBuffer[513] = 1;		//Zeichen um neues Paket zu identifizieren (in der Datei)
	memcpy(Univers.DMXFile_name, "        .dmx", MAX_FN_LENGTH);
	memcpy(Univers.DMXInfoFile_name, "        .nfo", MAX_FN_LENGTH);

	HAL_GPIO_WritePin(DMX_DE_GPIO_Port, DMX_DE_Pin, GPIO_PIN_RESET);	//Treiber deaktivieren um Signal durchzuleiten

	CLEAR_BIT(htim11.Instance->CR1, TIM_CR1_UDIS);	//Timer Update interrupt aktivieren
	SET_BIT(htim11.Instance->DIER, TIM_DIER_UIE);	//Update Interrupt aktivieren
}

void DMX_zeroes(uint8_t* array)
{
	for(int i = 0; i < 513; i++)
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
	htim11.Instance->CNT = 0;
	CLEAR_BIT(htim11.Instance->SR, TIM_SR_UIF);						//Update Interrupt Flag zurücksetzen
	SET_BIT(htim11.Instance->CR1, TIM_CR1_CEN);							//Timer starten
	while(!READ_BIT(htim11.Instance->SR, TIM_SR_UIF));					//Warten auf überlauf des Timers
	CLEAR_BIT(htim11.Instance->SR, TIM_SR_UIF);						//Update Interrupt Flag zurücksetzen
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
		return;
	}
//	else if(Univers.recording == 0)
//	{
	HAL_GPIO_TogglePin(LED_STATE_GPIO_Port, LED_STATE_Pin);
//	}
}

/**
	*@brief 	Interruptfunktion der UART Schnittstelle. Wird aufgerufen wenn ein Sendevorgang abgeschlossen ist
	*@param 	huart: Pointer auf aufrufenden UART handle
	*@retval 	none
	*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(Univers.sending == 1)
	{
	DMX_set_TX_Pin_manual();
	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);	//Ausgangspin mit BRAKE Pegel beschreiben (LOW)
	HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);					//Rückmelung für den User
	}
}

void DMX_Rec_variable(Lcd_HandleTypeDef *lcd)
{
	uint8_t exit = 0;
	while(!exit)
	{
		if(DMX_setRecTime(&Univers, lcd))
		{
			if(DMX_setFilename(&Univers, lcd))
			{
				memset(&Univers.DMXFile, 0, sizeof(FIL));
				if((Univers.fres = f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_WRITE | FA_CREATE_NEW)) == FR_OK)
				{
					Univers.recording = 1;
					DMX_Receive(&Univers, 514);
					while(&Univers.RxComplete == 0);
					while((Univers.recording = 0) != 0);
					Univers.received_packets = 0;

					Lcd_clear(lcd);
					Lcd_cursor(lcd, 0, 7);
					Lcd_string(lcd, "start?");
					HAL_Delay(500);
					while(!Button_pressed(ENTER))
						if(Button_pressed(BACK))
							return;

					HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
					Lcd_clear(lcd);
					Lcd_cursor(lcd, 0, 0);
					Lcd_string(lcd, "Aufnahme...");

					htim13.Instance->CNT = 0;
					m_seconds_passed = 0;
					HAL_TIM_Base_Start_IT(&htim13);
					Univers.recording = 1;
					Univers.RxComplete = 0;
					while(m_seconds_passed < Univers.rec_time) //millisecunden Timer starten (noch implementieren
					{
						if(Univers.RxComplete)
						{
							Lcd_cursor(lcd, 1, 0);
							Lcd_int(lcd, Univers.received_packets);
							Lcd_cursor(lcd, 2, 0);
							Lcd_int(lcd, ((Univers.rec_time - m_seconds_passed) / 100));
							Lcd_string_length(lcd, " ", 1);
							Univers.RxComplete = 0;
							save_packet();
						}
					}
					Univers.recording = 0;
					f_close(&Univers.DMXFile);
					HAL_UART_AbortReceive_IT(Univers.uart);
					HAL_TIM_Base_Stop_IT(&htim13);

					if(write_infofile(&Univers))
					{
						HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);

						char n[10];
						memcpy(n, Univers.DMXFile_name, 10);
						Lcd_clear(lcd);
						Lcd_cursor(lcd, 0, 0);
						Lcd_string(lcd, "Aufnahme erfolgreich");
						HAL_Delay(1);
						Lcd_cursor(lcd, 1, 0);
						Lcd_string(lcd, n);
						HAL_Delay(1);
						Lcd_cursor(lcd, 2, 0);
						Lcd_int(lcd, time[hour]);
						Lcd_string(lcd, "h");
						Lcd_int(lcd, time[minute]);
						Lcd_string(lcd, "m");
						Lcd_int(lcd, time[second]);
						Lcd_string(lcd, "s");
						Lcd_int(lcd, time[m_second]);
						Lcd_string(lcd, "ms");
						Lcd_cursor(lcd, 3, 0);
						HAL_Delay(1);
						Lcd_int(lcd, Univers.received_packets);
						HAL_Delay(1);
						Lcd_cursor(lcd, 3, 9);
						HAL_Delay(1);
						Lcd_string(lcd, "Datenpakete");
						while(!Button_pressed(ENTER));
					}
					else	//Info-Datei konnte nicht erstellt werden
					{
						Lcd_clear(lcd);
						Lcd_cursor(lcd, 0, 0);
						Lcd_string(lcd, "Error (info-file)");
						while(!Button_pressed(ENTER));
					}
				}
				else
				{
					Lcd_clear(lcd);
					Lcd_cursor(lcd, 0, 0);
					if(Univers.fres == FR_INVALID_NAME)
						Lcd_string(lcd, "Invalid Filename!");
					else if(Univers.fres == FR_NOT_READY)
						Lcd_string(lcd, "SD not ready!");
					else if(Univers.fres == FR_DISK_ERR)
						Lcd_string(lcd, "Disk error!");
					else if(Univers.fres == FR_EXIST)
					{
						Lcd_string(lcd, "Aufnahme exitiert");
						Lcd_cursor(lcd, 1, 0);
						Lcd_string(lcd, "bereits");
					}
					else
						Lcd_string(lcd, "Unknown error");
					while(!Button_pressed(ENTER));
				}
			}
			else			//Fehler beim erstellen einer neuen Aufnamedatei
			{
				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				if(Univers.fres == FR_INVALID_NAME)
					Lcd_string(lcd, "Invalid Filename!");
				else if(Univers.fres == FR_NOT_READY)
					Lcd_string(lcd, "SD not ready!");
				else if(Univers.fres == FR_DISK_ERR)
					Lcd_string(lcd, "Disk error!");
				else
					Lcd_string(lcd, "Unknown error");
				while(!Button_pressed(ENTER));
			}
		}
		else
		{
			 exit = 1;
		}
	}
}

void DMX_Rec_endless(Lcd_HandleTypeDef *lcd)
{
	if(DMX_setFilename(&Univers, lcd))
	{
		memset(&Univers.DMXFile, 0, sizeof(FIL));
		if(f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_WRITE | FA_OPEN_EXISTING) != FR_OK)
		{
			if((Univers.fres = f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_WRITE | FA_CREATE_ALWAYS)) == FR_OK)
			{
				Univers.recording = 1;
				DMX_Receive(&Univers, 514);
				while(&Univers.RxComplete == 0);
				while((Univers.recording = 0) != 0);
				Univers.received_packets = 0;

				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 7);
				Lcd_string(lcd, "start?");
				HAL_Delay(500);
				while(!Button_pressed(ENTER));

				HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				Lcd_string(lcd, "Aufnahme...");

				htim13.Instance->CNT = 0;
				m_seconds_passed = 0;
				HAL_TIM_Base_Start_IT(&htim13);
				Univers.recording = 1;
				Univers.RxComplete = 0;
				while(!Button_pressed(BACK)) //millisecunden Timer starten (noch implementieren
				{
					if(Univers.RxComplete)
					{
						Lcd_cursor(lcd, 1, 0);
						Lcd_int(lcd, Univers.received_packets);
						Lcd_cursor(lcd, 2, 0);
						Lcd_int(lcd, (m_seconds_passed / 100));
						Lcd_string(lcd, ".");
						Lcd_int(lcd, (m_seconds_passed%100));
						Univers.RxComplete = 0;
						save_packet();
					}
				}
				Univers.recording = 0;
				f_close(&Univers.DMXFile);
				HAL_UART_AbortReceive_IT(Univers.uart);
				HAL_TIM_Base_Stop_IT(&htim13);

				if(write_infofile(&Univers))
				{
					HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
					Lcd_clear(lcd);
					Lcd_cursor(lcd, 0, 0);
					Lcd_string(lcd, "Aufnahme erfolgreich");
					HAL_Delay(1);
					Lcd_cursor(lcd, 1, 0);
					Lcd_string_length(lcd, Univers.DMXFile_name, MAX_FN_LENGTH);
					HAL_Delay(1);
					Lcd_cursor(lcd, 2, 0);
					Lcd_int(lcd, m_seconds_passed/100);
					Lcd_string(lcd, ".");
					Lcd_int(lcd, m_seconds_passed%100);
					Lcd_string(lcd, "s");
					Lcd_cursor(lcd, 3, 0);
					HAL_Delay(1);
					Lcd_int(lcd, Univers.received_packets);
					HAL_Delay(1);
					Lcd_cursor(lcd, 3, 8);
					HAL_Delay(1);
					Lcd_string_length(lcd, "Datenpakete", 11);
					while(!Button_pressed(ENTER));
				}
				else	//Info-Datei konnte nicht erstellt werden
				{
					Lcd_clear(lcd);
					Lcd_cursor(lcd, 0, 0);
					Lcd_string(lcd, "Error (info-file)");
					while(!Button_pressed(ENTER));
				}
			}
			else
			{
				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				if(Univers.fres == FR_INVALID_NAME)
					Lcd_string(lcd, "Invalid Filename!");
				else if(Univers.fres == FR_NOT_READY)
					Lcd_string(lcd, "SD not ready!");
				else if(Univers.fres == FR_DISK_ERR)
					Lcd_string(lcd, "Disk error!");
				else
					Lcd_string(lcd, "Unknown error");
				while(!Button_pressed(ENTER));
			}
		}
		else			//Fehler beim erstellen einer neuen Aufnamedatei
		{
			Lcd_clear(lcd);
			Lcd_cursor(lcd, 0, 0);
			if(Univers.fres == FR_INVALID_NAME)
				Lcd_string(lcd, "Invalid Filename!");
			else if(Univers.fres == FR_NOT_READY)
				Lcd_string(lcd, "SD not ready!");
			else if(Univers.fres == FR_DISK_ERR)
				Lcd_string(lcd, "Disk error!");
			else
				Lcd_string(lcd, "Unknown error");
			while(!Button_pressed(ENTER));
		}
	}
	else			//Wenn Datei bereits exitiert
	{
		Lcd_clear(lcd);
		Lcd_cursor(lcd, 0, 0);
		Lcd_string(lcd, "Aufnahme exitiert");
		Lcd_cursor(lcd, 1, 0);
		Lcd_string(lcd, "bereits");
		while(!Button_pressed(ENTER));
	}
}

void DMX_Rec_step(Lcd_HandleTypeDef *lcd)
{
	uint8_t exit = 0;
	uint16_t step = 0, previous_step = 0, max_step = 0;
	while(!exit)
	{
		if(DMX_setFilename(&Univers, lcd))
		{
			memset(&Univers.DMXFile, 0, sizeof(FIL));
			Lcd_clear(lcd);
			Lcd_cursor(lcd, 0, 0);
			if((Univers.fres = f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_WRITE | FA_CREATE_ALWAYS)) == FR_OK)
			{
				Lcd_string(lcd, "Step-Aufnahme");

				Univers.recording = 1;
				DMX_Receive(&Univers, 514);
				while(!Univers.RxComplete);
				Univers.RxComplete = 0;

				Lcd_cursor(lcd, 1, 0);
				Lcd_int(lcd, step);
				Lcd_string(lcd, "/");
				Lcd_int(lcd, max_step);

				while(!Button_pressed(BACK))
				{

					if(Button_pressed(DOWN))
					{
						while(!Univers.RxComplete);

						save_packet();

						previous_step = step;
						step++;
						if(step > max_step)
							max_step = step;

						Lcd_cursor(lcd, 1, 0);
						Lcd_int(lcd, step);
						Lcd_string(lcd, "/");
						Lcd_int(lcd, max_step);
					}
					else if(Button_pressed(UP))
					{
						Lcd_cursor(lcd, 1, 0);
						Lcd_int(lcd, step);
						previous_step = step;
						step--;
						f_lseek(&Univers.DMXFile, 519 * step);
					}
					else if(Button_pressed(ENTER))
					{
						HAL_UART_Abort_IT(Univers.uart);
						HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
						Univers.received_packets = max_step;
						Univers.rec_time = 0;
						f_close(&Univers.DMXFile);
						if(write_infofile(&Univers))
						{
							Lcd_clear(lcd);
							Lcd_cursor(lcd, 0, 0);
							Lcd_string_length(lcd, "Aufnahme erfolgreich!", 20);
							while(!Button_pressed(ENTER));
							exit = 1;
						}
					}
				}
			}
			else
			{
				Lcd_string(lcd, "error");
				while(!Button_pressed(ENTER));
			}
		}
		else
			exit = 1;
	}
}

void DMX_Rec_Trigger(Lcd_HandleTypeDef *lcd)
{
	uint8_t exit = 0;
	while(!exit)
	{
		if(DMX_setFilename(&Univers, lcd))
		{
			memset(&Univers.DMXFile, 0, sizeof(FIL));
			if(f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_WRITE | FA_OPEN_ALWAYS) == FR_OK)
			{
				Univers.received_packets = 0;
				Univers.rec_time = 0;

				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				Lcd_string_length(lcd, "Triggerstart", 12);
				Lcd_cursor(lcd, 1, 0);

				htim13.Instance->CNT = 0;
				m_seconds_passed = 0;

				Univers.recording = 1;
				DMX_Receive(&Univers, 514);
				while(!Univers.RxComplete)
					if(Button_pressed(BACK))
						return;
				HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
				Univers.recording = 0;
				Lcd_string_length(lcd, "Warte auf Trigger...", 20);
				while((Univers.RxBuffer[Univers.triggerchhannel] < Univers.triggervalue));
				Univers.received_packets = 0;
				Univers.RxComplete = 0;
				Univers.recording = 1;
				HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);

				HAL_TIM_Base_Start_IT(&htim13);
				Lcd_clear(lcd);
				Lcd_string_length(lcd, "Aufnahme...", 11);
				while((Univers.RxBuffer[Univers.triggerchhannel] > Univers.triggervalue))
				{
					if(Univers.RxComplete == 1)
					{
						Univers.RxComplete = 0;
						save_packet();
						HAL_GPIO_TogglePin(LED_TX_GPIO_Port, LED_TX_Pin);
					}
					Lcd_cursor(lcd, 2, 0);
					Lcd_int(lcd, m_seconds_passed/100);
					Lcd_string_length(lcd, ",", 1);
					Lcd_int(lcd, m_seconds_passed % 100);
					Lcd_string_length(lcd, "s", 1);
				}
				HAL_TIM_Base_Stop_IT(&htim13);
				HAL_UART_AbortReceive_IT(Univers.uart);
				HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
				Univers.rec_time = m_seconds_passed;
				Univers.recording = 0;
				f_close(&Univers.DMXFile);
				write_infofile(&Univers);

				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				Lcd_string_length(lcd, "Aufnahme erfolgreich", 20);
				Lcd_cursor(lcd, 1, 0);
				Lcd_int(lcd, Univers.received_packets);
				Lcd_string_length(lcd, " Datenpakete", 12);
				Lcd_cursor(lcd, 2, 0);
				Lcd_int(lcd, Univers.rec_time / 100);
				Lcd_string_length(lcd, ",", 1);
				Lcd_int(lcd, Univers.rec_time % 100);
				Lcd_string_length(lcd, "s", 1);

				exit = 1;
				while(!Button_pressed(ENTER));
			}
			else
			{
				Lcd_clear(lcd);
				Lcd_cursor(lcd, 0, 0);
				Lcd_string(lcd, "Datei-Fehler");
				while(!Button_pressed(ENTER));
			}

		}
		else
			exit = 1;
	}
}

void DMX_Playback(Lcd_HandleTypeDef *lcd)
{
	Lcd_clear(lcd);

	if(select_file(lcd))
	{
		if(read_infofile(&Univers))
		{
			memset(&Univers.DMXFile, 0, sizeof(FIL));
			f_open(&Univers.DMXFile, Univers.DMXFile_name, FA_OPEN_ALWAYS | FA_READ);
			UINT bytesread;
			uint8_t value = 0;
			uint16_t channel = 0;
			uint32_t read_packets = 0;

			Lcd_clear(lcd);
			Lcd_cursor(lcd, 0, 7);
			Lcd_string(lcd, "start?");

			while(!Button_pressed(ENTER))
				if(Button_pressed(BACK))
					return;

			Lcd_clear(lcd);
			Lcd_string_length(lcd, "Wiedergabe: ", 12);
			Univers.sending = 1;

			if(Univers.rec_time > 0)	//Normale Wiedergabe
			{
				uint32_t starttime = 0;

				f_lseek(&Univers.DMXFile, 0);

				Lcd_string_length(lcd, "loop", 4);
				Lcd_cursor(lcd, 1, 0);
				Lcd_string_length(lcd, Univers.DMXFile_name, MAX_FN_LENGTH);

				htim13.Instance->CNT = 0;
				m_seconds_passed = 0;
				HAL_TIM_Base_Start_IT(&htim13);

				while(!Button_pressed(BACK))
				{
					f_read(&Univers.DMXFile, &starttime, 4, &bytesread);

					value = 0;
					channel = 0;
					while(value != 1  && channel < 514 && bytesread > 0)
					{
						if(channel == 512)
							HAL_Delay(1);
						f_read(&Univers.DMXFile, &value, 1, &bytesread);
						Univers.TxBuffer[channel] = value;
						channel++;
					}
					read_packets++;

					if(bytesread < 1)
					{
						f_lseek(&Univers.DMXFile, 0);
						read_packets = 0;
						m_seconds_passed = 0;
						while(m_seconds_passed < 4);
						m_seconds_passed = 0;
					}
					else
					{
						while(m_seconds_passed < starttime);
						DMX_Transmit(&Univers, channel - 1);
					}

//					show_progressbar(lcd, 3, Univers.received_packets, read_packets);
				}
				HAL_TIM_Base_Stop_IT(&htim13);
				f_close(&Univers.DMXFile);
			}
			else						//Step Wiedrgabe
			{
				Lcd_string_length(lcd, "step", 4);
				Lcd_cursor(lcd, 1, 0);
				Lcd_string_length(lcd, Univers.DMXFile_name, MAX_FN_LENGTH);

				enc_position = 450;
				HAL_TIM_Base_Start_IT(&htim13);
				while(!Button_pressed(BACK))
				{
					if(read_packets >= Univers.received_packets)
					{
						f_lseek(&Univers.DMXFile, 0);
						read_packets = 0;
					}

					f_lseek(&Univers.DMXFile, Univers.DMXFile.fptr + 4);	//Skip starttime

					value = 0;
					channel = 0;
					while(value != 1)
					{
						f_read(&Univers.DMXFile, &value, 1, &bytesread);
						Univers.TxBuffer[channel] = value;
						channel++;
					}

					read_packets++;

					while(m_seconds_passed < (10 * enc_position))
						if(enc_position < 23)
							enc_position = 23;
					m_seconds_passed = 0;
					DMX_Transmit(&Univers, channel - 1);


					Lcd_cursor(lcd, 3, 0);
					Lcd_int(lcd, enc_position);
					Lcd_string_length(lcd, "ms  ", 2);
					show_progressbar(lcd, 2, Univers.received_packets, read_packets);
				}
				HAL_TIM_Base_Stop_IT(&htim13);
				Univers.sending = 0;
				f_close(&Univers.DMXFile);
				HAL_GPIO_WritePin(LED_TX_GPIO_Port, LED_TX_Pin, GPIO_PIN_RESET);
			}
		}
		else
		{
			Lcd_string(lcd, "Could not open file");
			while(!Button_pressed(BACK));
		}
	}
}

uint8_t select_file(Lcd_HandleTypeDef *lcd)
{
	FRESULT res;
	DIR directory;
	FILINFO info;
	const TCHAR pattern[] = "*.dmx";
	TCHAR names[20][MAX_FN_LENGTH] = {0}, arrow = 0x7E;
	uint8_t counter = 0, prev_position = 0;

	enc_position = 0;

	Lcd_clear(lcd);
	Lcd_string_length(lcd, "Aufnahme waehlen", 16);

	res = f_findfirst(&directory, &info, "", pattern);
	while(res == FR_OK && info.fname[0])
	{
		memcpy(names[counter], info.fname, MAX_FN_LENGTH);
		res = f_findnext(&directory, &info);
		counter++;
	}
	if(counter == 0)
	{
		Lcd_cursor(lcd, 2, 0);
		Lcd_string_length(lcd, "Keine Aufnahmen...", 18);
		while(!Button_pressed(BACK));
		return 0;
	}

	Lcd_cursor(lcd, 1, 0);
	Lcd_string_length(lcd, &arrow, 1);

	for(int i = 0; i < 3; i++)
	{
		Lcd_cursor(lcd, i+1, 1);
		if(names[i])
			Lcd_string_length(lcd, names[i], MAX_FN_LENGTH);
	}

	HAL_Delay(1000);

	while(!Button_pressed(BACK))
	{
		if(enc_position < 0)
			enc_position = 0;
		if(enc_position > counter - 1)
			enc_position = counter - 1;
		if(enc_position != prev_position)
		{
			for(int i = 0; i < 3; i++)
			{
				Lcd_cursor(lcd, i+1, 1);
				if(enc_position + i > counter - 1)
					Lcd_string_length(lcd, "            ", 12);
				else
					Lcd_string_length(lcd, names[enc_position + i], MAX_FN_LENGTH);
				HAL_Delay(5);
				prev_position = enc_position;
			}
		}
		if(Button_pressed(ENTER))
		{
			memcpy(Univers.DMXFile_name, names[enc_position], MAX_FN_LENGTH);
			memcpy(Univers.DMXInfoFile_name, names[enc_position], 8);
			return 1;
		}
	}

	return 0;
}

uint8_t delete_file(Lcd_HandleTypeDef * lcd)
{
	while(!Button_pressed(BACK))
	{
		if(select_file(lcd))
		{
			Lcd_clear(lcd);
			Lcd_string_length(lcd, "Loeschen?", 9);
			uint8_t delete = 0;
			while(!delete)
			{
				if(Button_pressed(ENTER))
					delete = 1;
				else if(Button_pressed(BACK))
				{
					Lcd_clear(lcd);
					Lcd_string_length(lcd, "abgebrochen", 11);
					while(!Button_pressed(ENTER));
					return 0;
				}
			}

			Lcd_clear(lcd);
			if(f_unlink(Univers.DMXFile_name) == FR_OK && f_unlink(Univers.DMXInfoFile_name) == FR_OK)
			{
				Lcd_string_length(lcd, "Erfolgreich!", 12);
				while(!Button_pressed(ENTER));
				return 1;
			}
			else
			{
				Lcd_string_length(lcd, "Fehler!", 7);
				while(!Button_pressed(ENTER));
				return 0;
			}
		}
		else
			return 0;
	}
	return 0;
}

void show_progressbar(Lcd_HandleTypeDef *lcd, uint8_t lcd_row, uint32_t received, uint32_t read)
{

}

void check_newpacketcharacter()
{
	for(int i = 0; i < 513; i++)
		if(Univers.RxBuffer[i] == Univers.newpacketcharacter)
			Univers.RxBuffer[i] = Univers.exchangecharacter;
}

uint8_t DMX_setRecTime(DMX_TypeDef *hdmx, Lcd_HandleTypeDef *lcd)
{
	char arrow = 94;
	uint8_t lcd_positions[4] = {0, 3, 7, 11};
	uint8_t position = 0, previous_position = -1, previous_enc_position = 0;

	enc_position = 0;

	Lcd_clear(lcd);
	HAL_Delay(2);
	Lcd_string(lcd, "Zeit waehlen");
	while(!Button_pressed(BACK))
	{
		if(position != previous_position || enc_position != previous_enc_position)
		{
			previous_position = position;
			previous_enc_position = enc_position;
			switch(position)
			{
			case 0:
			{
				if(enc_position > 9)
					enc_position = 9;
				break;
			}
			case 1:
			case 2:
			{
				if(enc_position > 59)
					enc_position = 59;
				break;
			}
			case 3:
			{
				if(enc_position > 99)
					enc_position = 99;
				break;
			}
			}
			time[position] = enc_position;

			Lcd_cursor(lcd, 2, lcd_positions[0]);
			Lcd_int(lcd, time[hour]);
			Lcd_string(lcd, "h");

			Lcd_cursor(lcd, 2, lcd_positions[1]);
			if(time[minute] < 10)
				Lcd_string(lcd, "0");
			Lcd_int(lcd, time[minute]);
			Lcd_string(lcd, "m");

			Lcd_cursor(lcd, 2, lcd_positions[2]);
			if(time[second] < 10)
				Lcd_string(lcd, "0");
			Lcd_int(lcd, time[second]);
			Lcd_string(lcd, "s");

			Lcd_cursor(lcd, 2, lcd_positions[3]);
			if(time[m_second] < 10)
				Lcd_string(lcd, "0");
			Lcd_int(lcd, time[m_second]);
			Lcd_int(lcd, 0);
			Lcd_string(lcd, "ms");


			Lcd_cursor(lcd, 3, 0);
			Lcd_string(lcd, "                    ");
			Lcd_cursor(lcd, 3, lcd_positions[position]);
			Lcd_string_length(lcd, &arrow, 1);

			HAL_Delay(200);
		}

		if(Button_pressed(UP) && (position > 0))
		{
			position--;
			enc_position = time[position];
		}

		if(Button_pressed(DOWN) && position < 3)
		{
			position ++;
			enc_position = time[position];
		}
		if(Button_pressed(ENTER))
		{
			hdmx->rec_time = (time[hour] * 100 * 60 * 60) + (time[minute] * 100 * 60) + (time[second] * 100) + time[m_second];
			return 1;
		}
	}
	return 0;
}

uint8_t DMX_setFilename(DMX_TypeDef *hdmx, Lcd_HandleTypeDef *lcd)
{
	uint8_t position = 0, previous_position = -1, previous_enc_position = -1;
	char name[MAX_FN_LENGTH], arrow = 0x5E;
	for(int i = 0; i < MAX_FN_LENGTH; i++)
		name[i] = '_';
	name[MAX_FN_LENGTH-1] = 'x';
	name[MAX_FN_LENGTH-2] = 'm';
	name[MAX_FN_LENGTH-3] = 'd';
	name[MAX_FN_LENGTH-4] = '.';
	enc_position = 65;
	Lcd_clear(lcd);
	Lcd_cursor(lcd, 0, 0);
	Lcd_string(lcd, "Namen eingeben");
	Lcd_cursor(lcd, 3, 0);
	Lcd_string_length(lcd, &arrow, 1);

	while(!Button_pressed(BACK))
	{
		if(enc_position != previous_enc_position || position != previous_position)
		{
			if(previous_enc_position == 'A' && enc_position < 'A')
				enc_position = '_';
			else if(previous_enc_position == ' ' && enc_position > ' ')
				enc_position = '_';
			else if(previous_enc_position == ' ' && enc_position < ' ')
				enc_position = '9';
			else if(previous_enc_position == '9' && enc_position > '9')
				enc_position = ' ';
			else if(previous_enc_position == 'Z' && enc_position > 'Z')
				enc_position = 'a';
			else if(previous_enc_position == 'a' && enc_position < 'a')
				enc_position = 'Z';
			else if(previous_enc_position == 'z' && enc_position > 'z')
				enc_position = '0';
			else if(previous_enc_position == '0' && enc_position < '0')
				enc_position = 'z';
			else if(previous_enc_position == '_' && enc_position < '_')
				enc_position = ' ';
			else if(previous_enc_position == '_' && enc_position > '_')
				enc_position = 'A';
			name[position] = enc_position;
			previous_enc_position = enc_position;

			Lcd_cursor(lcd, 2, 0);
			Lcd_string_length(lcd, name, MAX_FN_LENGTH);
//			HAL_Delay(300);
		}
		if(Button_pressed(UP) && position > 0)
		{
			Lcd_cursor(lcd, 3, position);
			Lcd_string_length(lcd, " ", 1);
			position--;
			Lcd_cursor(lcd, 3, position);
			Lcd_string_length(lcd, &arrow, 1);
			enc_position = name[position];
			HAL_Delay(500);
		}
		if(Button_pressed(DOWN) && position < MAX_FN_LENGTH - 5)
		{
			Lcd_cursor(lcd, 3, position);
			Lcd_string_length(lcd, " ", 1);
			position++;
			Lcd_cursor(lcd, 3, position);
			Lcd_string_length(lcd, &arrow, 1);
			enc_position = name[position];
			HAL_Delay(500);
		}
		if(Button_pressed(ENTER))
		{
			memcpy(hdmx->DMXFile_name, name, MAX_FN_LENGTH);
			memcpy(hdmx->DMXInfoFile_name, name, MAX_FN_LENGTH);
			hdmx->DMXInfoFile_name[MAX_FN_LENGTH-1] = 'o';
			hdmx->DMXInfoFile_name[MAX_FN_LENGTH-2] = 'f';
			hdmx->DMXInfoFile_name[MAX_FN_LENGTH-3] = 'n';
			return 1;
		}
	}
	return 0;
}

uint8_t write_infofile(DMX_TypeDef *hdmx)
{
	UINT byteswritten = 0;
	FIL File;
	if(f_open(&File, Univers.DMXInfoFile_name, FA_WRITE | FA_OPEN_ALWAYS) != FR_OK)
		return 0;
	f_write(&File, &Univers.rec_time, 4, &byteswritten);
	f_write(&File, &Univers.received_packets, 4, &byteswritten);
	f_write(&File, &Univers.newpacketcharacter, 1, &byteswritten);
	f_close(&File);
	return 1;
}

uint8_t read_infofile(DMX_TypeDef *hdmx)
{
	UINT bytesread;
	FIL File;
	if(f_open(&File, hdmx->DMXInfoFile_name, FA_OPEN_ALWAYS | FA_READ)  == FR_OK)
	{
		f_read(&File, &hdmx->rec_time, 4, &bytesread);
		f_read(&File, &hdmx->received_packets, 4, &bytesread);
		f_read(&File, &hdmx->newpacketcharacter, 1, &bytesread);
		f_close(&File);
		return 1;
	}
	else
	{
		return 0;
	}
}

UINT save_packet()
{
	UINT byteswritten;
	uint32_t time = m_seconds_passed;
	check_newpacketcharacter();
	f_write(&Univers.DMXFile, &time, 4, &byteswritten);
	f_write(&Univers.DMXFile, Univers.RxBuffer, 513, &byteswritten);
	f_write(&Univers.DMXFile, &Univers.newpacketcharacter, 1, &byteswritten);
	f_sync(&Univers.DMXFile);
	return byteswritten;
}

void BSP_SD_ReadCpltCallback(void)
{
	HAL_GPIO_TogglePin(LED_SD_GPIO_Port, LED_SD_Pin);
}

void BSP_SD_WriteCpltCallback(void)
{
	HAL_GPIO_TogglePin(LED_SD_GPIO_Port, LED_SD_Pin);
}
