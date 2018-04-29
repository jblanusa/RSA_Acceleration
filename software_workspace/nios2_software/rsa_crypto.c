/*
 * rsa_crypto.c
 *
 *  Created on: Jun 26, 2016
 *      Author: Korisnik
 */
#include "rsa_crypto.h"

#define TRIAL_DIV_BOUND 256
#define SECURITY_PARAM 1

// Odredjivanje inverznog elementa po modulu ed = 1(mod fi)
uint64_t ExpandedEuclid(uint64_t fi, uint64_t e)
{
	int64_t y = 1, py;
	int64_t lastY = 0;
	uint64_t kol, p;
	uint64_t a = fi;
	uint64_t b = e;

	while(b != 0)
	{
		kol = a/b;
		p = a % b;
		a = b;
		b = p;

		py = lastY - (int64_t)kol*y;
		lastY = y;
		y = py;
	}

	if(lastY < 0)
	{
		lastY = fi + lastY;
	}

	return lastY;
}

// Modularno mnozenje
uint64_t ModMult(uint64_t mpand, uint64_t mplier, uint64_t modulus)
{
	uint64_t result = 0;
	uint64_t mask = 0x1;
	while(mplier)
	{
		if(mplier & mask)
		{
			result += mpand;
	/*		if(result >= 2*modulus)
			{
				result -= 2*modulus;
			}
			else if(result >= modulus)
			{
				result -= modulus;
			}*/
			if(result >= modulus)
			{
				result -= modulus;
				if(result >= modulus)
				{
					result -= modulus;
				}
			}
		}

		mplier >>= 1;
		mpand  <<= 1;

		if(mpand > modulus)
		{
			mpand -= modulus;
		}
	}
	return result;
}

// Modularni eksponent
uint64_t ModularExp(uint64_t a, uint64_t k, uint64_t n)
{
	uint64_t b = 1;
	if(k == 0) return b;
	uint64_t A = a;

	uint64_t mask = 0x1;
	if(mask & k) b = a;
	k >>= 1;

	while(k)
	{
		//A = (A * A) % n;
		A = ModMult(A,A,n);
		if(mask & k)
			//b = (A * b) % n;
			b = ModMult(A,b,n);
		k >>= 1;
	}
	return b;
}


// Moze hardverski lagano, jos i clk pa da se stalno menja
uint64_t xorshift64star()
{
	static uint64_t x = UINT64_C(1181783497276652981);
	x ^= x >> 12; // a
	x ^= x << 25; // b
	x ^= x >> 27; // c
	return x * UINT64_C(2685821657736338717);
}

// Testira da li je broj prost
int MillerRabin(uint64_t n, int t)
{
	uint64_t s = 0;
	uint64_t r = n - 1;

	// n-1 = r 2^s
	while(!(r & 1))
	{
		s++;
		r >>= 1;
	}

	int i,j;
	uint64_t a,y;
	for(i = 0; i < t; i++)
	{
		a = ((uint64_t)xorshift64star()) % (n-4) + 2;
		y = ModularExp(a,r,n);
		if((y != 1) && (y != n-1))
		{
			j = 1;
			while((j <= s-1) && y != n-1)
			{
				y = (y * y)% n;
				// y = ModMult(y,y,n);
				if(y == (uint64_t)1) return 0;
				j++;
			}
			if(y != n-1) return 0;
		}
	}
	return 1;
}

//Generise random prost broj
uint64_t RandomPrime(int len)
{
	while(1){
		uint64_t n = xorshift64star();
		n |= (UINT64_C(1)<<63);
		n >>= (64 - len);
		n |= 1;	// Postaviti bit najnize tezine na 1, neparan broj


		int i;
		for(i = 3; i < TRIAL_DIV_BOUND; i += 2){
			if(n % i == 0) continue;
		}

		if(MillerRabin(n,SECURITY_PARAM)) return n;
	}
	return 0;
}
