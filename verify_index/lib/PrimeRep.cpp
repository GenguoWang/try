/*
 * PrimeRep.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: jzhou
 */

#include "PrimeRep.h"
#include <openssl/sha.h>
#include "util.h"

PrimeRep::PrimeRep(long kl)
{
	k = kl;
	prime = GenPrime_ZZ(3 * k + 1, 80);

	a = RandomLen_ZZ(k);
	b = RandomLen_ZZ(k);
	c = RandomLen_ZZ(k);

	ar = InvMod(a, prime); // = 1/a mod p
}

void
PrimeRep::print()
{
	cout << "a = " << a << "\nb = " << b << "\nc = " << c << "\nar = " << ar
		<< "\nprime = " << prime << endl;
}

ZZ
PrimeRep::getPrime(ZZ v)
{
	ZZ x;

	for (int i = 0; i < 2 * k * k; i++) {
		MulMod(x, LeftShift(v, 2 * k) + c - b + i, ar, prime);
		// x may be larger than 3k bits, make at most 3k bits
		if (NumBits(x) > 3 * k) {
			x = RightShift(x, NumBits(x)-3*k);
			//if (!IsOdd(x)) x = MakeOdd(x);
		}
		if (ProbPrime(x)) {
			//cout << "found a prime: " << x << "\n" << NumBits(x)
			//		<< " bits i = " << i << endl;
			return x;
		}
	}
	alert("no prime found");
	exit(-1);
	return x; // not reached
}

std::set<ZZ>
PrimeRep::getPrime(std::set<ZZ> &values)
{
	std::set<ZZ> primes;
	for (std::set<ZZ>::iterator it=values.begin(); it != values.end(); it++) {
		primes.insert(getPrime(*it));
	}
	return primes;
}

std::set<ZZ>
PrimeRep::getPrime(std::set<int> &values)
{
	std::set<ZZ> primes;
	for (std::set<int>::iterator it=values.begin(); it != values.end(); it++) {
		ZZ v;
		v = *it;
		primes.insert(getPrime(v));
	}
	return primes;
}

std::vector<ZZ>
PrimeRep::getPrime(std::vector<ZZ>  &values)
{
	std::vector<ZZ> primes;
	for (std::vector<ZZ>::iterator it=values.begin(); it != values.end(); it++) {
		primes.push_back(getPrime(*it));
	}
	return primes;
}

std::vector<ZZ>
PrimeRep::getPrime(std::vector<int> &values)
{
	std::vector<ZZ> primes;
	for (int i = 0; i < values.size(); i++) {
		ZZ v;
		v = values[i];
		primes.push_back(getPrime(v));
	}
	return primes;
}

bool
PrimeRep::verifyPrime(const uint8_t* data, int len, ZZ prime)
{
	ZZ hash = shaToZZ(data, len);
	ZZ p = getPrime(hash);
	if (p == prime)
		return true;
	return false;
}

// compute SHA1 digest of input data and return digest value in ZZ
ZZ shaToZZ(const uint8_t* data, int len)
{
	unsigned char shaDigest[SHA_DIGEST_LENGTH];
	SHA1(data, len, shaDigest);

	ZZ v;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		int t = shaDigest[i];
		v = LeftShift(v, 8) + t;
	}
	//cout<< v << endl;
	return v;
}

ZZ a32, b32, c32, ar32, prime32;
ZZ a64, b64, c64, ar64, prime64;

void init_prime_rep_32_64()
{
	// For 32-bit PrimeRep, this is tested to be fine. Random choosing one may fail
	// to find primes.
	a32 = mpz_class("4259798124");
	b32 = mpz_class("2958654130");
	c32 = mpz_class("4086806493");
	ar32 = mpz_class("92120724154956558556099256115");
	prime32 = mpz_class("103063105079645973865556145757");

	// 64-bit PrimeRep
	a64 = mpz_class("15836486067837767045");
	b64 = mpz_class("11844402977525860743");
	c64 = mpz_class("13743857551391033716");
	ar64 = mpz_class("3573582872689503600473447676545102849106026526793098187393");
	prime64 = mpz_class("6283709399869597894832209291784394294366457329796052598131");
}
