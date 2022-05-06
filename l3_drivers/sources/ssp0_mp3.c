#include "ssp0_mp3.h"
#include "FreeRTOS.h"
#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>

void gpio_set_spi_func(uint32_t *pointer) {
  *pointer &= ~0b111;
  *pointer |= 0b010;
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void ssp0__pin_init() {
  gpio_set_spi_func(&LPC_IOCON->P0_15); // P0.15 --> SCK0
  gpio_set_spi_func(&LPC_IOCON->P0_17); // P0.17 --> MOSI
  gpio_set_spi_func(&LPC_IOCON->P0_18); // P0.18 --> MISO
}

void ssp0__initialize(uint32_t max_clock_khz) {
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP0);
  LPC_SSP0->CR0 = 7;        // 8-bit mode
  LPC_SSP0->CR1 = (1 << 1); // Enable SSP as Master
  ssp0__set_max_clock(max_clock_khz);
}

void ssp0__set_max_clock(uint32_t max_clock_khz) {
  uint8_t divider = 2;
  const uint32_t cpu_clock_khz = clock__get_core_clock_hz() / 1000UL;

  // Keep scaling down divider until calculated is higher
  while (max_clock_khz < (cpu_clock_khz / divider) && divider <= 254) {
    divider += 2;
  }

  LPC_SSP0->CPSR = divider;
}

uint8_t ssp0__exchange_byte(uint8_t byte_to_transmit) {
  LPC_SSP0->DR = byte_to_transmit;

  while (LPC_SSP0->SR & (1 << 4)) {
    ; // Wait until SSP is busy
  }

  return (uint8_t)(LPC_SSP0->DR & 0xFF);
}
