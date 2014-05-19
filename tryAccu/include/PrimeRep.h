/*
 * PrimeRep.h
 *
 * Prime Representative for arbitrary values.
 *
 * 	h(x) = ((a*x+b) mod p) >> 2^{2k} is the random hash function for mapping
 *  3k bits value x to k bits. Thus, the reverse is:
 *        x = (2^{2k} * y + c - b) * a^{-1} mod p,
 *  where c is a random constant.
 *
 *  Created on: Mar 18, 2013
 *      Author: jzhou
 */

#ifndef PRIMEREP_H_
#define PRIMEREP_H_

#include <set>
#include <vector>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <stdint.h>
NTL_CLIENT

// compute SHA1 digest of input data and return digest value in ZZ
ZZ shaToZZ(const uint8_t* data, int len);

class PrimeRep {
public:
	PrimeRep(long k);
	PrimeRep(long _k, ZZ _a, ZZ _b, ZZ _c, ZZ _prime, ZZ _ar): k(_k), a(_a),
			b(_b), c(_c), prime(_prime), ar(_ar) {}
	PrimeRep(PrimeRep &p) : k(p.k), a(p.a), b(p.b), c(p.c), prime(p.prime),
			ar(p.ar) { }

	// return prime representative for value "v"
	ZZ getPrime(ZZ v);
	ZZ getPrime(int v) {
		ZZ t;
		t = v;
		return getPrime(t);
	}

	// return prime representatives for a set of values
	std::set<ZZ> getPrime(std::set<ZZ>  &values);
	std::set<ZZ> getPrime(std::set<int> &values);

	// the vector version, output[i] = PrimeRepresentative(input[i])
	std::vector<ZZ> getPrime(std::vector<ZZ>  &values);

	// verify prime is generated from input data
	bool verifyPrime(const uint8_t* data, int len, ZZ prime);

	void print();

private:
	long k; // parameter k
	ZZ a, b, c, prime;
	ZZ ar; // 1/a under modulo p
};

// default 32-bit and 64-bit PrimeRep parameters
extern ZZ a32, b32, c32, ar32, prime32;
extern ZZ a64, b64, c64, ar64, prime64;
extern void init_prime_rep_32_64();

#endif /* PRIMEREP_H_ */
