/*
 * rsa_hardware.h
 *
 *  Created on: Aug 17, 2016
 *      Author: Korisnik
 */

#ifndef RSA_HARDWARE_H_
#define RSA_HARDWARE_H_

#include "alt_types.h"
#include "altera_avalon_sgdma.h"

alt_u32 create_test_data(alt_u32 *** input_buffers_p,
                         alt_u32 *** output_buffers_p,
                         alt_u16 number_of_buffers,
                         alt_u16 buffer_length);

alt_u32 create_descriptors(alt_sgdma_descriptor ** transmit_descriptors_p,
                      alt_sgdma_descriptor ** transmit_descriptors_copy_p,
                      alt_sgdma_descriptor ** receive_descriptors_p,
                      alt_sgdma_descriptor ** receive_descriptors_copy_p,
                      alt_u32 ** input_buffers,
                      alt_u32 ** output_buffers,
                      alt_u16 buffer_lengths,
                      alt_u32 number_of_buffers);

alt_u32 rsa_crypto_hw(alt_sgdma_dev * transmit_DMA,
                    alt_sgdma_descriptor * transmit_descriptors,
                    volatile alt_u16 * tx_done_p,
                    alt_sgdma_dev * receive_DMA,
                    alt_sgdma_descriptor * receive_descriptors,
                    volatile alt_u16 * rx_done_p,
                    alt_u32 * exp,
                    alt_u32 * mod,
                    alt_u16 bitsize);

alt_u32 linear_function_hw(alt_sgdma_dev * transmit_DMA,
		                alt_sgdma_descriptor * transmit_descriptors,
		                volatile alt_u16 * tx_done_p,
		                alt_sgdma_dev * receive_DMA,
                        alt_sgdma_descriptor * receive_descriptors,
                        volatile alt_u16 * rx_done_p,
                        alt_8 param_a,
                        alt_8 param_b);

void validate_hw_results(alt_sgdma_descriptor * transmit_descriptors,
                         alt_sgdma_descriptor * receive_descriptors,
                         alt_u32 number_of_buffers,
                         alt_8 param_a,
                         alt_8 param_b);

#endif /* RSA_HARDWARE_H_ */
