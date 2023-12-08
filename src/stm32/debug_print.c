// Debug print for STM32G0, fixed USART3 (PB8)

#include "autoconf.h" // CONFIG_SERIAL_BAUD
#include "command.h" // DECL_CONSTANT_STR
#include "internal.h" // enable_pclock
#include "sched.h" // DECL_INIT
#include "gpio.h"
#include <stdbool.h>

DECL_CONSTANT_STR("RESERVE_PINS_debug", "PB8");
#define DEBUG_GPIO_TX GPIO('B', 8)
#define DEBUG_GPIO_TX_FUNCTION GPIO_FUNCTION(4)
#define DEBUG_USART USART3

// Missing from the header
#define USART_ISR_TXE USART_ISR_TXE_TXFNF

static bool debug_print_inited;

static void debug_print_init(void)
{
  if (debug_print_inited) {
    return;
  }
  debug_print_inited = true;

  enable_pclock((uint32_t)DEBUG_USART);

  uint32_t pclk = get_pclock_frequency((uint32_t)DEBUG_USART);
  uint32_t div = DIV_ROUND_CLOSEST(pclk, CONFIG_DEBUG_PRINT_BAUD);

  DEBUG_USART->BRR = div;
  DEBUG_USART->CR3 = USART_CR3_OVRDIS;
  DEBUG_USART->CR1 = USART_CR1_UE | USART_CR1_TE;

  gpio_peripheral(DEBUG_GPIO_TX, DEBUG_GPIO_TX_FUNCTION, 0);
}

void debug_print(const char *s)
{
  debug_print_init();
  for (; *s; ++s) {
    while (!(DEBUG_USART->ISR & USART_ISR_TXE))
      ;
    DEBUG_USART->TDR = *s;
  }
}
