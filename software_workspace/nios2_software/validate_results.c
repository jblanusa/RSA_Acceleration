#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "alt_types.h"
#include "altera_avalon_sgdma.h"

#define VERBOSE_LEVEL 2

/**********************************************************************************
 * This function returning nothing since it's going to print errors to the        *
 * console. It checks content of output buffers and report any errors             *
 **********************************************************************************/
void validate_hw_results(alt_sgdma_descriptor * transmit_descriptors,
                         alt_sgdma_descriptor * receive_descriptors,
                         alt_u32 number_of_buffers,
                         alt_8 param_a,
                         alt_8 param_b)
{
  alt_u32 buffer_counter, contents_counter, temp_length, failure = 0;
  alt_16 * data_buffer;
  alt_16 * result_buffer;
  alt_16 temp_data;
  alt_16 temp_result, correct_result;

  /* loop through each descriptor */  
  for(buffer_counter = 0; buffer_counter < number_of_buffers; buffer_counter++)
  {
    data_buffer   = (alt_16 *)transmit_descriptors[buffer_counter].read_addr;
    result_buffer = (alt_16 *)receive_descriptors[buffer_counter].write_addr;
    temp_length = transmit_descriptors[buffer_counter].bytes_to_transfer / sizeof(alt_16);
    
#if VERBOSE_LEVEL>1
    printf("\nSent data: ");
    for(contents_counter = 0; contents_counter < temp_length; contents_counter++) printf("%d, ", data_buffer[contents_counter]);
    printf("\n");

    printf("\nReceived data: ");
    for(contents_counter = 0; contents_counter < temp_length; contents_counter++) printf("%d, ", result_buffer[contents_counter]);
    printf("\n");
#endif
    /* loop through each buffer to check the contents on each byte */
    for(contents_counter = 0; contents_counter < temp_length; contents_counter++)
    {
      temp_data = data_buffer[contents_counter];
      temp_result = result_buffer[contents_counter];

      correct_result = param_a*temp_data + param_b;

      if(temp_result != correct_result)
      {
#if VERBOSE_LEVEL>1
        printf("Error, sent %d, received %d, correct %d, buffer: %d, index: %d\n", (int)temp_data, (int)temp_result, (int) correct_result, (int)(buffer_counter+1), (int)contents_counter);
#endif
        failure = 1;
      }

    }
    
  }

  if(failure == 0)
  {
#if VERBOSE_LEVEL>0
    printf("The entire data transfer finished error free.\n");
#endif
  }

}
