#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "alt_types.h"
#include "altera_avalon_sgdma.h"
#include "hardware_setup.h"


/**********************************************************************************
 * This function is called with double pointers so that it can modify (allocate)  *
 * the regions in memory to be pointed at. The easiest way to call this function  *
 * is to pass in single pointers by reference (thereby creating double pointers). *
 *                                                                                *
 * Since the descriptor masters are 256 bits wide (32 bytes) we must ensure that  *
 * all descriptors are located on 32 byte boundaries. This function will allocate *
 * enough space for one additional descriptor and slide the pointer from the      *
 * beginning until it is on a 32 byte boundary. Malloc can only provide 8 byte    *
 * alignment so without a custom memory allocation library this step is necessary *
 *                                                                                *
 * Returns 0 for sucess, 1 for failure.                                           *
 **********************************************************************************/
alt_u32 create_descriptors(alt_sgdma_descriptor ** transmit_descriptors_p,
                      alt_sgdma_descriptor ** transmit_descriptors_copy_p,
                      alt_sgdma_descriptor ** receive_descriptors_p,
                      alt_sgdma_descriptor ** receive_descriptors_copy_p,
#ifndef LINEAR_TEST
                      alt_u32 ** input_buffers,
                      alt_u32 ** output_buffers,
#else
                      alt_8 ** input_buffers,
                      alt_16 ** output_buffers,
#endif
                      alt_u16 buffer_lengths,
                      alt_u32 number_of_buffers)
{
  /* Allocate some big buffers to hold all descriptors which will slide until
   * the first 32 byte boundary is found */
  void * temp_ptr;
  alt_u16 buffer_counter;
  alt_sgdma_descriptor *transmit_descriptors, *receive_descriptors;


/**************************************************************
   * Allocation of the transmit descriptors                   *
   * - First allocate a large buffer to the temporary pointer *
   * - Second check for successful memory allocation          *
   * - Third put this memory location into the pointer copy   *
   *   to be freed before the program exits                   *
   * - Forth slide the temporary pointer until it lies on a 32*
   *   byte boundary (descriptor master is 256 bits wide)     * 
   ************************************************************/  
  temp_ptr = malloc((number_of_buffers + 2) * ALTERA_AVALON_SGDMA_DESCRIPTOR_SIZE);
  if(temp_ptr == NULL)
  {
    printf("Failed to allocate memory for the transmit descriptors\n");
    return 1; 
  }
  *transmit_descriptors_copy_p = (alt_sgdma_descriptor *)temp_ptr;
  
  while((((alt_u32)temp_ptr) % ALTERA_AVALON_SGDMA_DESCRIPTOR_SIZE) != 0)
  {
    temp_ptr++;  // slide the pointer until 32 byte boundary is found
  }

  transmit_descriptors = (alt_sgdma_descriptor *)temp_ptr;

  *transmit_descriptors_p = transmit_descriptors;
/**************************************************************/


  /* Clear out the null descriptor owned by hardware bit.  These locations
   * came from the heap so we don't know what state the bytes are in (owned bit could be high).*/
  transmit_descriptors[number_of_buffers].control = 0;

/**************************************************************
   * Allocation of the receive descriptors                    *
   * - First allocate a large buffer to the temporary pointer *
   * - Second check for successful memory allocation          *
   * - Third put this memory location into the pointer copy   *
   *   to be freed before the program exits                   *
   * - Forth slide the temporary pointer until it lies on a 32*
   *   byte boundary (descriptor master is 256 bits wide)     * 
   ************************************************************/  
  temp_ptr = malloc((number_of_buffers + 2) * ALTERA_AVALON_SGDMA_DESCRIPTOR_SIZE);
  if(temp_ptr == NULL)
  {
    printf("Failed to allocate memory for the receive descriptors\n");
    return 1; 
  }
  *receive_descriptors_copy_p = (alt_sgdma_descriptor *)temp_ptr;
  
  while((((alt_u32)temp_ptr) % ALTERA_AVALON_SGDMA_DESCRIPTOR_SIZE) != 0)
  {
    temp_ptr++;  // slide the pointer until 32 byte boundary is found
  }

  receive_descriptors = (alt_sgdma_descriptor *)temp_ptr;
  *receive_descriptors_p = receive_descriptors;
/**************************************************************/
  
  /* Clear out the null descriptor owned by hardware bit.  These locations
   * came from the heap so we don't know what state the bytes are in (owned bit could be high).*/
  receive_descriptors[number_of_buffers].control = 0;
  //receive_descriptors[0].actual_bytes_transferred = !0;

  for(buffer_counter = 0; buffer_counter < number_of_buffers; buffer_counter++)
  {
	  /* This will create a descriptor that is capable of transmitting data from an Avalon-MM buffer
	   * to a packet enabled Avalon-ST FIFO component */
	  alt_avalon_sgdma_construct_mem_to_stream_desc(transmit_descriptors+buffer_counter,  // descriptor
                                                	transmit_descriptors+buffer_counter+1, // next descriptor
                                                	(alt_u32*)input_buffers[buffer_counter],  // read buffer location
#ifndef LINEAR_TEST
                                                	(alt_u16)buffer_lengths*sizeof(alt_u32),  // length of the buffer
#else
                                                	(alt_u16)buffer_lengths*sizeof(alt_u16),
#endif
                                                	0, // reads are not from a fixed location
                                                	0, // start of packet is enabled for the Avalon-ST interfaces
                                                	0, // end of packet is enabled for the Avalon-ST interfaces,
                                                	0);  // there is only one channel

	  /* This will create a descriptor that is capable of transmitting data from an Avalon-ST FIFO
	   * to an Avalon-MM buffer */

	  alt_avalon_sgdma_construct_stream_to_mem_desc(receive_descriptors+buffer_counter,  // descriptor
                                                	receive_descriptors+buffer_counter+1, // next descriptor
                                                	(alt_u32*)output_buffers[buffer_counter],  // write buffer location
#ifndef LINEAR_TEST
                                                	(alt_u16)buffer_lengths*sizeof(alt_u32),  // length of the buffer
#else
                                                	(alt_u16)buffer_lengths*sizeof(alt_u16),
#endif
                                                	0); // writes are not to a fixed location
  }

  return 0;  // no failures in allocation
}
