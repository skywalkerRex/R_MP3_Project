#include "i2c_slave_functions.h"
#include "lpc40xx.h"
#include "stdio.h"

static volatile uint8_t slave_memory[256];

bool i2c_slave_callback__read_memory(uint8_t memory_index, uint8_t *memory) {
  if (memory_index < 256) {
    *memory = slave_memory[memory_index];
    // fprintf(stderr, "Read 0x%X from 0x%X\n", slave_memory[memory_index], memory_index);
    return true;
  } else {
    return false;
  }
}

bool i2c_slave_callback__write_memory(uint8_t memory_index, uint8_t memory_value) {
  if (memory_index < 256) {
    slave_memory[memory_index] = memory_value;
    // fprintf(stderr, "Write 0x%X to 0x%X\n", memory_value, memory_index);
    return true;
  } else {
    return false;
  }
}