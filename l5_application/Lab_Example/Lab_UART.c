
#if 0 // Part 0&1
void uart_read_task(void *p) {
  while (1) {
    char recevie;
    uart_lab__polled_get(UART_3, &recevie);
    fprintf(stderr, "UART Recevie: %c\n", recevie);
    vTaskDelay(500);
  }
}

void uart_write_task(void *p) {
  while (1) {
    uart_lab__polled_put(UART_3, 'A');
    vTaskDelay(500);
  }
}

void uart3_pin_config() {
  gpio_s UART3_TX = {4, 28};
  gpio_s UART3_RX = {4, 29};
  gpio__set_as_output(UART3_TX);
  gpio__construct_with_function(4, 28, 0b010); // Set up Uart2 TX
  gpio__set_as_input(UART3_RX);
  gpio__construct_with_function(4, 29, 0b010); // Set up Uart2 RX
}

void uart2_pin_config() {
  gpio_s UART2_TX = {2, 8};
  gpio_s UART2_RX = {2, 9};
  gpio__set_as_output(UART2_TX);
  gpio__construct_with_function(2, 8, 0b010); // Set up Uart2 TX
  gpio__set_as_input(UART2_RX);
  gpio__construct_with_function(2, 9, 0b010); // Set up Uart2 RX
}

void main(void) {
  uart_lab__init(UART_3, 96, 115200);
  uart3_pin_config();
  xTaskCreate(uart_read_task, "UART_T", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(uart_write_task, "UART_R", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
}
#endif

#if 0 // Part 2
void uart2_int_read_task(void *p) {
  while (1) {
    char recevie;
    uart_lab__get_char_from_queue(&recevie, 500);
    fprintf(stderr, "UART Recevie: %c\n", recevie);
    vTaskDelay(500);
  }
}

void uart2_read_task(void *p) {
  while (1) {
    char recevie;
    uart_lab__polled_get(UART_2, &recevie);
    fprintf(stderr, "UART Recevie: %c\n", recevie);
    vTaskDelay(500);
  }
}

void uart2_write_task(void *p) {
  while (1) {
    uart_lab__polled_put(UART_2, 'A');
    vTaskDelay(500);
  }
}

void uart3_write_task(void *p) {
  while (1) {
    uart_lab__polled_put(UART_3, 'B');
    vTaskDelay(500);
  }
}

void uart2_pin_config() {
  gpio_s UART2_TX = {2, 8};
  gpio_s UART2_RX = {2, 9};
  gpio__set_as_output(UART2_TX);
  gpio__construct_with_function(2, 8, 0b010); // Set up Uart2 TX
  gpio__set_as_input(UART2_RX);
  gpio__construct_with_function(2, 9, 0b010); // Set up Uart2 RX
}

void uart3_pin_config() {
  gpio_s UART3_TX = {4, 28};
  gpio_s UART3_RX = {4, 29};
  gpio__set_as_output(UART3_TX);
  gpio__construct_with_function(4, 28, 0b010); // Set up Uart3 TX
  gpio__set_as_input(UART3_RX);
  gpio__construct_with_function(4, 29, 0b010); // Set up Uart3 RX
}

void main(void) {
  uart_lab__init(UART_2, 96, 115200);
  uart2_pin_config();
  uart__enable_receive_interrupt(UART_2);
  xTaskCreate(uart2_int_read_task, "UART_T", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(uart2_write_task, "UART_R", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif

#if 0 // Part 3
void uart2_int_read_task(void *p) {
  while (1) {
    char recevie;
    uart_lab__get_char_from_queue(&recevie, 500);
    fprintf(stderr, "UART Recevie: %c\n", recevie);
    vTaskDelay(500);
  }
}

void uart2_read_task(void *p) {
  while (1) {
    char recevie;
    uart_lab__polled_get(UART_2, &recevie);
    fprintf(stderr, "UART Recevie: %c\n", recevie);
    vTaskDelay(500);
  }
}

void uart2_write_task(void *p) {
  while (1) {
    uart_lab__polled_put(UART_2, 'A');
    vTaskDelay(500);
  }
}

void uart3_write_task(void *p) {
  while (1) {
    uart_lab__polled_put(UART_3, 'R');
    vTaskDelay(500);
  }
}

void uart2_pin_config() {
  gpio_s UART2_TX = {2, 8};
  gpio_s UART2_RX = {2, 9};
  gpio__set_as_output(UART2_TX);
  gpio__construct_with_function(2, 8, 0b010); // Set up Uart2 TX
  gpio__set_as_input(UART2_RX);
  gpio__construct_with_function(2, 9, 0b010); // Set up Uart2 RX
}

void uart3_pin_config() {
  gpio_s UART3_TX = {4, 28};
  gpio_s UART3_RX = {4, 29};
  gpio__set_as_output(UART3_TX);
  gpio__construct_with_function(4, 28, 0b010); // Set up Uart3 TX
  gpio__set_as_input(UART3_RX);
  gpio__construct_with_function(4, 29, 0b010); // Set up Uart3 RX
}

void main(void) {
  uart_lab__init(UART_2, 96, 115200);
  uart_lab__init(UART_3, 96, 115200);
  uart2_pin_config();
  uart3_pin_config();
  uart__enable_receive_interrupt(UART_2);
  xTaskCreate(uart2_int_read_task, "UART_2_R", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(uart3_write_task, "UART_3_T", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif
