/*
 * rsa_crypto_hw.c
 *
 *  Created on: Aug 18, 2016
 *      Author: Korisnik
 */
#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "alt_types.h"
#include "altera_avalon_sgdma.h"
#include "system.h"

#define VERBOSE_LEVEL 1

#define RSA_EXP_REG 0
#define RSA_MOD_REG 1

alt_u32 rsa_crypto_hw(alt_sgdma_dev * transmit_DMA,
						alt_sgdma_descriptor * transmit_descriptors,
						volatile alt_u16 * tx_done_p,
						alt_sgdma_dev * receive_DMA,
						alt_sgdma_descriptor * receive_descriptors,
						volatile alt_u16 * rx_done_p,
						alt_u32 * exp,
						alt_u32 * mod,
						alt_u16 bitsize)
{

/************************************************************
 * Configure rsa_wrapper module.                        *
 ************************************************************/
alt_u16 bit_iter, i;
for(bit_iter = 0, i = 0; bit_iter < bitsize; bit_iter += 32, i++){
	IOWR(RSA_WRAPPER_BASE, RSA_EXP_REG, exp[i]);
	IOWR(RSA_WRAPPER_BASE, RSA_MOD_REG, mod[i]);
}

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
  printf("The transmit SGDMA has completed\n");
#endif
  while(*rx_done_p < 1) {}
#if VERBOSE_LEVEL>0
  printf("The receive SGDMA has completed\n");
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

