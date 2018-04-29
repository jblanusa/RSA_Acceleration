/*
 * hardware_setup.h
 *
 *  Created on: Aug 18, 2016
 *      Author: Korisnik
 */

#ifndef HARDWARE_SETUP_H_
#define HARDWARE_SETUP_H_

#include "alt_types.h"
#include "altera_avalon_sgdma.h"
/*
#ifndef LINEAR_TEST
#define LINEAR_TEST
#endif
*/
void init_hardware();
int  init_sgdma(alt_sgdma_dev ** transmitDMA, alt_sgdma_dev ** receiveDMA);

#endif /* HARDWARE_SETUP_H_ */
