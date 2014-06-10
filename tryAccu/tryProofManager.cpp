#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include <NTL/ZZ.h>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <ctime>
#include <numeric>
#include <sys/time.h>
#include "PrimeManager.h"
#include "TreeProof.h"
#include "VTwoLevelProof.h"
#include "ProofManager.h"
using std::cout;
int main()
{
    init_prime_rep_32_64();
    PrimeRep pr(32, a32, b32, c32, prime32, ar32);

    init_rsa1024_accu_param();
    RSAAccumulator accu(rsaP, rsaQ, rsaG);

    PrimeManager pm(pr);

    ProofManager proofManager("wgg.dat",pm,accu);
    std::set<int> test1;
    test1.insert(1);
    std::set<ZZ> testPrime1 = pr.getPrime(test1);
    TreeProofResult tpr = proofManager.buildProof(1,testPrime1);
    if(proofManager.verifyProof(1,testPrime1,tpr))
    {
        cout << "PASS\n";
    }
    else
    {
        cout << "FAILED\n";
    }
    return 0;
}

