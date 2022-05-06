/*
Part0:

void gpio_interrupt(void);

gpio_s button = {0, 30};
gpio_s LED = {1, 24};

void main(void) {
  // Read Table 95 in the LPC user manual and setup an interrupt on a switch connected to Port0 or Port2
  // a) For example, choose SW2 (P0_30) pin on SJ2 board and configure as input
  //.   Warning: P0.30, and P0.31 require pull-down resistors
  gpio__set_as_input(button);
  gpio__set_as_output(LED);

  // b) Configure the registers to trigger Port0 interrupt (such as falling edge)

  gpio__enable_pull_down_resistors(button);
  LPC_GPIOINT->IO0IntEnR |= (1u << 30);

  // Install GPIO interrupt function at the CPU interrupt (exception) vector
  // c) Hijack the interrupt vector at interrupt_vector_table.c and have it call our gpio_interrupt()
  //    Hint: You can declare 'void gpio_interrupt(void)' at interrupt_vector_table.c such that it can see this function

  // Most important step: Enable the GPIO interrupt exception using the ARM Cortex M API (this is from lpc40xx.h)
  NVIC_EnableIRQ(GPIO_IRQn);

  // Toggle an LED in a loop to ensure/test that the interrupt is entering ane exiting
  // For example, if the GPIO interrupt gets stuck, this LED will stop blinking
  gpio__reset(LED);
  while (1) {
    gpio__toggle(LED);
    delay__ms(100);
    fprintf(stderr, "\nAAA");
    // TODO: Toggle an LED here
  }

  // vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
}

void gpio_interrupt(void) {
  LPC_GPIOINT->IO0IntClr |= (1u << 30);
  gpio__set(LED);
  fprintf(stderr, "\nButton Pressed\n");
  delay__ms(1000);
  // a) Clear Port0/2 interrupt using CLR0 or CLR2 registers
  // b) Use fprintf(stderr) or blink and LED here to test your ISR
}

*/

/*
Part1:

gpio_s button = {0, 30};
gpio_s LED = {1, 24};

SemaphoreHandle_t switch_pressed_signal;

void gpio_interrupt();
void sleep_on_sem_task(void *p);

void main(void) {
  switch_pressed_signal = xSemaphoreCreateBinary(); // Create your binary semaphore
  gpio__set_as_input(button);
  gpio__set_as_output(LED);
  gpio__enable_pull_down_resistors(button);
  LPC_GPIOINT->IO0IntEnR |= (1u << 30);
  gpio__set(LED);

  gpio_interrupt();          // TODO: Setup interrupt by re-using code from Part 0
  NVIC_EnableIRQ(GPIO_IRQn); // Enable interrupt gate for the GPIO

  xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

// WARNING: You can only use printf(stderr, "foo") inside of an ISR
void gpio_interrupt(void) {
  fprintf(stderr, "ISR Entry");
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1u << 30);
}

void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTakeFromISR(switch_pressed_signal, portMAX_DELAY)) {
      printf("Semaphore taken\n");
      gpio__reset(LED);
      delay__ms(500);
      gpio__set(LED);
    }
  }
}

*/

#if 0 // Part2:

gpio_s button = {0, 30};
gpio_s LED = {1, 24};
gpio_s LED2 = {1, 18};

SemaphoreHandle_t switch_pressed_signal;
SemaphoreHandle_t switch_pressed_signal_2;

void gpio_interrupt();
void sleep_on_sem_task(void *p);

void pin30_isr(void) {
  fprintf(stderr, "\nISR Entry on 30: ");
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
}

void pin29_isr(void) {
  fprintf(stderr, "\nISR Entry on 29: ");
  xSemaphoreGiveFromISR(switch_pressed_signal_2, NULL);
}

void main(void) {
  switch_pressed_signal = xSemaphoreCreateBinary();   // Create your binary semaphore
  switch_pressed_signal_2 = xSemaphoreCreateBinary(); // Create your binary semaphore
  gpio0__attach_interrupt(0, 30, GPIO_INTR__RISING_EDGE, pin30_isr);
  gpio0__attach_interrupt(0, 29, GPIO_INTR__FALLING_EDGE, pin29_isr);

  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio0__interrupt_dispatcher, "isr");
  NVIC_EnableIRQ(GPIO_IRQn); // Enable interrupt gate for the GPIO

  xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTakeFromISR(switch_pressed_signal, portMAX_DELAY)) {
      printf("Semaphore taken\n");
      gpio__set(LED);
      delay__ms(1000);
      gpio__reset(LED);
      // Do more stuff below ...
    }
    if (xSemaphoreTakeFromISR(switch_pressed_signal_2, portMAX_DELAY)) {
      printf("Semaphore taken\n");
      gpio__set(LED2);
      delay__ms(1000);
      gpio__reset(LED2);
      // Do more stuff below ...
    }
  }
}

#endif

/*
Final



gpio_s button = {0, 30};
gpio_s LED = {1, 24};
gpio_s LED2 = {1, 18};

SemaphoreHandle_t switch_pressed_signal;
SemaphoreHandle_t switch_pressed_signal_2;

void gpio_interrupt();
void sleep_on_sem_task(void *p);

void pin30_isr(void) {
  fprintf(stderr, "\nISR Entry on 30: ");
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
}

void pin29_isr(void) {
  fprintf(stderr, "\nISR Entry on 29: ");
  xSemaphoreGiveFromISR(switch_pressed_signal_2, NULL);
}

void main(void) {
  switch_pressed_signal = xSemaphoreCreateBinary();   // Create your binary semaphore
  switch_pressed_signal_2 = xSemaphoreCreateBinary(); // Create your binary semaphore
  gpio0__attach_interrupt(0, 30, GPIO_INTR__RISING_EDGE, pin30_isr);
  gpio0__attach_interrupt(0, 29, GPIO_INTR__FALLING_EDGE, pin29_isr);

  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio0__interrupt_dispatcher, "isr");
  NVIC_EnableIRQ(GPIO_IRQn); // Enable interrupt gate for the GPIO

  xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTakeFromISR(switch_pressed_signal, portMAX_DELAY)) {
      printf("Semaphore taken\n");
      gpio__set(LED);
      delay__ms(1000);
      gpio__reset(LED);
      // Do more stuff below ...
    }
    if (xSemaphoreTakeFromISR(switch_pressed_signal_2, portMAX_DELAY)) {
      printf("Semaphore taken\n");
      gpio__set(LED2);
      delay__ms(1000);
      gpio__reset(LED2);
      // Do more stuff below ...
    }
  }
}

*/