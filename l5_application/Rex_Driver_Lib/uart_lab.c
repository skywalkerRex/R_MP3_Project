#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static QueueHandle_t uart_rx_queue;

typedef enum {
  UART_2,
  UART_3,
} uart_number_e;

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  const uint32_t uart2_power_bit = (1 << 24); // Uart2 Power bit
  const uint32_t uart3_power_bit = (1 << 25); // Uart3 Power bit
  const uint32_t DLAB_MASK = (1 << 7);
  const uint32_t Baud_SET = peripheral_clock * 1000 * 1000 / (16 * baud_rate);

  if (uart == UART_2) {
    LPC_SC->PCONP |= uart2_power_bit;
    LPC_UART2->LCR |= DLAB_MASK;
    LPC_UART2->DLM = (Baud_SET >> 8) & 0xFF;
    LPC_UART2->DLL = (Baud_SET)&0xFF;
    LPC_UART2->LCR &= ~DLAB_MASK;
    LPC_UART2->LCR |= (3 << 0); // Set the LCR to 8bit mode
    LPC_UART2->FCR |= (1 << 0);
  } else if (uart == UART_3) {
    LPC_SC->PCONP |= uart3_power_bit;
    LPC_UART3->LCR |= DLAB_MASK;
    LPC_UART3->DLM = (Baud_SET >> 8) & 0xFF;
    LPC_UART3->DLL = (Baud_SET)&0xFF;
    LPC_UART3->LCR &= ~DLAB_MASK;
    LPC_UART3->LCR |= (3 << 0); // Set the LCR to 8bit mode
    LPC_UART3->FCR |= (1 << 0);
  }
}

bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
  if (uart == UART_2) {
    while (!(LPC_UART2->LSR & (0x1))) {
    }
    *input_byte = (LPC_UART2->RBR & 0xFF);
    return true;
  } else if (uart == UART_3) {
    while (!(LPC_UART3->LSR & (0x1))) {
    }
    *input_byte = (LPC_UART3->RBR & 0xFF);
    return true;
  } else {
    return false;
  }
}

bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  if (uart == UART_2) {
    while (!(LPC_UART2->LSR & (1 << 5))) {
    }
    LPC_UART2->THR = output_byte;
    return true;
  } else if (uart == UART_3) {
    while (!(LPC_UART3->LSR & (1 << 5))) {
    }
    LPC_UART3->THR = output_byte;
    return true;
  } else {
    return false;
  }
}

static void UART2_receive_interrupt(void) {
  // TODO: Read the IIR register to figure out why you got interrupted
  uint32_t status;
  status = (LPC_UART2->IIR >> 1) & 0b111;
  if (status == 0x2) {
    // 0x2 2a - Receive Data Available (RDA).
    while (!(LPC_UART2->LSR & (0x1))) {
    }
    const char byte = LPC_UART2->RBR;
    xQueueSendFromISR(uart_rx_queue, &byte, NULL);
  } else if (status == 0x3) {
    // 0x3 1 - Receive Line Status (RLS).
  } else if (status == 0x6) {
    // 0x6 2b - Character Time-out Indicator (CTI).
  } else if (status == 0x1) {
    // 0x1 3 - THRE Interrupt
  }
}

static void UART3_receive_interrupt(void) {
  uint32_t status;
  status = (LPC_UART3->IIR >> 1) & 0b111;
  if (status == 0x2) {
    // 0x2 2a - Receive Data Available (RDA).
    while (!(LPC_UART3->LSR & (0x1))) {
    }
    const char byte = LPC_UART3->RBR;
    xQueueSendFromISR(uart_rx_queue, &byte, NULL);
  } else if (status == 0x3) {
    // 0x3 1 - Receive Line Status (RLS).
  } else if (status == 0x6) {
    // 0x6 2b - Character Time-out Indicator (CTI).
  } else if (status == 0x1) {
    // 0x1 3 - THRE Interrupt
  }
}

// Public function to enable UART interrupt
// TODO Declare this at the header file
void uart__enable_receive_interrupt(uart_number_e uart_number) {
  // TODO: Use lpc_peripherals.h to attach your interrupt

  // TODO: Enable UART receive interrupt by reading the LPC User manual
  // Hint: Read about the IER register
  static uint32_t RBRIE_MASK = (1 << 0);
  if (uart_number == UART_2) {
    NVIC_EnableIRQ(UART2_IRQn);
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, UART2_receive_interrupt, "UART2");
    LPC_UART2->IER |= RBRIE_MASK;
  }

  if (uart_number == UART_3) {
    NVIC_EnableIRQ(UART3_IRQn);
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART3, UART3_receive_interrupt, "UART3");
    LPC_UART3->IER |= RBRIE_MASK;
  }

  // TODO: Create your RX queue
  uart_rx_queue = xQueueCreate(10, sizeof(char));
}

// Public function to get a char from the queue (this function should work without modification)
// TODO: Declare this at the header file
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(uart_rx_queue, input_byte, timeout);
}