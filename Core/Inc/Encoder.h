#include "main.h"
#include "gpio.h"

#define ENC_UP 1
#define ENC_DOWN 2
#define ENC_TIMEOUT 0

extern volatile uint16_t enc_position;

uint8_t enc_move(uint32_t timeout);
void enc_Interrupt();
