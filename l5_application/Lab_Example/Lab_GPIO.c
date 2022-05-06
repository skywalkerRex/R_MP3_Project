/*
Part 0:

void led_task(void *pvParameters) {
  const uint32_t led = (1U << 18);
  // Choose one of the onboard LEDS by looking into schematics and write code for the below
  // 0) Set the IOCON MUX function(if required) select pins to 000
  // 1) Set the DIR register bit for the LED port pin

  LPC_GPIO1->DIR |= led;

  while (true) {
    LPC_GPIO1->SET = led;
    // 2) Set PIN register bit to 0 to turn ON LED (led may be active low)
    vTaskDelay(500);
    LPC_GPIO1->CLR = led;
    // 3) Set PIN register bit to 1 to turn OFF LED
    vTaskDelay(500);
  }
}

int main(void) {
  // create_blinky_tasks();
  // create_uart_task();

  // port_pin_s Led_P;

  // Led_P.pin = 3;

  xTaskCreate(led_task, "led", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  // Start Scheduler - This will not return, and your tasks will start to run their while(1) loop
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}
*/

/*
Part 2:

typedef struct {
  // uint8_t port;

  uint8_t pin;
} port_pin_s;

port_pin_s Led0, Led1;

void led_task(void *task_parameter) {
  // Type-cast the paramter that was passed from xTaskCreate()
  const port_pin_s *led = (port_pin_s *)(task_parameter);

  uint8_t p = led->pin;

  printf("\nPin_Print:  %i\n", p);

  gpio0__set_as_output(p);

  while (true) {
    gpio0__set_high(p);
    vTaskDelay(200);

    gpio0__set_low(p);
    vTaskDelay(1000);
  }
}

int main(void) {
  // create_blinky_tasks();
  // create_uart_task();

  Led0.pin = 24;
  Led1.pin = 18;

  xTaskCreate(led_task, "led0", 2048 / sizeof(void *), &Led0, 1, NULL);
  xTaskCreate(led_task, "led1", 2048 / sizeof(void *), &Led1, 1, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}

*/
