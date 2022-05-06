#if 0
static EventGroupHandle_t WatchDog_Eve;
static QueueHandle_t acc_queue;

void producer(void *p) {
  int avgx, avgy, avgz;
  acceleration__axis_data_s data;
  acceleration__axis_data_s avg = {0, 0, 0};
  
  while (1) {
    avgx = 0;
    avgy = 0;
    avgz = 0;
    for (int i = 0; i < 100; i++) {
      data = acceleration__get_data();
      avgx += data.x;
      avgy += data.y;
      avgz += data.z;
      vTaskDelay(1);
    }
    avg.x = avgx / 100;
    avg.y = avgy / 100;
    avg.z = avgz / 100;
    xQueueSend(acc_queue, &avg, 100);
    xEventGroupSetBits(WatchDog_Eve, 0x1);
  }
}

void consumer(void *p) {
  int time;
  const char *filename = "sensor.csv";
  FIL file; // File handle
  UINT bytes_written = 0;
  FRESULT result;
  result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  acceleration__axis_data_s data;
  char string[64];
  int count = 0;
  if (FR_OK == result) {
    xQueueReceive(acc_queue, &data, portMAX_DELAY);
    time = xTaskGetTickCount();
    sprintf(string, "time, x, y, z, \n");
    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
      // printf("%s", string);
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
  } else {
    printf("ERROR: Failed to open: %s\n", filename);
  }
  while (1) {
    if (FR_OK == result) {
      xQueueReceive(acc_queue, &data, portMAX_DELAY);
      time = xTaskGetTickCount();
      sprintf(string, "%i, %i, %i, %i, \n", time, data.x, data.y, data.z);
      if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
        // printf("%s", string);
      } else {
        printf("ERROR: Failed to write data to file\n");
      }
    } else {
      printf("ERROR: Failed to open: %s\n", filename);
    }
    count++;
    if (count == 10) {
      f_close(&file);
      result = f_open(&file, filename, (FA_WRITE | FA_OPEN_APPEND));
      count = 0;
    }
    xEventGroupSetBits(WatchDog_Eve, 0x2);
  }
}

void watchdog_task(void *P) {
  EventBits_t uxBits;
  while (1) {
    uxBits = xEventGroupWaitBits(WatchDog_Eve, 0x3, pdTRUE, pdTRUE, 120);
    if ((uxBits & 0x3) == (0x3)) {
      printf("Both Run\n");
    } else if ((uxBits & 0x1) != 0) {
      printf("Consumer did not run\n");
    } else if ((uxBits & 0x2) != 0) {
      printf("Producer did not run\n");
    } else {
      printf("Both did not run\n");
    }
  }
}

void main(void) {
  create_uart_task();
  acceleration__init();
  xTaskCreate(watchdog_task, "WatchDogs", (512U * 8) / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(producer, "Proc", (512U * 8) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(consumer, "Cons", (512U * 8) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);

  acc_queue = xQueueCreate(1, sizeof(acceleration__axis_data_s));
  WatchDog_Eve = xEventGroupCreate();
  vTaskStartScheduler();
}

static void create_uart_task(void) {
  // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
  // Change '#if (0)' to '#if (1)' and vice versa to try it out
#if (0)
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif
}

// This sends periodic messages over printf() which uses system_calls.c to send them to UART0
static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  TickType_t ticks = 0;

  while (true) {
    // This loop will repeat at precise task delay, even if the logic below takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 2000);

    /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire output will be fully
     * sent out before this function returns. See system_calls.c for actual implementation.
     *
     * Use this style print for:
     *  - Interrupts because you cannot use printf() inside an ISR
     *    This is because regular printf() leads down to xQueueSend() that might block
     *    but you cannot block inside an ISR hence the system might crash
     *  - During debugging in case system crashes before all output of printf() is sent
     */
    ticks = xTaskGetTickCount();
    fprintf(stderr, "%u: This is a polled version of printf used for debugging ... finished in", (unsigned)ticks);
    fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

    /* This deposits data to an outgoing queue and doesn't block the CPU
     * Data will be sent later, but this function would return earlier
     */
    ticks = xTaskGetTickCount();
    printf("This is a more efficient printf ... finished in");
    printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
  }
}

#endif