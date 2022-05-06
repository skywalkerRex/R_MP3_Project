#include <stdio.h>
#include <string.h>

#if 1 // default addon
#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"
#endif

#if 1 // addon
//#include "../l3_drivers/adc.h"
#include "acceleration.h"
#include "adc.h"
#include "event_groups.h"
#include "ff.h"
#include "i2c.h"
#include "lpc40xx.h"
#include "pwm1.h"
#include "queue.h"
#endif

#include "R_CTRL.h"
#include "R_LCD.h"
#include "R_MP3.h"

// Don't Touch
// xTaskCreate(uart2_write_task, "UART_R", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

int main(void) {
  sj2_cli__init(); // CLI init
  LCD_init(0x4E);
  if (board_io__sd_card_is_present()) {
    mp3_init();
    key_board_init();
  } else {
    setCursor(1, 2);
    LCD_printf("SD Card Unfound", 15);
  }
  xTaskCreate(mp3_Song_From_Queue, "MP3_Song", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(mp3_Play_Data_Get, "MP3_Play", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(key_board, "Key_Board", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler(); // Run RTOS
  return 0;
}

// Don't Touch
