#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include <NTL/ZZ.h>
#include <set>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <stdexcept>
using std::cout;
using std::endl;
class TreeProof
{
int e;
int num;
int n;
std::vector<vector<ZZ> > t;
std::map<ZZ,int> eleIndex;
private:
    std::set<ZZ> getPrimeSet(int level,int index,PrimeRep &pr)
    {
        std::set<ZZ> result;
        for(int i=0;i<num;++i)
        {
            int offset = index*num+i;
            if(offset>=t[level].size())break;
            result.insert(pr.getPrime(t[level][offset]));
        }
        return result;
    }
public:
    TreeProof(int exp):e(exp),t(exp+1)
    {
        
    }
    void buildProof(std::set<ZZ> &subSet,RSAAccumulator &a,PrimeRep &pr)
    {
        std::vector<std::set<ZZ> > splitSet((n-1)/num+1);
        for(std::set<ZZ>::iterator it = subSet.begin();it != subSet.end();++it)
        {
            if(eleIndex.count(*it)==0)
            {
                throw runtime_error("unknown subSet element");
            }
            int index = eleIndex[*it];
            splitSet[index/num].insert(pr.getPrime(*it));
        }
        for(int i=0;i<splitSet.size();++i)
        {
            std::set<ZZ> tmpSet = splitSet[i];
            if(tmpSet.empty())continue;
            int index = i;
            for(int j=0;j<e;++j)
            {
                std::set<ZZ> tmpAllPrimeSet = getPrimeSet(j,index,pr);
                ZZ proof = a.publicGenSubsetProof(tmpAllPrimeSet, tmpSet);
                tmpSet.clear();
                tmpSet.insert(t[j+1][index]);
                index = index/num;
                cout << "proof"<<j<<": " << proof << endl;
            }
        }
    }
    void init(std::set<ZZ> &allSet,RSAAccumulator &a,PrimeRep &pr)
    {
        n = allSet.size();
        num = ceil(pow(n,1.0/e));
        int level = e+1;
        int tmpCnt=0;
        for(std::set<ZZ>::iterator it = allSet.begin();it != allSet.end();++it)
        {
            t[0].push_back(*it);
            eleIndex[*it] = tmpCnt;
            tmpCnt++;
        }
        std::set<ZZ> tmpSet;
        for(int i=1;i<level;++i)
        {
            int cnt = (t[i-1].size()-1)/num+1;
            for(int j=0;j<cnt;++j)
            {
                for(int k=0;k<num;++k)
                {
                    size_t index = j*num+k;
                    if(index>t[i-1].size()) break;
                    tmpSet.insert(pr.getPrime(t[i-1][index]));
                }
                t[i].push_back(a.privateAccumulate(tmpSet));
                tmpSet.clear();
            }
        }
        for(int i=level-1;i>=0;--i)
        {
            for(size_t j=0;j<t[i].size();++j)
            {
                cout << t[i][j] << " ";
            }
            cout << endl;
        }
        cout << "number: " << n << " weight: " << num << endl;
    }
};
int main()
{
    init_prime_rep_32_64();
    PrimeRep pr(32, a32, b32, c32, prime32, ar32);
    //PrimeRep pr(64, a64, b64, c64, prime64, ar64);

    init_rsa1024_accu_param();
    RSAAccumulator a(rsaP, rsaQ, rsaG);

    std::set<ZZ> allSet;
    ZZ tmp;
    for(int i=1;i<=64;++i)
    {
        tmp = i;
        allSet.insert(tmp);   
    }
    TreeProof tp(3);
    tp.init(allSet,a,pr);
    std::set<ZZ> subSet;
    tmp = 3;
    subSet.insert(tmp);
    tp.buildProof(subSet,a,pr);
    /*
    ZZ z,accu,proof;
    std::set<ZZ> primeAll = pr.getPrime(allSet);
    accu = a.privateAccumulate(primeAll);
    std::set<ZZ> primeSubset;
    primeSubset.insert(pr.getPrime(3));
    proof = a.publicGenSubsetProof(primeAll, primeSubset);
	if(a.verifySubsetProof(primeSubset, proof, accu))
    {
        std::cout << "OK" << std::endl;
    }
    else
    {
        std::cout << "Failed" << std::endl;
    }
    */
    return 0;
}
