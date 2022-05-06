

#if 0

void get_seconds_and_minutes_from_ctime_register(uint8_t *seconds, uint8_t *minutes) {
  uint32_t S_MASK = (0x3F << 0);
  uint32_t M_MASK = (0x3F << 8);
  *seconds = LPC_RTC->CTIME0 & S_MASK;
  *minutes = ((LPC_RTC->CTIME0 & M_MASK) >> 8);
}

void producer(void *p) {
  uint8_t s;
  uint8_t m;
  LPC_RTC->CCR |= (1 << 0);
  while (1) {
    get_seconds_and_minutes_from_ctime_register(&s, &m);
    printf("Time: %i:%i\n", m, s);
    vTaskDelay(1000);
  }
}

int main(void) {
  xTaskCreate(producer, "producer", (512U * 4) / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  vTaskStartScheduler();
  return 0;
}

QueueHandle_t q;

void producer(void *p) {
  int x = 10;
  while (1) {
    xQueueSend(q, &x, 0);
    vTaskDelay(100);
  }
}

void consumer(void *p) {
  int y;
  while (1) {
    xQueueReceive(q, &y, 50);
    printf("Received %d\n", y);
  }
}

int main(void) {
  q = xQueueCreate(1, sizeof(int));
  xTaskCreate(producer, "producer", 1024, NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(consumer, "consumer", 1024, NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}

#endif