/*
 * test.c
 *
 *  Created on: Jul 28, 2016
 *      Author: Korisnik
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/alt_irq.h>

#include <system.h>
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#include "rsa_crypto.h"
#include "rsa_hardware.h"
#include "hardware_setup.h"

#define BUF_SIZE (50)
#define NUM_OF_BYTES 7

#define NUMBER_OF_BUFFERS 1
#define BUFFER_LENGTH 2176
#define BITSIZE 64

extern volatile alt_u16 milis;
extern volatile alt_u16 tx_done;
extern volatile alt_u16 rx_done;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CryptoTest1(){
	  printf("Hello from Nios II!\n");

	  FILE* report = fopen ("/mnt/host/files/report64.txt", "w");

	  // p = 60637, q = 47639
	  // fi = 288577768
	  unsigned int t1 = milis;
	  uint64_t p = RandomPrime(32);//7529;
	  uint64_t q = RandomPrime(32);//191237;
	  t1 = milis - t1;

	  printf("Vreme generisanja random prostih brojeva %d ms\n", t1);
	  fprintf(report, "Vreme generisanja random prostih brojeva %d ms\n", t1);

	  printf("Prosti brojevi su: p = %llu, q = %llu\n",p ,q);
	  fprintf(report,"Prosti brojevi su: p = %llu, q = %llu\n",p ,q);

	  uint64_t n = p*q;
	  uint64_t fi = (p-1)*(q-1);
	  uint64_t e = 65537;
	  uint64_t d = ExpandedEuclid(fi,e);

	  printf("n = %llu, fi = %llu\n",n,fi);
	  fprintf(report,"n = %llu, fi = %llu\n",n,fi);
	  printf("e = %llu, d = %llu\n",e,d);
	  fprintf(report,"e = %llu, d = %llu\n",e,d);

	  uint64_t i,C,M;
	  t1 = milis;
	  for(i = 0xABCD; i < 0xABCD + 100; i++)
	  {
		  C = ModularExp(i,e,n);
		  M = ModularExp(C,d,n);
		  printf("Original I = %llX, Sifrovano C = %llX, Desifrovano D = %llX\n", i,C, M);
		  fprintf(report,"Original I = %llX, Sifrovano C = %llX, Desifrovano D = %llX\n", i,C, M);
	  }
	  t1 = milis - t1;

	  printf("Vreme trajanja sifrovanja i desifrovanja %d ms\n", t1);
	  fprintf(report, "Vreme trajanja sifrovanja i desifrovanja %d ms\n", t1);

	  t1 = milis;
	  for(i = 0xABCD; i < 0xABCD + 100; i++)
	  {
		  C = ModularExp(i,e,n);
		  M = ModularExp(C,d,n);
	  }
	  t1 = milis - t1;

	  printf("Vreme trajanja sifrovanja i desifrovanja bez ispisa %d ms\n", t1);
	  fprintf(report, "Vreme trajanja sifrovanja i desifrovanja bez ispisa %d ms\n", t1);
	  fclose(report);
}

void CryptoTest2(){
	  printf("Hello from Nios II!\n");

	  FILE* report = fopen ("/mnt/host/files/reportTxt64.txt", "w");

	  unsigned int t1 = milis;
	  uint64_t p = RandomPrime(31);//7529;
	  uint64_t q = RandomPrime(31);//191237;
	  t1 = milis - t1;

	  printf("Vreme generisanja random prostih brojeva %d ms\n", t1);
	  fprintf(report, "Vreme generisanja random prostih brojeva %d ms\n", t1);

	  uint64_t n = p*q;
	  uint64_t fi = (p-1)*(q-1);
	  uint64_t e = 65537;
	  uint64_t d = ExpandedEuclid(fi,e);

	  printf("Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  fprintf(report,"Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  printf("n = %llu, fi = %llu\n",n,fi);
	  fprintf(report,"n = %llu, fi = %llu\n",n,fi);
	  printf("e = %llu, d = %llu\n",e,d);
	  fprintf(report,"e = %llu, d = %llu\n",e,d);

	  FILE* Mstream = fopen ("/mnt/host/files/no_encrypt.txt", "r");
	  FILE* Cstream = fopen ("/mnt/host/files/encrypt.txt", "w");
	  FILE* Dstream = fopen ("/mnt/host/files/decrypt.txt", "w");

	  uint64_t M = 0, C = 0, D = 0;

	  t1 = milis;
	  while(!feof(Mstream))
	  {
		 fread(&M, NUM_OF_BYTES, 1, Mstream);
		 C = ModularExp(M, e, n);
		 fwrite(&C, NUM_OF_BYTES, 1, Cstream);
		 D = ModularExp(C, d, n);
		 fwrite(&D, NUM_OF_BYTES, 1, Dstream);
//		 fprintf(report,"Original I = %llX, Sifrovano C = %llX, Desifrovano D = %llX\n", M, C, D);
	  }
	  t1 = milis - t1;

	  fclose(Mstream);
	  fclose(Cstream);
	  fclose(Dstream);

	  printf("Vreme trajanja sifrovanja i desifrovanja %d ms\n", t1);
	  fprintf(report,"Vreme trajanja sifrovanja i desifrovanja %d ms\n", t1);

	  fclose(report);
}

void CryptoTest3(){
	  printf("Hello from Nios II!\n");

	  FILE* report = fopen ("/mnt/host/files/reportHw64.txt", "w");

	  unsigned int t1 = milis;
	  uint64_t p = RandomPrime(31);//7529;
	  uint64_t q = RandomPrime(31);//191237;
	  t1 = milis - t1;

	  printf("Vreme generisanja random prostih brojeva %d ms\n", t1);
	  fprintf(report, "Vreme generisanja random prostih brojeva %d ms\n", t1);

	  uint64_t n = p*q;
	  uint64_t fi = (p-1)*(q-1);
	  uint64_t e = 65537;
	  uint64_t d = ExpandedEuclid(fi,e);

	  printf("Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  fprintf(report,"Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  printf("n = %llu, fi = %llu\n",n,fi);
	  fprintf(report,"n = %llu, fi = %llu\n",n,fi);
	  printf("e = %llu, d = %llu\n",e,d);
	  fprintf(report,"e = %llu, d = %llu\n",e,d);

	  ////////////////////////////////////////////
	  alt_sgdma_dev *transmit_DMA, *receive_DMA;
	  init_sgdma(&transmit_DMA, &receive_DMA);

	  alt_sgdma_descriptor *transmit_descriptors, *transmit_descriptors_copy;
	  alt_sgdma_descriptor *receive_descriptors, *receive_descriptors_copy;

	  alt_u32 return_code, i;

	  alt_u32 publicExp64[] = {(alt_u32)e, (alt_u32)(e >> 32)};
	  alt_u32 privateExp64[] = {(alt_u32)d, (alt_u32)(d >> 32)};
	  alt_u32 mod64[] = {(alt_u32)n, (alt_u32)(n >> 32)};

	  alt_u32 ** result_buffers;
	  alt_u32 ** data_buffers;

	  return_code = create_test_data(&data_buffers,
									 &result_buffers,
									 NUMBER_OF_BUFFERS,
									 BUFFER_LENGTH);

	  return_code = create_descriptors(&transmit_descriptors,
	                                   &transmit_descriptors_copy,
	                                   &receive_descriptors,
									   &receive_descriptors_copy,
									   data_buffers,
									   result_buffers,
									   BUFFER_LENGTH,
									   NUMBER_OF_BUFFERS);

		t1 = milis;
		return_code = rsa_crypto_hw(transmit_DMA,
									  transmit_descriptors,
									  &tx_done,
									  receive_DMA,
									  receive_descriptors,
									  &rx_done,
									  publicExp64,
									  mod64,
									  BITSIZE);

		t1 = milis - t1;
		printf("Vreme sifrovanja je %d ms\n", t1);
		fprintf(report,"Vreme sifrovanja je %d ms\n", t1);

/*	    for(i = 0; i < BUFFER_LENGTH; i += 2){
	    	printf("Original I = %lX%lX, Sifrovano C = %lX%lX\n",(*data_buffers)[i+1],(*data_buffers)[i],
	    														 (*result_buffers)[i+1],(*result_buffers)[i]);
	    	fprintf(report,"Original I = %lX%lX, Sifrovano C = %lX%lX\n",(*data_buffers)[i+1],(*data_buffers)[i],
	    														 (*result_buffers)[i+1],(*result_buffers)[i]);
	    }
*/

	  free(transmit_descriptors_copy);
	  free(receive_descriptors_copy);
	  free(data_buffers[i]);
	  data_buffers[i] = (alt_u32 *)malloc(BUFFER_LENGTH * sizeof(alt_u32));

	  return_code = create_descriptors(&transmit_descriptors,
	                                   &transmit_descriptors_copy,
	                                   &receive_descriptors,
									   &receive_descriptors_copy,
									   result_buffers,
									   data_buffers,
									   BUFFER_LENGTH,
									   NUMBER_OF_BUFFERS);
		t1 = milis;
		return_code = rsa_crypto_hw(transmit_DMA,
									  transmit_descriptors,
									  &tx_done,
									  receive_DMA,
									  receive_descriptors,
									  &rx_done,
									  privateExp64,
									  mod64,
									  BITSIZE);

		t1 = milis - t1;
		printf("Vreme desifrovanja je %d ms\n", t1);
		fprintf(report,"Vreme desifrovanja je %d ms\n", t1);

	 /*   for(i = 0; i < BUFFER_LENGTH; i += 2){
	    	printf("Sifrovano C = %lX%lX, Desifrovano D = %lX%lX\n", (*result_buffers)[i+1],(*result_buffers)[i],
	    																(*data_buffers)[i+1],(*data_buffers)[i]);
	    	fprintf(report,"Sifrovano C = %lX%lX, Desifrovano D = %lX%lX\n", (*result_buffers)[i+1],(*result_buffers)[i],
																			(*data_buffers)[i+1],(*data_buffers)[i]);
	    }*/

	      for (i = 0; i < NUMBER_OF_BUFFERS; i++)
	      {
	    	  free(data_buffers[i]);
	    	  free(result_buffers[i]);
	      }

	      free(data_buffers);
	      free(result_buffers);

	      free(transmit_descriptors_copy);
	      free(receive_descriptors_copy);

	      fclose(report);
}

void CryptoTest4(){
	  printf("Hello from Nios II!\n");

	  FILE* report = fopen ("/mnt/host/files/reportHw128.txt", "w");

	  unsigned int t1 = milis;
	  uint64_t p = RandomPrime(31);//7529;
	  uint64_t q = RandomPrime(31);//191237;
	  t1 = milis - t1;

	  printf("Vreme generisanja random prostih brojeva %d ms\n", t1);
	  fprintf(report, "Vreme generisanja random prostih brojeva %d ms\n", t1);

	  uint64_t n = p*q;
	  uint64_t fi = (p-1)*(q-1);
	  uint64_t e = 65537;
	  uint64_t d = ExpandedEuclid(fi,e);

	  printf("Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  fprintf(report,"Prosti brojevi su: p = %llu, q = %llu\n",p,q);
	  printf("n = %llu, fi = %llu\n",n,fi);
	  fprintf(report,"n = %llu, fi = %llu\n",n,fi);
	  printf("e = %llu, d = %llu\n",e,d);
	  fprintf(report,"e = %llu, d = %llu\n",e,d);

	  FILE* Mstream = fopen ("/mnt/host/files/no_encrypt.txt", "r");
	  FILE* Cstream = fopen ("/mnt/host/files/encrypt.txt", "w");
	  FILE* Dstream = fopen ("/mnt/host/files/decrypt.txt", "w");

	  ////////////////////////////////////////////
	  alt_sgdma_dev *transmit_DMA, *receive_DMA;
	  init_sgdma(&transmit_DMA, &receive_DMA);

	  alt_sgdma_descriptor *transmit_descriptors, *transmit_descriptors_copy;
	  alt_sgdma_descriptor *receive_descriptors, *receive_descriptors_copy;

	  alt_u32 return_code, i;

	  alt_u32 publicExp64[] = {(alt_u32)e, (alt_u32)(e >> 32)};
	  alt_u32 privateExp64[] = {(alt_u32)d, (alt_u32)(d >> 32)};
	  alt_u32 mod64[] = {(alt_u32)n, (alt_u32)(n >> 32)};

	  //**************************************************************************
	  // 128bit

	  //p = 12,048,282,231,162,725,293
	  //q = 14,815,918,891,228,532,449

	  //e = 65537
/*	  alt_u32 publicExp64[] = {(alt_u32)e, (alt_u32)(e >> 32), (alt_u32)0, (alt_u32)0};

	  // d = 134779297515750051653495347030693638017, 0x65658c6a4b10e38e682b2373f6fdef81
	  alt_u32 privateExp64[] = {0xf6fdef81, 0x682b2373, 0x4b10e38e, 0x65658c6a};

	  // fi = 178506372315536873981268810669332274816
	  // n = 178,506,372,315,536,874,008,133,011,791,723,532,557, 0x864b13a73da89cd22ca0a86fedfe910d
	  alt_u32 mod64[] = {0xedfe910d, 0x2ca0a86f, 0x3da89cd2, 0x864b13a7};*/

	  //**************************************************************************
	  // 512bit

	  //e = 65537
/*	  alt_u32 publicExp64[16] = {(alt_u32)e, (alt_u32)(e >> 32), (alt_u32)0};

	  // d = 0x2a3ff2f501835d7d0f7ccad34265525b0c91f800fc252324e4bb5668392ba55a65d812d658b7065725c259a12115918dbdbbea6cead9225a91c3fcc6b43f1cb3
	  alt_u32 privateExp64[] = {0xb43f1cb3 ,0x91c3fcc6, 0xead9225a, 0xbdbbea6c, 0x2115918d,
			  	    0x25c259a1, 0x58b70657, 0x65d812d6, 0x392ba55a, 0xe4bb5668, 0xfc252324,
			  	    0x0c91f800, 0x4265525b, 0x0f7ccad3, 0x01835d7d, 0x2a3ff2f5};

	  // n = 0x4bb26cf2e1e1cdbbf78f89a60d5af5cf094a7216f9fe2d0e2288dc14e6005e0b6de46dc38f0e92b8b3281e9f1cf87257fb54bff579907e8a3c114133b74cb815
	  alt_u32 mod64[] = { 0xb74cb815, 0x3c114133, 0x79907e8a, 0xfb54bff5, 0x1cf87257, 0xb3281e9f,
			  0x8f0e92b8, 0x6de46dc3, 0xe6005e0b, 0x2288dc14, 0xf9fe2d0e, 0x094a7216, 0x0d5af5cf,
			  0xf78f89a6, 0xe1e1cdbb, 0x4bb26cf2};*/


	  //**************************************************************************
	  // 256bit

	  //p = 139,384,400,564,462,624,965,799,956,788,057,106,141
	  //q = 158,133,121,933,283,802,002,136,232,807,470,209,477

	  //e = 65537
/*	  alt_u32 publicExp64[8] = {(alt_u32)e, (alt_u32)(e >> 32), (alt_u32)0, (alt_u32)0, (alt_u32)0, (alt_u32)0, (alt_u32)0, (alt_u32)0};

	  // d = 0x54a5600 5b649f9a eb7fbbb7 da0c1590 cfe9ee86 8a28b699 afdbaea8 b864bcd1
	  alt_u32 privateExp64[] = {0xb864bcd1, 0xafdbaea8, 0x8a28b699, 0xcfe9ee86, 0xda0c1590, 0xeb7fbbb7, 0x5b649f9a, 0x054a5600};

	  // fi = 22041290410057839872284847672877883606469376347253327531527884109916165782640
	  // n = 22,041,290,410,057,839,872,284,847,672,877,883,606,766,893,869,751,073,958,495,820,299,511,693,098,257
	  // 0x30baeda3 94ab84a4 9d1d1475 3727d0f9 3118ff1c 4508d1f4 722eeb43 d200bd11
	  alt_u32 mod64[] = {0xd200bd11, 0x722eeb43, 0x4508d1f4, 0x3118ff1c, 0x3727d0f9, 0x9d1d1475, 0x94ab84a4, 0x30baeda3};*/

	  alt_u32 ** result_buffers;
	  alt_u32 ** data_buffers;

	  data_buffers  = (alt_u32 **) malloc(sizeof(alt_u32 *));
	  result_buffers = (alt_u32 **) malloc(sizeof(alt_u32 *));

	  data_buffers[0] = (alt_u32 *)malloc(BUFFER_LENGTH * sizeof(alt_u32));
	  result_buffers[0] = (alt_u32 *)malloc(BUFFER_LENGTH * sizeof(alt_u32));

	  for(i = 0; i < BUFFER_LENGTH; i++){
		  fread(data_buffers[0]+i, 3, 1, Mstream);
	  }

	  fclose(Mstream);
	/*  return_code = create_test_data(&data_buffers,
									 &result_buffers,
									 NUMBER_OF_BUFFERS,
									 BUFFER_LENGTH);*/

	  return_code = create_descriptors(&transmit_descriptors,
	                                   &transmit_descriptors_copy,
	                                   &receive_descriptors,
									   &receive_descriptors_copy,
									   data_buffers,
									   result_buffers,
									   BUFFER_LENGTH,
									   NUMBER_OF_BUFFERS);

		t1 = milis;
		return_code = rsa_crypto_hw(transmit_DMA,
									  transmit_descriptors,
									  &tx_done,
									  receive_DMA,
									  receive_descriptors,
									  &rx_done,
									  publicExp64,
									  mod64,
									  BITSIZE);

		t1 = milis - t1;
		printf("Vreme sifrovanja je %d ms\n", t1);
		fprintf(report,"Vreme sifrovanja je %d ms\n", t1);

		for(i = 0; i < BUFFER_LENGTH; i++){
			fwrite(result_buffers[0]+i, 3, 1, Cstream);
		}

		  fclose(Cstream);

	    for(i = 0; i < BUFFER_LENGTH; i += 4){
	    	printf("Original I = %lX%lX%lX%lX, Sifrovano C = %lX%lX%lX%lX\n",(*data_buffers)[i+3],(*data_buffers)[i+2],(*data_buffers)[i+1],(*data_buffers)[i],
	    			(*result_buffers)[i+3],(*result_buffers)[i+2],(*result_buffers)[i+1],(*result_buffers)[i]);
	    	fprintf(report,"Original I = %lX%lX%lX%lX, Sifrovano C = %lX%lX%lX%lX\n",(*data_buffers)[i+3],(*data_buffers)[i+2],(*data_buffers)[i+1],(*data_buffers)[i],
	    			(*result_buffers)[i+3],(*result_buffers)[i+2],(*result_buffers)[i+1],(*result_buffers)[i]);
	    }


	  free(transmit_descriptors_copy);
	  free(receive_descriptors_copy);
	  free(data_buffers[i]);
	  data_buffers[i] = (alt_u32 *)malloc(BUFFER_LENGTH * sizeof(alt_u32));

	  return_code = create_descriptors(&transmit_descriptors,
	                                   &transmit_descriptors_copy,
	                                   &receive_descriptors,
									   &receive_descriptors_copy,
									   result_buffers,
									   data_buffers,
									   BUFFER_LENGTH,
									   NUMBER_OF_BUFFERS);
		t1 = milis;
		return_code = rsa_crypto_hw(transmit_DMA,
									  transmit_descriptors,
									  &tx_done,
									  receive_DMA,
									  receive_descriptors,
									  &rx_done,
									  privateExp64,
									  mod64,
									  BITSIZE);

		t1 = milis - t1;
		printf("Vreme desifrovanja je %d ms\n", t1);
		fprintf(report,"Vreme desifrovanja je %d ms\n", t1);

		for(i = 0; i < BUFFER_LENGTH; i++){
			fwrite(data_buffers[0]+i, 3, 1, Dstream);
		}

		  fclose(Dstream);

	    for(i = 0; i < BUFFER_LENGTH; i += 4){
	    	printf("Sifrovano C = %lX%lX%lX%lX, Desifrovano D = %lX%lX%lX%lX\n", (*result_buffers)[i+3],(*result_buffers)[i+2],(*result_buffers)[i+1],(*result_buffers)[i],
	    			(*data_buffers)[i+3],(*data_buffers)[i+2],(*data_buffers)[i+1],(*data_buffers)[i]);
	    	fprintf(report,"Sifrovano C = %lX%lX%lX%lX, Desifrovano D = %lX%lX%lX%lX\n", (*result_buffers)[i+3],(*result_buffers)[i+2],(*result_buffers)[i+1],(*result_buffers)[i],
	    			(*data_buffers)[i+3],(*data_buffers)[i+2],(*data_buffers)[i+1],(*data_buffers)[i]);
	    }

	      for (i = 0; i < NUMBER_OF_BUFFERS; i++)
	      {
	    	  free(data_buffers[i]);
	    	  free(result_buffers[i]);
	      }

	      free(data_buffers);
	      free(result_buffers);

	      free(transmit_descriptors_copy);
	      free(receive_descriptors_copy);


		  fclose(report);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LINEAR_TEST
alt_u32 ** result_buffers;
alt_u32 ** data_buffers;
#else
alt_16 ** result_buffers;
alt_16  ** data_buffers;
#endif

#ifndef LINEAR_TEST
void RSAhardwareTest(){
	  alt_sgdma_dev *transmit_DMA, *receive_DMA;
	  init_sgdma(&transmit_DMA, &receive_DMA);

	  alt_sgdma_descriptor *transmit_descriptors, *transmit_descriptors_copy;
	  alt_sgdma_descriptor *receive_descriptors, *receive_descriptors_copy;

	  alt_u32 return_code, i;

//	  alt_32 ** result_buffers;
//	  alt_32 ** data_buffers;

	  // p = 60637, q = 47639
	  // fi = 288577768
	  alt_u32 modul = 2888686043u; 	    // n
	  alt_u32 public_exp  = 65537u;     // e
	  alt_u32 private_exp = 963138401u; // d

	  alt_u32 publicExp64[] = {0x2A153C0B, 0x20FD8796};
	  alt_u32 privateExp64[] = {0xB6A3C723, 0x7456DEAF};
	  alt_u32 mod64[] = {0xA73A1CB3, 0x9DD0D2DD};
	  /**************************************************************
	     * Allocating data buffers and populating them with data    *
	     ************************************************************/
	    return_code = create_test_data(&data_buffers,
	                                   &result_buffers,
	                                   NUMBER_OF_BUFFERS,
	                                   BUFFER_LENGTH);
	    if(return_code == 1)
	    {
	      printf("Allocating the data buffers failed... exiting\n");
	      return;
	    }


	  /**************************************************************
	     * Allocating descriptor table space from main memory.      *
	     * Pointers are passed by reference since they will be      *
	     * modified by this function.                               *
	     ************************************************************/
	    return_code = create_descriptors(&transmit_descriptors,
	                                  	 &transmit_descriptors_copy,
	                                  	 &receive_descriptors,
										 &receive_descriptors_copy,
										 data_buffers,
										 result_buffers,
										 BUFFER_LENGTH,
										 NUMBER_OF_BUFFERS);

	    if(return_code == 1)
	    {
	      printf("Allocating the descriptor memory failed... exiting\n");
	      return;
	    }

	    /***************************************************/
	    // RSA kriptovanje
	    /***************************************************/


		unsigned int t1 = milis;
	    return_code = rsa_crypto_hw(transmit_DMA,
							  transmit_descriptors,
							  &tx_done,
							  receive_DMA,
							  receive_descriptors,
							  &rx_done,
							  publicExp64,
							  mod64,
							  BITSIZE);

		unsigned int t2 = milis - t1;
		printf("Vreme je %d ms\n", t2);

	    if(return_code == 1)
	    {
	      printf("RSA crypto hardware processing failed... exiting\n");
	      return;
	    }
	    printf("RSA crypto hardware done\n");

	    for(i = 0; i < BUFFER_LENGTH; i += 2){
	    	printf("Original I = %lX%lX, Sifrovano C = %lX%lX\n",
	    			(*data_buffers)[i+1],(*data_buffers)[i],
	    			(*result_buffers)[i+1],(*result_buffers)[i]);
	    }

	      free(transmit_descriptors_copy);
	      free(receive_descriptors_copy);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	      // DEKRIPCIJA


		  /**************************************************************
		     * Allocating descriptor table space from main memory.      *
		     * Pointers are passed by reference since they will be      *
		     * modified by this function.                               *
		     ************************************************************/
		    return_code = create_descriptors(&transmit_descriptors,
		                                  	 &transmit_descriptors_copy,
		                                  	 &receive_descriptors,
											 &receive_descriptors_copy,
											 result_buffers,
											 data_buffers,
											 BUFFER_LENGTH,
											 NUMBER_OF_BUFFERS);
			t1 = milis;
		    return_code = rsa_crypto_hw(transmit_DMA,
								  transmit_descriptors,
								  &tx_done,
								  receive_DMA,
								  receive_descriptors,
								  &rx_done,
								  privateExp64,
								  mod64,
								  BITSIZE);

			t2 = milis - t1;
			printf("Vreme je %d ms\n", t2);

		    for(i = 0; i < BUFFER_LENGTH; i += 2){
		    	printf("Original I = %lX%lX, Sifrovano C = %lX%lX\n",
		    			(*result_buffers)[i+1],(*result_buffers)[i],
		    			(*data_buffers)[i+1],(*data_buffers)[i]);
		    }

		    if(return_code == 1)
		    {
		      printf("Allocating the descriptor memory failed... exiting\n");
		      return;
		    }
	    /**************************************************************
	       * Free allocated memory buffers.						      *
	       ************************************************************/
	      for (i = 0; i < NUMBER_OF_BUFFERS; i++)
	      {
	    	  free(data_buffers[i]);
	    	  free(result_buffers[i]);
	      }

	      free(data_buffers);
	      free(result_buffers);

	      free(transmit_descriptors_copy);
	      free(receive_descriptors_copy);
	    /**************************************************************/
}
#else
void LinearTest(){
	  alt_sgdma_dev *transmit_DMA, *receive_DMA;
	  init_sgdma(&transmit_DMA, &receive_DMA);

	  alt_sgdma_descriptor *transmit_descriptors, *transmit_descriptors_copy;
	  alt_sgdma_descriptor *receive_descriptors, *receive_descriptors_copy;

	  alt_u32 return_code, i;

	  alt_8 param_a = 0x02;
	  alt_8 param_b = 0;
	/**************************************************************
	   * Allocating data buffers and populating them with data    *
	   ************************************************************/
	    return_code = create_test_data(&data_buffers,
	                                   &result_buffers,
	                                   NUMBER_OF_BUFFERS,
	                                   BUFFER_LENGTH);
	  if(return_code == 1)
	  {
	    printf("Allocating the data buffers failed... exiting\n");
	    return;
	  }


	/**************************************************************
	   * Allocating descriptor table space from main memory.      *
	   * Pointers are passed by reference since they will be      *
	   * modified by this function.                               *
	   ************************************************************/
	    return_code = create_descriptors(&transmit_descriptors,
	                                  	 &transmit_descriptors_copy,
	                                  	 &receive_descriptors,
										 &receive_descriptors_copy,
										 data_buffers,
										 result_buffers,
										 BUFFER_LENGTH,
										 NUMBER_OF_BUFFERS);
	  if(return_code == 1)
	  {
	    printf("Allocating the descriptor memory failed... exiting\n");
	    return;
	  }
	/**************************************************************/

	/***************************************************/
	// Linearna Funkcija
	/***************************************************/

	return_code = linear_function_hw(transmit_DMA,
									 transmit_descriptors,
									 &tx_done,
									 receive_DMA,
									 receive_descriptors,
									 &rx_done,
									 param_a,
									 param_b);

    if(return_code == 1)
    {
      printf("Linear function hardware processing failed... exiting\n");
      return;
    }
    printf("Linear function hardware done\n");

    for(i = 0; i < BUFFER_LENGTH; i++){
    	printf("Ulaz I = %hX, Izlaz C = %hX\n", (*data_buffers)[i], (*result_buffers)[i]);
    }

    validate_hw_results(transmit_descriptors,
    		            receive_descriptors,
    		            NUMBER_OF_BUFFERS,
    		            param_a,
    		            param_b);

	/**************************************************************
	   * Free allocated memory buffers.						      *
	   ************************************************************/
	  for (i = 0; i < NUMBER_OF_BUFFERS; i++)
	  {
		  free(data_buffers[i]);
		  free(result_buffers[i]);
	  }

	  free(data_buffers);
	  free(result_buffers);

	  free(transmit_descriptors_copy);
	  free(receive_descriptors_copy);
	/**************************************************************/
}
#endif


