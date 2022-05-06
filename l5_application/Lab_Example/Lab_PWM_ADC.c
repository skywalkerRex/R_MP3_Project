/*
Part 0:

void pin_config_pwm() {
  LPC_IOCON->P1_18 &= ~0b010;
  LPC_IOCON->P1_18 |= 0b010;
}

void pwm_task(void *p) {
  pwm1__init_single_edge(100);

  // Locate a GPIO pin that a PWM channel will control
  // NOTE You can use gpio__constr0uct_with_function() API from gpio.h
  // TODO Write this function yourself
  pin_config_pwm();

  // We only need to set PWM configuration once, and the HW will drive
  // the GPIO at 1000Hz, and control set its duty cycle to 50%
  pwm1__set_duty_cycle(PWM1__2_0, 50);

  // Continue to vary the duty cycle in the loop
  uint8_t percent = 0;
  while (1) {
    pwm1__set_duty_cycle(PWM1__2_0, percent);
    if (++percent > 100) {
      percent = 0;
    }
    printf("MR0: 0x%X\n", LPC_PWM1->MR0);
    printf("MR1: 0x%X\n", LPC_PWM1->MR1);
    vTaskDelay(10);
  }
}

void main(void) {
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

*/

/*
Part 1：


void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  uint16_t adc_value;
  float adc_voltage;
  adc__initialize();

  // TODO This is the function you need to add to adc.h
  // You can configure burst mode for just the channel you are using
  adc__enable_burst_mode();

  // Configure a pin, such as P1.31 with FUNC 011 to route this pin as ADC channel 5
  // You can use gpio__construct_with_function() API from gpio.h
  pin_config_adc(); // TODO You need to write this function

  while (1) {
    // Get the ADC reading using a new routine you created to read an ADC burst reading
    // TODO: You need to write the implementation of this function
    adc_value = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_value / 4095.0) * 3.3;
    printf("ADC： %i \n", adc_value);
    printf("ADC_Voltage： %f \n", adc_voltage);
    vTaskDelay(100);
  }
}

void main(void) {
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}


*/

/*
Part 2



// This is the queue handle we will need for the xQueue Send/Receive API
static QueueHandle_t adc_to_pwm_task_queue;

void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  // NOTE: Reuse the code from Part 1
  adc__initialize();
  adc__enable_burst_mode();
  pin_config_adc();
  int adc_reading = 0; // Note that this 'adc_reading' is not the same variable as the one from adc_task
  float adc_voltage;
  while (1) {
    // Implement code to send potentiometer value on the queue
    // a) read ADC input to 'int adc_reading'
    // b) Send to queue: xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_reading / 4095.0) * 3.3;
    printf("ADC_Voltage: %f\n", adc_voltage);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
  }
}

void pwm_task(void *p) {
  // NOTE: Reuse the code from Part 0
  int adc_reading = 0;
  float percent = 0;
  int duty;
  pwm1__init_single_edge(100);

  // Locate a GPIO pin that a PWM channel will control
  // NOTE You can use gpio__constr0uct_with_function() API from gpio.h
  // TODO Write this function yourself
  gpio__construct_with_function(1, 18, 2);

  pwm1__set_duty_cycle(PWM1__2_0, 50);

  while (1) {
    // Implement code to receive potentiometer value from queue
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      percent = ((float)adc_reading / (float)4096) * 100;
      duty = 100 - (int)percent;
      printf("MR0: 0x%X\n", LPC_PWM1->MR0);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      pwm1__set_duty_cycle(PWM1__2_0, duty);
    }

    // We do not need task delay because our queue API will put task to sleep when there is no data in the queue
    // vTaskDelay(100);
  }
}

void main(void) {
  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}


*/

/*
Part 3:

// This is the queue handle we will need for the xQueue Send/Receive API
static QueueHandle_t adc_to_pwm_task_queue;
gpio_s Butt_0 = {0, 29};
gpio_s Butt_1 = {0, 30};
gpio_s Butt_2 = {1, 15};

void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  // NOTE: Reuse the code from Part 1
  adc__initialize();
  adc__enable_burst_mode();
  pin_config_adc();
  int adc_reading = 0; // Note that this 'adc_reading' is not the same variable as the one from adc_task
  float adc_voltage;
  while (1) {
    // Implement code to send potentiometer value on the queue
    // a) read ADC input to 'int adc_reading'
    // b) Send to queue: xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_reading / 4095.0) * 3.3;
    printf("ADC_Voltage: %f\n", adc_voltage);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
  }
}

void pwm_task(void *p) {
  // NOTE: Reuse the code from Part 0
  int adc_reading = 0;
  float percent = 0;
  int duty;
  int LED_Selec = 0;
  pwm1__init_single_edge(100);

  // Locate a GPIO pin that a PWM channel will control
  // NOTE You can use gpio__constr0uct_with_function() API from gpio.h
  // TODO Write this function yourself
  gpio__construct_with_function(2, 0, 1);
  gpio__construct_with_function(2, 1, 1);
  gpio__construct_with_function(2, 2, 1);

  pwm1__set_duty_cycle(PWM1__2_0, 50);
  pwm1__set_duty_cycle(PWM1__2_1, 50);
  pwm1__set_duty_cycle(PWM1__2_2, 50);

  while (1) {
    // Implement code to receive potentiometer value from queue
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      percent = ((float)adc_reading / (float)4096) * 100;
      duty = (int)percent;
      printf("MR0: 0x%X\n", LPC_PWM1->MR0);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      switch (LED_Selec) {
      case 0:
        pwm1__set_duty_cycle(PWM1__2_0, duty);
        break;

      case 1:
        pwm1__set_duty_cycle(PWM1__2_1, duty);
        break;

      case 2:
        pwm1__set_duty_cycle(PWM1__2_2, duty);
        break;

      default:
        break;
      }
    }
    if (gpio__get(Butt_0)) {
      LED_Selec = 0;
    }
    if (gpio__get(Butt_1)) {
      LED_Selec = 1;
    }
    if (gpio__get(Butt_2)) {
      LED_Selec = 2;
    }
    // We do not need task delay because our queue API will put task to sleep when there is no data in the queue
    // vTaskDelay(100);
  }
}

void main(void) {
  gpio__set_as_input(Butt_0);
  gpio__set_as_input(Butt_1);
  gpio__set_as_input(Butt_2);
  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

*/

/*


#if 0 // part 0
void pin_config_pwm() {
  LPC_IOCON->P1_18 &= ~0b111;
  LPC_IOCON->P1_18 |= 0b010;
}

void pwm_task(void *p) {
  pwm1__init_single_edge(100);
  pin_config_pwm();
  pwm1__set_duty_cycle(PWM1__2_0, 50);
  uint8_t percent = 0;
  while (1) {
    pwm1__set_duty_cycle(PWM1__2_0, percent);
    if (++percent > 100) {
      percent = 0;
    }
    printf("MR0: 0x%X\n", LPC_PWM1->MR0);
    printf("MR1: 0x%X\n", LPC_PWM1->MR1);
    vTaskDelay(10);
  }
}

void main(void) {
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif

#if 0 // part 1
void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  uint16_t adc_value;
  float adc_voltage;
  adc__initialize();
  adc__enable_burst_mode();
  pin_config_adc();

  while (1) {
    adc_value = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_value / 4095.0) * 3.3;
    printf("ADC： %i \n", adc_value);
    printf("ADC_Voltage： %f \n", adc_voltage);
    vTaskDelay(100);
  }
}

void main(void) {
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif

#if 0 // Part2
static QueueHandle_t adc_to_pwm_task_queue;

void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  adc__initialize();
  adc__enable_burst_mode();
  pin_config_adc();
  int adc_reading = 0;
  float adc_voltage;
  while (1) {
    adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_reading / 4095.0) * 3.3;
    printf("ADC_Voltage: %f\n", adc_voltage);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
  }
}

void pwm_task(void *p) {
  int adc_reading = 0;
  float percent = 0;
  int duty;
  pwm1__init_single_edge(100);
  gpio__construct_with_function(1, 18, 2);

  pwm1__set_duty_cycle(PWM1__2_0, 50);

  while (1) {
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      percent = ((float)adc_reading / (float)4096) * 100;
      duty = 100 - (int)percent;
      printf("MR0: 0x%X\n", LPC_PWM1->MR0);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      pwm1__set_duty_cycle(PWM1__2_0, duty);
    }
  }
}

void main(void) {
  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}
#endif

#if 1 // Part3
static QueueHandle_t adc_to_pwm_task_queue;
gpio_s Butt_0 = {0, 29};
gpio_s Butt_1 = {0, 30};
gpio_s Butt_2 = {1, 15};

void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

void adc_task(void *p) {
  adc__initialize();
  adc__enable_burst_mode();
  pin_config_adc();
  int adc_reading = 0;
  float adc_voltage;
  while (1) {
    adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    adc_voltage = ((float)adc_reading / 4095.0) * 3.3;
    printf("ADC_Voltage: %f\n", adc_voltage);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
  }
}

void pwm_task(void *p) {
  int adc_reading = 0;
  float percent = 0;
  int duty;
  int LED_Selec = 0;
  pwm1__init_single_edge(100);
  gpio__construct_with_function(2, 0, 1);
  gpio__construct_with_function(2, 1, 1);
  gpio__construct_with_function(2, 2, 1);

  pwm1__set_duty_cycle(PWM1__2_0, 50);
  pwm1__set_duty_cycle(PWM1__2_1, 50);
  pwm1__set_duty_cycle(PWM1__2_2, 50);

  while (1) {
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      percent = ((float)adc_reading / (float)4096) * 100;
      duty = (int)percent;
      printf("MR0: 0x%X\n", LPC_PWM1->MR0);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      printf("MR1: 0x%X\n", LPC_PWM1->MR1);
      switch (LED_Selec) {
      case 0:
        pwm1__set_duty_cycle(PWM1__2_0, duty);
        break;

      case 1:
        pwm1__set_duty_cycle(PWM1__2_1, duty);
        break;

      case 2:
        pwm1__set_duty_cycle(PWM1__2_2, duty);
        break;

      default:
        break;
      }
    }
    if (gpio__get(Butt_0)) {
      LED_Selec = 0;
    }
    if (gpio__get(Butt_1)) {
      LED_Selec = 1;
    }
    if (gpio__get(Butt_2)) {
      LED_Selec = 2;
    }
  }
}

void main(void) {
  gpio__set_as_input(Butt_0);
  gpio__set_as_input(Butt_1);
  gpio__set_as_input(Butt_2);
  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}

#endif


*/