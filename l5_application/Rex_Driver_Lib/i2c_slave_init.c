#include "i2c_slave_init.h"

#include "lpc40xx.h"
#include "stdio.h"

void i2c2__slave_init(uint8_t slave_address_to_respond_to) {
  uint32_t CLR_Mask = (0x1F << 2);
  uint32_t I2EN_Flag_Mask = (1 << 6);
  uint32_t AA_Flag_Mask = (1 << 2);
  LPC_I2C2->CONSET &= ~(CLR_Mask);
  LPC_I2C2->CONSET |= AA_Flag_Mask;
  LPC_I2C2->CONSET |= I2EN_Flag_Mask;
  LPC_I2C2->ADR0 = (slave_address_to_respond_to);
}
