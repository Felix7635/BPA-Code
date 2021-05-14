#include "main.h"
#include "gpio.h"

#define ENC_UP 1
#define ENC_DOWN 2
#define ENC_TIMEOUT 0

uint8_t enc_move(uint32_t timeout);
