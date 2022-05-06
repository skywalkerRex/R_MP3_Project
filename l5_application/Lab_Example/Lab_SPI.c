
#if 0 // part 1
void adesto_cs(void) {
  gpio_s Pin_CS = {1, 10};
  gpio__set_as_output(Pin_CS);
  gpio__reset(Pin_CS);
  gpio_s Pin_Trig = {1, 31};
  gpio__set_as_output(Pin_Trig);
  gpio__set(Pin_Trig);
}

void adesto_ds(void) {
  gpio_s Pin_CS = {1, 10};
  gpio__set_as_output(Pin_CS);
  gpio__set(Pin_CS);
  gpio_s Pin_Trig = {1, 31};
  gpio__set_as_output(Pin_Trig);
  gpio__reset(Pin_Trig);
}

typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

adesto_flash_id_s adesto_read_signature(void) {
  adesto_flash_id_s data = {0};

  adesto_cs();
  spi2__exchange_byte(0x9F);
  data.manufacturer_id = spi2__exchange_byte(0x00);
  data.device_id_1 = spi2__exchange_byte(0x00);
  data.device_id_2 = spi2__exchange_byte(0x00);
  // data.extended_device_id = spi2__exchange_byte(0x00);
  adesto_ds();

  return data;
}

void spi_task(void *p) {
  const uint32_t spi_clock_mhz = 24;
  spi2_init(spi_clock_mhz);
  spi2_pin_init();

  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    printf("\nManufacturer id: 0x%X\n", id.manufacturer_id);
    printf("Device id 01: 0x%X\n", id.device_id_1);
    printf("Device id 02: 0x%X\n", id.device_id_2);
    // printf("Extended device id: 0x%X\n", id.extended_device_id);
    vTaskDelay(500);
  }
}

void main(void) {
  xTaskCreate(spi_task, "spi", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif

#if 0 // part 2

SemaphoreHandle_t spi_bus_mutex;

void adesto_cs(void) {
  gpio_s Pin_CS = {1, 10};
  gpio__set_as_output(Pin_CS);
  gpio__reset(Pin_CS);
  gpio_s Pin_Trig = {1, 31};
  gpio__set_as_output(Pin_Trig);
  gpio__set(Pin_Trig);
}

void adesto_ds(void) {
  gpio_s Pin_CS = {1, 10};
  gpio__set_as_output(Pin_CS);
  gpio__set(Pin_CS);
  gpio_s Pin_Trig = {1, 31};
  gpio__set_as_output(Pin_Trig);
  gpio__reset(Pin_Trig);
}

typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

adesto_flash_id_s adesto_read_signature(void) {
  adesto_flash_id_s data = {0};

  adesto_cs();
  spi2__exchange_byte(0x9F);
  data.manufacturer_id = spi2__exchange_byte(0x00);
  data.device_id_1 = spi2__exchange_byte(0x00);
  data.device_id_2 = spi2__exchange_byte(0x00);
  adesto_ds();

  return data;
}

void spi_task(void *p) {
  const uint32_t spi_clock_mhz = 24;
  spi2_init(spi_clock_mhz);
  spi2_pin_init();

  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    printf("\nManufacturer id: 0x%X\n", id.manufacturer_id);
    printf("Device id 01: 0x%X\n", id.device_id_1);
    printf("Device id 02: 0x%X\n", id.device_id_2);
    // printf("Extended device id: 0x%X\n", id.extended_device_id);
    vTaskDelay(500);
  }
}

adesto_flash_id_s ssp2__adesto_read_signature(void) {
  const uint32_t spi_clock_mhz = 1;
  spi2_init(spi_clock_mhz);
  spi2_pin_init();
  while (1) {
    if (xSemaphoreTake(spi_bus_mutex, 1000)) {
      adesto_flash_id_s id = adesto_read_signature();
      xSemaphoreGive(spi_bus_mutex);
      return id;
    }
  }
}

void spi_id_verification_task(void *p) {
  while (1) {
    const adesto_flash_id_s id = ssp2__adesto_read_signature();
    // When we read a manufacturer ID we do not expect, we will kill this task
    if (0x1F != id.manufacturer_id) {
      fprintf(stderr, "Manufacturer ID read failure\n");
      vTaskSuspend(NULL); // Kill this task
    }
  }
}

void main(void) {
  spi_bus_mutex = xSemaphoreCreateMutex();
  xTaskCreate(spi_id_verification_task, "spi1", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(spi_id_verification_task, "spi2", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif