/*
 * RSAAccumulator.h
 *
 * Based on J. Li, N. Li, and R. Xue, ÒUniversal Accumulators with Efficient
 * Nonmembership Proofs,Ó in Proceedings of the 5th international conference
 * on Applied Cryptography and Network Security (ACNS), 2007, pp. 253Ð269.
 *
 *  Created on: Mar 18, 2013
 *      Author: jzhou
 */

#ifndef RSAACCUMULATOR_H_
#define RSAACCUMULATOR_H_

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <set>

NTL_CLIENT

class RSAAccumulator {
public:

	RSAAccumulator(long keylen);

	RSAAccumulator(RSAAccumulator &a) :
		_p(a._p), _q(a._q), _n(a._n), _g(a._g), _phi(a._phi) { ZZ_p::init(_n); }

	RSAAccumulator(ZZ p, ZZ q, ZZ g) :
		_p(p), _q(q), _g(g) { _n = _p * _q; _phi = (_p - 1) * (_q - 1); ZZ_p::init(_n); }

	// set the internal set of values as the input parameter
	void setSet(std::set<ZZ> &s);

	// generate accumulator with public/private key information
	ZZ publicAccumulate();
	ZZ privateAccumulate();
	ZZ privateAccumulate(std::set<ZZ> &s) { return privateAccumulate(s, _g); }

	// Given a new set of values and previous accumulator c, generate a new one
	ZZ privateAccumulate(std::set<ZZ> &s, ZZ c);

	// generate membership proof (<= _keylen bits) for value v in the set
	ZZ publicGenProof(ZZ v);
	//ZZ privateGenProof(ZZ v);

	// generate a proof for a subset, must have called setSet
	ZZ publicGenSubsetProof(std::set<ZZ> &s);
	ZZ publicGenSubsetProof(std::set<ZZ> &all, std::set<ZZ> &partial);

	// verify a proof for a subset
	bool verifySubsetProof(std::set<ZZ> &set, ZZ proof, ZZ c);
	bool publicVerifySubsetProof(std::set<ZZ> &s, ZZ proof, ZZ c);

	// generate non-membership proof (a, d) for value v in the set
	// a is <= 480 bits (3*160 SHA1 hash length)
	// b is <= _keylen bits
	bool publicGenNonProof(const ZZ &v, ZZ &a, ZZ &d);

	// generate non-membership proof (a, d) for a set of values
	bool publicGenNonProof(std::set<ZZ> &s, ZZ &a, ZZ &d);

	// Given value x in the set, verify with proof and accumulator "c"
	bool verifyMemberProof(ZZ x, ZZ proof, ZZ c);

	// Given value x not in set, verify with proof (a,d) and accumulator "c"
	bool verifyNonmemberProof(ZZ x, ZZ c, ZZ a, ZZ d);

	bool verifyNonmemberProof(std::set<ZZ> &s, ZZ c, ZZ &a, ZZ &d);

protected:
	// generate key parameters
	void keygen(long keylen);

private:
	ZZ _p, _q, _n, _g; // RSA parameters
	ZZ _phi; // totient function of N, = (p-1)*(q-1)

	std::set<ZZ> _set; // set to accumulated
	ZZ _u; // product of set values
	long _keylen;
};

// default 1024-bit RSAAccumulator parameters
extern ZZ rsaP, rsaQ, rsaG;
extern void init_rsa1024_accu_param();

#endif /* RSAACCUMULATOR_H_ */
