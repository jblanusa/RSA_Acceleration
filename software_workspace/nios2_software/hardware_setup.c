/*
 * hardware_setup.c
 *
 *  Created on: Aug 18, 2016
 *      Author: Korisnik
 */
#include <stdio.h>
#include <stdlib.h>

#include <system.h>
#include <sys/alt_irq.h>
#include "alt_types.h"
#include "altera_avalon_sgdma.h"
#include "altera_avalon_sgdma_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "rsa_crypto.h"
#include "hardware_setup.h"

#define BTN_0 0x1
#define BTN_1 0x2
#define BTN_ALL_MSK (BTN_0 + BTN_1)

volatile alt_u16 button_action = 0;
volatile alt_u16 milis = 0;
volatile alt_u16 tx_done = 0;
volatile alt_u16 rx_done = 0;

/*****************************************************************************
************************ PREKIDNE RUTINE *************************************
******************************************************************************/
// Prekidna rutina za tastere
void handle_button_interrupts(void *context)
{
	button_action = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PI_BUTTONS_BASE);

	// Acknowledge interrupt by clearing edge capture register
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PI_BUTTONS_BASE,button_action);
}

void handle_timer_interrupt(void *context)
{
	// Brisanje statusnog registra
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0x0);

	xorshift64star();
	(milis)++;
}

// Callback rutine za sgdma
void transmit_callback_function(void * context)
{
  	tx_done++;  /* main will be polling for this value being 1 */
}

void receive_callback_function(void * context)
{
  	rx_done++;  /* main will be polling for this value being 1 */
}
/*****************************************************************************
************************ Inicijalizacija *************************************
******************************************************************************/

void init_hardware(){
	  unsigned int timer_period = TIMER_FREQ / 1000; 				// milisekunda
/*******************************************************************************/
// BUTTON INIT	  
	  // Reset button edge capture register for all buttons
	  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PI_BUTTONS_BASE,BTN_ALL_MSK);

	  // Enable interrupts for all buttons
	  IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PI_BUTTONS_BASE,BTN_ALL_MSK);

	  // Register buttons ISR
	  alt_ic_isr_register(PI_BUTTONS_IRQ_INTERRUPT_CONTROLLER_ID,
						  PI_BUTTONS_IRQ,
						  handle_button_interrupts,
						  (void*)NULL,
						  0x0);
/*******************************************************************************/
// BUTTON INIT
	  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, timer_period);
	  IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, timer_period >> 16);

	  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE,ALTERA_AVALON_TIMER_CONTROL_ITO_MSK +
												  ALTERA_AVALON_TIMER_CONTROL_CONT_MSK +
												  ALTERA_AVALON_TIMER_CONTROL_START_MSK);

	  // Register timer ISR
	  alt_ic_isr_register(TIMER_IRQ_INTERRUPT_CONTROLLER_ID,
						  TIMER_IRQ,
						  handle_timer_interrupt,
						  (void*)0,
						  0x0);
}

int  init_sgdma(alt_sgdma_dev ** transmitDMA, alt_sgdma_dev ** receiveDMA){
#ifndef LINEAR_TEST
	*transmitDMA = alt_avalon_sgdma_open(SGDMA_TRANSMIT_NAME);
	*receiveDMA = alt_avalon_sgdma_open(SGDMA_RECEIVE_NAME);
#else
	*transmitDMA = alt_avalon_sgdma_open(SGDMA_TRANSMIT_TEST_NAME);
	*receiveDMA = alt_avalon_sgdma_open(SGDMA_RECEIVE_TEST_NAME);
#endif

	  if(*transmitDMA == NULL)
	  {
		printf("Could not open the transmit SG-DMA\n");
		return 1;
	  }
	  if(*receiveDMA == NULL)
	  {
		printf("Could not open the receive SG-DMA\n");
		return 1;
	  }
	
	  alt_avalon_sgdma_register_callback(*transmitDMA,
	                                    &transmit_callback_function,
	                                    (ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK |
	                                    ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK |
	                                    ALTERA_AVALON_SGDMA_CONTROL_PARK_MSK),
	                                    NULL);
	  alt_avalon_sgdma_register_callback(*receiveDMA,
	                                    &receive_callback_function,
	                                    (ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK |
	                                    ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK |
	                                    ALTERA_AVALON_SGDMA_CONTROL_PARK_MSK),
	                                    NULL);

	  return 0;
}
