// @file gpio_isr.c
#include "gpio_isr.h"

// Note: You may want another separate array for falling vs. rising edge callbacks
static function_pointer_t gpio0_callbacks_R[2][32];
static function_pointer_t gpio0_callbacks_F[2][32];

void gpio0__attach_interrupt(uint8_t port, uint32_t pin, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 1) Store the callback based on the pin at gpio0_callbacks
  // 2) Configure GPIO 0 pin for rising or falling edge
  gpio_s but = {port, pin};
  gpio__set_as_input(but);
  if (port == 0) {
    if (interrupt_type) {
      gpio0_callbacks_R[0][pin] = callback;
      LPC_GPIOINT->IO0IntEnR |= (1u << pin);
    } else {
      gpio0_callbacks_F[0][pin] = callback;
      LPC_GPIOINT->IO0IntEnF |= (1u << pin);
    }
  } else if (port == 2) {
    if (interrupt_type) {
      gpio0_callbacks_R[1][pin] = callback;
      LPC_GPIOINT->IO2IntEnR |= (1u << pin);
    } else {
      gpio0_callbacks_F[1][pin] = callback;
      LPC_GPIOINT->IO2IntEnF |= (1u << pin);
    }
  }
}

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {
  // Check which pin generated the interrupt
  int pin_that_generated_interrupt;
  function_pointer_t attached_user_handler;
  bool Rise = false;
  bool is_0 = false;
  for (int i = 0; i < 32; i++) {
    if ((LPC_GPIOINT->IO0IntStatR >> i) & 0x1) {
      pin_that_generated_interrupt = i;
      Rise = true;
      is_0 = true;
      break;
    } else if ((LPC_GPIOINT->IO0IntStatF >> i) & 0x1) {
      pin_that_generated_interrupt = i;
      Rise = false;
      is_0 = true;
      break;
    } else if ((LPC_GPIOINT->IO2IntStatR >> i) & 0x1) {
      pin_that_generated_interrupt = i;
      Rise = true;
      is_0 = false;
      break;
    } else if ((LPC_GPIOINT->IO2IntStatF >> i) & 0x1) {
      pin_that_generated_interrupt = i;
      Rise = false;
      is_0 = false;
      break;
    }
  }
  if (is_0) {
    if (Rise) {
      attached_user_handler = gpio0_callbacks_R[0][pin_that_generated_interrupt];
    } else {
      attached_user_handler = gpio0_callbacks_F[0][pin_that_generated_interrupt];
    }
  } else {
    if (Rise) {
      attached_user_handler = gpio0_callbacks_R[1][pin_that_generated_interrupt];
    } else {
      attached_user_handler = gpio0_callbacks_F[1][pin_that_generated_interrupt];
    }
  }

  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  LPC_GPIOINT->IO0IntClr |= (1u << pin_that_generated_interrupt);
}