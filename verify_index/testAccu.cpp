#include <iostream>
#include <string>
#include <vector>
#include "ZZ.h"
#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include "util.h"
#include <google/profiler.h>
const int SET_SIZE = 100;
using namespace std;
int main(int argc, char *argv[])
{
    ProfilerStart("wgg.prof");
    init_prime_rep_32_64();
    PrimeRep pr(32, a32, b32, c32, prime32, ar32);
    init_rsa1024_accu_param();
    RSAAccumulator a(rsaP, rsaQ, rsaG);
    std::set<ZZ> allSet;
    ZZ tmp;
    for(int i=1;i<=SET_SIZE;++i)
    {
        tmp = i;
        allSet.insert(tmp);   
    }
    double bb = get_time();
    std::set<ZZ> primeAll = pr.getPrime(allSet);
    ZZ accu = a.privateAccumulate(primeAll);
    srand(time(0));
    int testNum=100;
    std::set<ZZ> subSet;
    for(int i=SET_SIZE-testNum+1;i<=SET_SIZE;++i)
    {
        tmp = rand()%i+1;
        if(subSet.find(tmp)==subSet.end())
        {
            subSet.insert(tmp);
        }
        else
        {
            tmp = i;
            subSet.insert(tmp);
        }
    }
    double b0 = get_time();
    std::set<ZZ> primeSubset = pr.getPrime(subSet);
    double b = get_time();
    ZZ proof = a.publicGenSubsetProof(primeAll, primeSubset);
    double e = get_time();
    ZZ proof1 = a.privateGenSubsetProof(primeAll, primeSubset);
    double e1 = get_time();
    ZZ proof2 = a.publicGenSubsetProofMT(primeAll, primeSubset,5);
    double e2 = get_time();
    cout << b0-bb<<" "<<b-b0<<" " <<e-b<<" "<<e1-e<<" " << e2-e1<<endl;
    cout << proof << endl;
    cout << proof1 << endl;
    cout << proof2 << endl;
    assert(proof==proof1);
    assert(proof==proof2);
    ProfilerStop();
	return 0;
}

