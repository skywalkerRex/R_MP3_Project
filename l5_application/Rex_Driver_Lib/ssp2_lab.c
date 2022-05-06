// ssp2_lab.c

#include "ssp2_lab.h"
#include "lpc40xx.h"
#include <stdint.h>

void gpio_set_spi_function(uint32_t *pointer) {
  *pointer &= ~0b111;
  *pointer |= 0b100;
}

void spi2_pin_init() {
  gpio_set_spi_function(&LPC_IOCON->P1_0); // P1.0 --> SCK2
  gpio_set_spi_function(&LPC_IOCON->P1_1); // P1.1 --> MOSI
  gpio_set_spi_function(&LPC_IOCON->P1_4); // P1.4 --> MISO
  // P1.10 -> CS for external flash
  LPC_IOCON->P1_10 &= ~0b111; // Leave as GPIO
}

void spi2_init(int freq) {
  const uint32_t spi2_power_bit = (1 << 20); // Set UP Power bit
  LPC_SC->PCONP |= spi2_power_bit;

  const uint32_t scr_value = 0;
  LPC_SSP2->CR0 = (0b111 << 0) | (scr_value << 8); // set it to 8bit transfer and the divider of the clock

  const uint32_t ssp2_enable = (1 << 1); // Enable SSP2
  LPC_SSP2->CR1 = ssp2_enable;

  LPC_SSP2->CPSR = 96 / freq; // 96/4 = 24Mhz
}

uint8_t spi2__exchange_byte(uint8_t data_out) {
  const uint32_t BSY_Mask = (1U << 4); // BSY Busy. This bit is 0 if the SSPn controller is idle, or 1 if it is
                                       // currently sending/receiving a frame and/or the Tx FIFO is not empty.
  LPC_SSP2->DR = data_out;
  while ((LPC_SSP2->SR & BSY_Mask)) {
  }
  return LPC_SSP2->DR;
  // Configure the Data register(DR) to send and receive data by checking the SPI peripheral status register
}