#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"

static SemaphoreHandle_t switch_press_indication;

static void task_one(void *task_parameter);
static void task_two(void *task_parameter);
/*
static void task_one(void *task_parameter) {
  while (true) {
    // Read existing main.c regarding when we should use fprintf(stderr...) in place of printf()
    // For this lab, we will use fprintf(stderr, ...)
    fprintf(stderr, "AAAAAAAAAAAA");

    // Sleep for 100ms
    vTaskDelay(100);
  }
}

static void task_two(void *task_parameter) {
  while (true) {
    fprintf(stderr, "bbbbbbbbbbbb");
    vTaskDelay(100);
  }
}

typedef struct {
  uint8_t pin;
} port_pin_s;

port_pin_s Led, SW;

void led_task(void *task_parameter) {
  const port_pin_s *led = (port_pin_s *)(task_parameter);
  uint8_t p = led->pin;
  while (true) {
    // Note: There is no vTaskDelay() here, but we use sleep mechanism while waiting for the binary semaphore (signal)
    if (xSemaphoreTake(switch_press_indication, 1000)) {
      gpio0__set_low(p);
      vTaskDelay(100);
    } else {
      puts("Timeout: No switch press indication for 1000ms");
    }
    gpio0__set_high(p);
    vTaskDelay(100);
  }
}

void switch_task(void *task_parameter) {
  port_pin_s *sw = (port_pin_s *)task_parameter;

  while (true) {
    // TODO: If switch pressed, set the binary semaphore
    if (gpio0__get_level(sw->pin)) {
      xSemaphoreGive(switch_press_indication);
    }

    // Task should always sleep otherwise they will use 100% CPU
    // This task sleep also helps avoid spurious semaphore give during switch debeounce
    vTaskDelay(100);
  }
}

void switch_task(void *task_parameter) {
  port_pin_s *sw = (port_pin_s *)task_parameter;

  while (true) {
    // TODO: If switch pressed, set the binary semaphore
    if (gpio0__get_level(sw->pin)) {
      xSemaphoreGive(switch_press_indication);
    }

    // Task should always sleep otherwise they will use 100% CPU
    // This task sleep also helps avoid spurious semaphore give during switch debeounce
    vTaskDelay(100);
  }
}*/
/*


int main(void) {
  // create_blinky_tasks();
  // create_uart_task();

  Led.pin = 26;
  SW.pin = 15;

  switch_press_indication = xSemaphoreCreateBinary();

  xTaskCreate(led_task, "led", 2048 / sizeof(void *), &Led, 1, NULL);
  xTaskCreate(switch_task, "sw", 2048 / sizeof(void *), &SW, 1, NULL);
  //Start Scheduler - This will not return, and your tasks will start to run their while(1) loop
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}


  */