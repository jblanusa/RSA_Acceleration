/*
 * rsa_crypto.h
 *
 *  Created on: Jun 26, 2016
 *      Author: Korisnik
 */

#ifndef RSA_CRYPTO_H_
#define RSA_CRYPTO_H_

#include <stdint.h>

uint64_t ExpandedEuclid(uint64_t fi, uint64_t e);
uint64_t ModularExp(uint64_t m, uint64_t e, uint64_t n);
uint64_t xorshift64star();
uint64_t ModMult(uint64_t mpand, uint64_t mplier, uint64_t modulus);
int MillerRabin(uint64_t n, int t);
uint64_t RandomPrime(int len);

#endif /* RSA_CRYPTO_H_ */
