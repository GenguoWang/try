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
using std::cout;
int main()
{
    init_prime_rep_32_64();
    PrimeRep pr(32, a32, b32, c32, prime32, ar32);

    init_rsa1024_accu_param();
    RSAAccumulator accu(rsaP, rsaQ, rsaG);
    PrimeManager pm(pr);
    ofstream ofs("wgg.dat");
    for(int tid=1; tid <= 10;++tid)
    {
        std::set<ZZ> allSet = pm.getTermPrimeSet(tid);
        VTwoLevelProof tlp(allSet,TREE_NODENUM,pm,accu);
        ofs << tid << ' ';
        tlp.saveToStream(ofs);
    }
    ofs.close();
    return 0;
}
