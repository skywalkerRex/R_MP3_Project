#pragma once

#include "FreeRTOS.h"
#include "R_MP3.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Butt_UP 0
#define Butt_DOWN 1
#define Butt_LEFT 2
#define Butt_RIGHT 3
#define Butt_PLAY 4
#define Butt_MODE 5
#define Butt_LOCK 6

QueueHandle_t Butt_Queues[7];

void key_board_init(); // Inital the Keyboard Button

void key_board(void *p); // Key Board Checking Task
