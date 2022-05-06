#pragma once

#include <stdint.h>
#include <stdlib.h>

void ssp0__pin_init();

/// Initialize the bus with the given maximum clock rate in Khz
void ssp0__initialize(uint32_t max_clock_khz);

/// After initialization, this allows you to change the bus clock speed
void ssp0__set_max_clock(uint32_t max_clock_khz);

/**
 * Exchange a single byte over the SPI bus
 * @returns the byte received while sending the byte_to_transmit
 */
uint8_t ssp0__exchange_byte(uint8_t byte_to_transmit);

/**
 * @{
 * @name    Exchanges larger blocks over the SPI bus
 * These are designed to be one-way transmission for the SPI for now
 */
