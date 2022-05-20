#pragma once

#include <stdbool.h>
#include <stdint.h>

bool i2c_slave_callback__read_memory(uint8_t memory_index, uint8_t *memory);

bool i2c_slave_callback__write_memory(uint8_t memory_index, uint8_t memory_value);