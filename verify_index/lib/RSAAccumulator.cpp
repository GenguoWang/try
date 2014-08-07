/*
 * RSAAccumulator.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: jzhou
 */
#include "RSAAccumulator.h"
#include "util.h"
#include "boost/threadpool.hpp"
#include "boost/bind.hpp"
#include <algorithm>
using namespace boost::threadpool;


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
	ZZ exp;
	exp = 1;
	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		exp *= *it;
	}

	ZZ c = PowerMod(_g, exp, _n);
	return c;
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
	ZZ c = prevAccu;

	ZZ p = 1;
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		MulMod(p,  p, *it, _phi);
	}
	c = PowerMod(c, p, _n);
	return c;
}

// c = g^{x1*x2*...xn/x} mod n
ZZ RSAAccumulator::publicGenProof(ZZ v)
{
	ZZ c = _g;

	for (std::set<ZZ>::iterator it = _set.begin(); it != _set.end(); it++) {
		if (*it == v)
			continue;
		c = PowerMod(c, *it, _n);
	}

	return c;
}

// we try to do exponential one by one
ZZ
RSAAccumulator::publicGenSubsetProof(std::set<ZZ> &all, std::set<ZZ> &partial)
{
	std::set<ZZ> result;
    std::set_difference(all.begin(), all.end(), partial.begin(), partial.end(),
                    std::inserter(result, result.end()) );

	ZZ c = _g;
	for (std::set<ZZ>::iterator it = result.begin(); it != result.end(); it++) {
		c = PowerMod(c, *it, _n);
	}

	return c;
}

ZZ
RSAAccumulator::privateGenSubsetProof(std::set<ZZ> &all, std::set<ZZ> &partial)
{
	std::set<ZZ> result;
    std::set_difference(all.begin(), all.end(), partial.begin(), partial.end(),
                    std::inserter(result, result.end()) );

	ZZ c = _g;
    ZZ p = 1;
	for (std::set<ZZ>::iterator it = result.begin(); it != result.end(); it++) {
        p = MulMod(p,*it,_phi);
	}
    c = PowerMod(c,p,_n);

	return c;
}

void dowork(const vector<ZZ> &todo, ZZ &result)
{
    result = 1;
    for(vector<ZZ>::const_iterator it = todo.begin();it != todo.end();++it)
    {
        result *= *it;
    }
}

ZZ RSAAccumulator::publicGenSubsetProofMT(const::set<ZZ> &all, const std::set<ZZ> &partial,int n)
{
    pool tp(n);
    vector<vector<ZZ> > v(n);
    vector<ZZ> r(n);
    int cnt=0;
	std::set<ZZ> result;
    std::set_difference(all.begin(), all.end(), partial.begin(), partial.end(),
                    std::inserter(result, result.end()) );
	for (std::set<ZZ>::iterator it = result.begin(); it != result.end(); it++) {
        v[cnt%n].push_back(*it);
        cnt++;
	}
    for(int i=0;i<n;++i)
    {
        tp.schedule(boost::bind(dowork,boost::cref(v[i]),boost::ref(r[i])));
    }
    tp.wait();
	ZZ c = _g;
    for(int i=0;i<n;++i)
    {
        c = PowerMod(c,r[i],_n);
    }
    return c;
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

	ZZ p = PowerMod(proof, product, _n);
	return (p == c);
}

// verify proof^x = c mod n
bool RSAAccumulator::verifyMemberProof(ZZ x, ZZ proof, ZZ c)
{
	// first, x must be prime larger than 3k bits
	if (!ProbPrime(x)) {
		alert("Not a prime!");
		return false;
	}

	ZZ p = PowerMod(proof, x, _n);
	return (p == c);
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
	td = XGCD(_u, v, ta, tb);
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

	ZZ tmp;
	d = PowerMod(_g, -tb, _n);

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

	return publicGenNonProof(v, a, d);
}

// verify c^a = d^x * g (mod n)
bool RSAAccumulator::verifyNonmemberProof(ZZ x, ZZ c, ZZ a, ZZ d)
{
	/*
	if (!ProbPrime(x)) {
		alert("Not a prime!");
		return false;
	}*/

	ZZ l, r;
	l = PowerMod(c, a, _n);

	r = PowerMod(d, x, _n);
	r = MulMod(r, _g, _n);

	return (l == r);
}

bool
RSAAccumulator::verifyNonmemberProof(std::set<ZZ> &s, ZZ c, ZZ &a, ZZ &d)
{
	ZZ v;
	v = 1;
	for (std::set<ZZ>::iterator it = s.begin(); it != s.end(); it++) {
		v *= *it;
	}

	return verifyNonmemberProof(v, c, a, d);
}


ZZ rsaP, rsaQ, rsaG;

// 1024 bit RSA accumulator parameter
void init_rsa1024_accu_param()
{
	rsaP = mpz_class("8807759348057915046602890450305201988482945060153279131967135931733125464078417535971388150123644254021071199836471271482234689373713868277028534517246463");
	rsaQ = mpz_class("8593372488608406883931232164898980937483640906142276303242374883536646934125626522030731843119569262735139924299275505967319288557309661145307208218036303");
	rsaG = mpz_class("9068581345075310674653689495809418270285225023922581109606296068000836117483808610677451827593729353942329658558665630621528354780249010920989573362766761");
}
