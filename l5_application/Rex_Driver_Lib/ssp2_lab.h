// ssp2_lab.h

#include <stdint.h>

#pragma once

void spi2_pin_init();

void spi2_init(int freq);

uint8_t spi2__exchange_byte(uint8_t data_out);