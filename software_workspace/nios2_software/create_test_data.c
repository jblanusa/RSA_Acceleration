/*
 * rsa_hardware.c
 *
 *  Created on: Aug 17, 2016
 *      Author: Korisnik
 */
#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "alt_types.h"
#include "hardware_setup.h"


/**********************************************************************************
 * This function is responsible for allocating input and output data buffers.     *
 * A random buffer length will be generated if minimum and maximum buffer lengths *
 * are different.                                                                 *
 *                                                                                *
 * Returns 0 for success, 1 for failure.                                          *
 **********************************************************************************/
alt_u32 create_test_data(
#ifndef LINEAR_TEST
						alt_u32  *** input_buffers_p,
		                alt_u32  *** output_buffers_p,
#else
		                alt_16  *** input_buffers_p,
		                alt_16  *** output_buffers_p,
#endif
                         alt_u16 number_of_buffers,
                         alt_u16 buffer_length)
{

  alt_u32 buffer_counter, contents_counter;



#ifndef LINEAR_TEST
  alt_u32 temp_data = 0x00100000;
  alt_u32 ** input_buffers  = (alt_u32 **) malloc(number_of_buffers * sizeof(alt_u32 *));
  alt_u32 ** output_buffers = (alt_u32 **) malloc(number_of_buffers * sizeof(alt_u32 *));
#else
  alt_16 temp_data = 1;
  alt_16 ** input_buffers  = (alt_16 **) malloc(number_of_buffers * sizeof(alt_16 *));
  alt_16 ** output_buffers = (alt_16 **) malloc(number_of_buffers * sizeof(alt_16 *));
#endif
  if ((input_buffers == NULL) || (output_buffers == NULL))
  {
    printf("Buffer allocation failed\n");
    return 1;
  }

  *input_buffers_p = input_buffers;
  *output_buffers_p = output_buffers;

  //  Initialisation of the buffer memories and the transmit+receive descriptors
  for(buffer_counter = 0; buffer_counter < number_of_buffers; buffer_counter++)
  {
#ifndef LINEAR_TEST
	    input_buffers[buffer_counter] = (alt_u32 *)malloc(buffer_length * sizeof(alt_u32));
#else
	    input_buffers[buffer_counter] = (alt_16 *)malloc(buffer_length * sizeof(alt_16));
#endif
    if(input_buffers[buffer_counter] == NULL)
    {
      printf("Allocating a transmit buffer region failed\n");
      return 1; 
    }

#ifndef LINEAR_TEST
    output_buffers[buffer_counter] = (alt_u32 *)malloc(buffer_length * sizeof(alt_u32));
#else
    output_buffers[buffer_counter] = (alt_16 *)malloc(buffer_length * sizeof(alt_16));
#endif
    if(output_buffers[buffer_counter] == NULL)
    {
      printf("Allocating a receive buffer region failed\n");
      return 1; 
    }

    // This will populate sequential data (modulo 256) in each transmit buffer a byte at a time.
    // The receive buffers are also cleared with zeros so that each time this software is run
    // the data can be reverified.
    

    for(contents_counter = 0; contents_counter < buffer_length; contents_counter++)
    {
#ifndef LINEAR_TEST
        input_buffers[buffer_counter][contents_counter] = (alt_32)(temp_data);
#else
        input_buffers[buffer_counter][contents_counter] = (alt_16)(temp_data & 0xFF);
#endif
      output_buffers[buffer_counter][contents_counter] = 0;
      temp_data++;
    }

  }
  
  return 0;  // no failures creating data buffers
}
