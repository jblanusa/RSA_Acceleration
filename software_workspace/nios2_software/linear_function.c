#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "alt_types.h"
#include "altera_avalon_sgdma.h"
#include "system.h"
#include "errno.h"

#include "hardware_setup.h"

#define VERBOSE_LEVEL 1

/**********************************************************************************
 * Hardware calculating linear function. Input data is transfered to accelerator  *
 * using DMA controller. Output samples are transfered back to memory using       *
 * second DMA controller.                                                         *
 **********************************************************************************/
#ifdef LINEAR_TEST
alt_u32 linear_function_hw(alt_sgdma_dev * transmit_DMA,
		                alt_sgdma_descriptor * transmit_descriptors,
		                volatile alt_u16 * tx_done_p,
		                alt_sgdma_dev * receive_DMA,
                        alt_sgdma_descriptor * receive_descriptors,
                        volatile alt_u16 * rx_done_p,
                        alt_8 param_a,
                        alt_8 param_b)
{
/************************************************************
 * Configure linear function module.                        *
 ************************************************************/

  IOWR_8DIRECT(LINEAR_FUNCTION_BASE, 0, param_a);
  IOWR_8DIRECT(LINEAR_FUNCTION_BASE, 1, param_b);

/**************************************************************
   * Starting both the transmit and receive transfers         *
   ************************************************************/

#if VERBOSE_LEVEL>0
    printf("Starting up the SGDMA engines\n");
#endif

  /* Start non blocking transfer with DMA modules. */
  if(alt_avalon_sgdma_do_async_transfer(transmit_DMA, &transmit_descriptors[0]) != 0)
  {
    printf("Writing the head of the transmit descriptor list to the DMA failed\n");
    return 1;
  }
  if(alt_avalon_sgdma_do_async_transfer(receive_DMA, &receive_descriptors[0]) != 0)
  {
    printf("Writing the head of the receive descriptor list to the DMA failed\n");
    return 1;
  }

/**************************************************************/


/**************************************************************
   * Blocking until the SGDMA interrupts fire                 *
   ************************************************************/
  while(*tx_done_p < 1) {}
#if VERBOSE_LEVEL>0
  printf("The transmit SGDMA linear has completed\n");
#endif
  while(*rx_done_p < 1) {}
#if VERBOSE_LEVEL>0
  printf("The receive SGDMA linear has completed\n");
#endif

  *tx_done_p = 0;
  *rx_done_p = 0;

/**************************************************************
   * Stop the SGDMAs                                          *
   ************************************************************/
  alt_avalon_sgdma_stop(transmit_DMA);
  alt_avalon_sgdma_stop(receive_DMA);

  return 0;
}
#endif
