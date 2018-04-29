#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/alt_irq.h>

#include <system.h>
#include <sys/alt_timestamp.h>
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "altera_avalon_sgdma.h"
#include "altera_avalon_sgdma_regs.h"
#include "alt_types.h"

#include "hardware_setup.h"
#include "rsa_crypto.h"
#include "test.h"

#define LEDS_ALL_MSK 0xFF

extern volatile alt_u16 button_action;

int main()
{
  init_hardware();

  CryptoTest4();

  while(1){}

  return 0;
}
