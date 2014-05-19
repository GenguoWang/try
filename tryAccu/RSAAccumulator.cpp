/*
 * RSAAccumulator.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: jzhou
 */
#include "RSAAccumulator.h"
#include "util.h"

RSAAccumulator::RSAAccumulator(long keylen)
{
	keygen(keylen);
}

void RSAAccumulator::setSet(std::set<ZZ> &s)
{
	_set.clear();

	_set = s;
	_u = 1;
	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		_u *= *it;
	}
}

void
RSAAccumulator::keygen(long keylen)
{
	_keylen = keylen;

	// generate safe prime of length keylen/2
	_p = GenGermainPrime_ZZ(keylen/2, 80);
	_q = GenGermainPrime_ZZ(keylen/2, 80);
	_n = _p * _q;
	_phi = (_p - 1) * (_q - 1);

	ZZ r;
	r = RandomLen_ZZ(keylen/4);
	MulMod(_g, r, r, _n);

	cout<<"public key is: \nn=" << _n << "\ng=" << _g << "\n\n";
	cout<<"private key is: \np=" << _p << "\nq=" << _q << "\n\n";
}

// c = g^{x1*x2*...xn} mod n
ZZ
RSAAccumulator::publicAccumulate()
{
	ZZ_pBak bak;
	ZZ exp;
	exp = 1;
	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		exp *= *it;
	}

	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p c;
	conv(c, _g);

	c = power(c, exp);

	ZZ ret;
	conv(ret, c);
	bak.restore(); // restore old modulus (if any)
	return ret;
}

// exp = x1*x2*...xn mod phi; c = g ^ exp;
ZZ
RSAAccumulator::privateAccumulate()
{
	return privateAccumulate(_set, _g);
}

// exp = x1*x2*...xn mod phi; return prevAccu ^ exp;
ZZ
RSAAccumulator::privateAccumulate(std::set<ZZ> &s, ZZ prevAccu)
{
	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p c;
	conv(c, prevAccu);

	ZZ p;
	p = 1;
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		MulMod(p,  p, *it, _phi);
	}
	c = power(c, p);
	ZZ ret;
	conv(ret, c);
	bak.restore(); // restore old modulus (if any)
	return ret;
}

// c = g^{x1*x2*...xn/x} mod n
ZZ RSAAccumulator::publicGenProof(ZZ v)
{
	ZZ_pBak bak;
	ZZ exp;
	exp = 1;
	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		if (*it == v)
			continue;
		exp *= *it;
	}

	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p c;
	conv(c, _g);

	c = power(c, exp);
	ZZ ret;
	conv(ret, c);
	bak.restore(); // restore old modulus (if any)
	return ret;
}

// proof = g^{u/(s1*s2*...)} mod n
ZZ RSAAccumulator::publicGenSubsetProof(std::set<ZZ> &s)
{
	ZZ_pBak bak;
	ZZ exp;
	exp = 1;
	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		if (s.find(*it) != s.end())
			continue;
		exp *= *it;
	}

	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p c;
	conv(c, _g);

	c = power(c, exp);
	ZZ ret;
	conv(ret, c);
	bak.restore(); // restore old modulus (if any)
	return ret;
}

// we try to do exponential one by one
ZZ
RSAAccumulator::publicGenSubsetProof(std::set<ZZ> &all, std::set<ZZ> &partial)
{
	ZZ_pBak bak;

	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p c;
	conv(c, _g);

	for (std::set<ZZ>::iterator it = all.begin(); it != all.end(); it++) {
		if (partial.find(*it) != partial.end())
			continue;
		// not found, do exponential
		c = power(c, *it);
	}

	ZZ ret;
	conv(ret, c);
	bak.restore(); // restore old modulus (if any)
	return ret;
}

// verify if c == proof^{s1*s2*...)} mod n, the data owner verify with private information of _phi.
bool
RSAAccumulator::verifySubsetProof(std::set<ZZ> &s, ZZ proof, ZZ c)
{
	// use privateAccumulate
	ZZ x = privateAccumulate(s, proof);
	return x == c;
}

/*
 * The following code assumes public verification of subset proof, used by a
 * third party.
 */
bool
RSAAccumulator::publicVerifySubsetProof(std::set<ZZ> &s, ZZ proof, ZZ c)
{
	ZZ product;
	product = 1;
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		product *= *it;
	}

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n
	ZZ_p p;
	conv(p, proof);
	p = power(p, product);
	bak.restore(); // restore old modulus (if any)

	ZZ t, v;
	conv(v, p); // v = proof^product mod n
	t = AddMod(c, t, _n); // t = c mod n
	if (v == t)
		return true;
	return false;
}

// verify proof^x = c mod n
bool RSAAccumulator::verifyMemberProof(ZZ x, ZZ proof, ZZ c)
{
	// first, x must be prime larger than 3k bits
	if (!ProbPrime(x)) {
		alert("Not a prime!");
		return false;
	}

	/* TODO: verify x is a prime generated from input: SHA1 -> prime
	if (NumBits(x) < 3 * _keylen-3) {
		cerr << NumBits(x) << " # of prime bits is wrong!\n";
		return false;
	}*/

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n
	ZZ_p p;
	conv(p, proof);
	p = power(p, x);
	bak.restore(); // restore old modulus (if any)

	ZZ t, v;
	conv(v, p); // proof^x mod n
	t = AddMod(c, t, _n); // c mod n
	if (v == t)
		return true;
	return false;
}

bool RSAAccumulator::publicGenNonProof(const ZZ &v, ZZ &a, ZZ &d)
{
	// v must not be in the set
	if (_set.find(v) != _set.end()) {
		cerr<< __FUNCTION__ << ": value is in the set.\n";
		return false;
	}

	/* disable for fast speed
	if (!ProbPrime(v)) { // must be prime
		alert("Not a prime! ");
		return false;
	}*/

	ZZ td, ta, tb, b;

	// td = 1 = gcd(_u, v) = ta*_u + tb*v
	XGCD(td, ta, tb, _u, v);
	if (td != 1) {
		alert("GCD(u,v) = 1! Something seriously wrong!");
		return false;
	}
	while (ta < 0) {
		ta += v;
		tb -= _u;
	}

	//cout << NumBits(ta) << " bits, a = " << ta << endl;
	//cout << NumBits(tb) << " bits, b = " << tb << endl;
	//cout << NumBits(v) << " bits, v = " << v << endl;

	// calculate proof (a, d)
	a = ta;

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n
	ZZ_p g, tmp;
	conv(g, _g);
	NTL::negate(b, tb);
	tmp = power(g, b);
	bak.restore(); // restore old modulus (if any)
	conv(d, tmp);

	return true;
}

bool
RSAAccumulator::publicGenNonProof(std::set<ZZ> &s, ZZ &a, ZZ &d)
{
	ZZ v;
	v = 1;

	// s must not be in the set
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		if (_set.find(*it) != _set.end()) {
			cerr<< __FUNCTION__ << ": value is in the set.\n";
			return false;
		}

		/* disable for fast speed
		if (!ProbPrime(*it)) { // must be prime
			alert("Not a prime! ");
			return false;
		}*/
		v *= *it;
	}

	ZZ td, ta, tb, b;

	// td = 1 = gcd(_u, v) = ta*_u + tb*v
	XGCD(td, ta, tb, _u, v);
	if (td != 1) {
		alert("GCD(u,v) = 1! Something seriously wrong!");
		return false;
	}
	while (ta < 0) {
		ta += v;
		tb -= _u;
	}

	// calculate proof (a, d)
	a = ta;

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n
	ZZ_p g, tmp;
	conv(g, _g);
	NTL::negate(b, tb);
	tmp = power(g, b);
	bak.restore(); // restore old modulus (if any)
	conv(d, tmp);

	return true;
}

/* TODO: verify x is a prime generated from input: SHA1 -> prime
 * 	verify c is the accumulator
 */
// verify c^a = d^x * g (mod n)
bool RSAAccumulator::verifyNonmemberProof(ZZ x, ZZ c, ZZ a, ZZ d)
{
	// first, x must be prime larger than 3k bits
	if (!ProbPrime(x)) {
		alert("Not a prime!");
		return false;
	}

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p l, r, g;
	conv(l, c);
	l = power(l, a);

	conv(r, d);
	conv(g, _g);
	r = power(r, x);
	r *= g;
	bak.restore(); // restore old modulus (if any)

	if (l == r)
		return true;
	return false;
}

bool
RSAAccumulator::verifyNonmemberProof(std::set<ZZ> &s, ZZ c, ZZ &a, ZZ &d)
{
	ZZ v;
	v = 1;
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		v *= *it;
	}

	ZZ_pBak bak;
	bak.save(); // save current modulus (if any)
	ZZ_p::init(_n); // set modulus to _n

	ZZ_p l, r, g;
	conv(l, c);
	l = power(l, a);

	conv(r, d);
	conv(g, _g);
	r = power(r, v);
	r *= g;
	bak.restore(); // restore old modulus (if any)

	if (l == r)
		return true;
	return false;
}


ZZ rsaP, rsaQ, rsaG;

// 1024 bit RSA accumulator parameter
void init_rsa1024_accu_param()
{
	rsaP = conv<ZZ>("8807759348057915046602890450305201988482945060153279131967135931733125464078417535971388150123644254021071199836471271482234689373713868277028534517246463");
	rsaQ = conv<ZZ>("8593372488608406883931232164898980937483640906142276303242374883536646934125626522030731843119569262735139924299275505967319288557309661145307208218036303");
	rsaG = conv<ZZ>("9068581345075310674653689495809418270285225023922581109606296068000836117483808610677451827593729353942329658558665630621528354780249010920989573362766761");
}
