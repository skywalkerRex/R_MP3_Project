#include "gpio_lab.h"

#include "../lpc40xx.h"
#include "stdio.h"

void gpio0__set_as_input(uint8_t pin_num) { LPC_GPIO1->DIR &= ~(1U << pin_num); }

/// Should alter the hardware registers to set the pin as output
void gpio0__set_as_output(uint8_t pin_num) { LPC_GPIO1->DIR |= (1U << pin_num); }

/// Should alter the hardware registers to set the pin as high
void gpio0__set_high(uint8_t pin_num) { LPC_GPIO1->SET = (1U << pin_num); }

/// Should alter the hardware registers to set the pin as low
void gpio0__set_low(uint8_t pin_num) { LPC_GPIO1->CLR = (1U << pin_num); }

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpio0__set(uint8_t pin_num, bool high) {
  if (high) {
    LPC_GPIO1->SET = (1U << pin_num);
  } else {
    LPC_GPIO1->CLR = (1U << pin_num);
  }
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpio0__get_level(uint8_t pin_num) {
  if (LPC_GPIO1->PIN & (1 << pin_num)) {
    return true;
  } else {
    return false;
  }
}

/*

void gpio0__set_as_input(uint8_t IO_num, uint8_t pin_num) {
  switch (IO_num) {
  case 0:
    LPC_GPIO0->DIR &= ~(1U << pin_num);
    break;

  case 1:
    LPC_GPIO1->DIR &= ~(1U << pin_num);
    break;

  case 2:
    LPC_GPIO2->DIR &= ~(1U << pin_num);
    break;

  case 3:
    LPC_GPIO3->DIR &= ~(1U << pin_num);
    break;

  case 4:
    LPC_GPIO4->DIR &= ~(1U << pin_num);
    break;

  case 5:
    LPC_GPIO5->DIR &= ~(1U << pin_num);
    break;

  default:
    break;
  }
}

/// Should alter the hardware registers to set the pin as output
void gpio0__set_as_output(uint8_t IO_num, uint8_t pin_num) {
  switch (IO_num) {
  case 0:
    LPC_GPIO0->DIR |= (1U << pin_num);
    break;
  case 1:
    LPC_GPIO1->DIR |= (1U << pin_num);
    break;
  case 2:
    LPC_GPIO2->DIR |= (1U << pin_num);
    break;
  case 3:
    LPC_GPIO3->DIR |= (1U << pin_num);
    break;
  case 4:
    LPC_GPIO4->DIR |= (1U << pin_num);
    break;
  case 5:
    LPC_GPIO5->DIR |= (1U << pin_num);
    break;
  default:
    break;
  }
}

/// Should alter the hardware registers to set the pin as high
void gpio0__set_high(uint8_t IO_num, uint8_t pin_num) {
  switch (IO_num) {
  case 0:
    LPC_GPIO0->SET = (1U << pin_num);
    break;
  case 1:
    LPC_GPIO1->SET = (1U << pin_num);
    break;
  case 2:
    LPC_GPIO2->SET = (1U << pin_num);
    break;
  case 3:
    LPC_GPIO3->SET = (1U << pin_num);
    break;
  case 4:
    LPC_GPIO4->SET = (1U << pin_num);
    break;
  case 5:
    LPC_GPIO5->SET = (1U << pin_num);
    break;
  default:
    break;
  }
}

/// Should alter the hardware registers to set the pin as low
void gpio0__set_low(uint8_t IO_num, uint8_t pin_num) {
  switch (IO_num) {
  case 0:
    LPC_GPIO0->CLR = (1U << pin_num);
    break;
  case 1:
    LPC_GPIO1->CLR = (1U << pin_num);
    break;
  case 2:
    LPC_GPIO2->CLR = (1U << pin_num);
    break;
  case 3:
    LPC_GPIO3->CLR = (1U << pin_num);
    break;
  case 4:
    LPC_GPIO4->CLR = (1U << pin_num);
    break;
  case 5:
    LPC_GPIO5->CLR = (1U << pin_num);
    break;
  default:
    break;
  }
}


void gpio0__set(uint8_t IO_num, uint8_t pin_num, bool high) {
  if (high) {
    gpio0__set_high(IO_num, pin_num);
  } else {
    gpio0__set_low(IO_num, pin_num);
  }
}


bool gpio0__get_level(uint8_t IO_num, uint8_t pin_num) {
  switch (IO_num) {
  case 0:
    if (LPC_GPIO0->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  case 1:
    if (LPC_GPIO1->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  case 2:
    if (LPC_GPIO2->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  case 3:
    if (LPC_GPIO3->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  case 4:
    if (LPC_GPIO4->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  case 5:
    if (LPC_GPIO5->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
    break;
  default:
    break;
  }
}
*/