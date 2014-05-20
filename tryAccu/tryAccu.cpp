#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include <NTL/ZZ.h>
#include <set>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <ctime>
#include <sys/time.h>
using std::cout;
using std::endl;
const int SET_SIZE=10000;
//const int SUB_SIZE=9000;
double get_time()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + static_cast<double>(tv.tv_usec*1e-6);
}
template <typename _T>
void outputSet(std::set<_T> s)
{
    for(typename std::set<_T>::iterator it = s.begin();it != s.end();++it)
    {
        cout << *it << " ";
    }
    cout << endl;
}
struct OneSetProof
{
    std::set<ZZ> oneset;
    std::vector<std::pair<ZZ,ZZ> >proofs;
};
class PrimeManager
{
private:
    std::map<ZZ,ZZ> pm;
    PrimeRep pr;
public:
    PrimeManager(PrimeRep &pr):pr(pr)
    {
        
    }
    std::set<ZZ> getPrime(const std::set<ZZ> &originSet)
    {
        std::set<ZZ> result;
        for(std::set<ZZ>::iterator it=originSet.begin();it != originSet.end();++it)
        {
            result.insert(getPrime(*it));
        }
        return result;
    }
    ZZ getPrime(const ZZ &origin)
    {
        if(pm.count(origin)>0)
        {
            return pm[origin];
        }
        else
        {
            ZZ p = pr.getPrime(origin);
            pm[origin] = p;
            return p;
        }
    }
};
class TreeProof
{
int e;
int num;
int n;
std::vector<vector<ZZ> > t;
std::map<ZZ,int> eleIndex;
std::vector<vector<ZZ> > preProof;
std::vector<OneSetProof> proof;
ZZ finalAccu;
private:
    std::set<ZZ> getPrimeSet(int level,int index)
    {
        std::set<ZZ> result;
        for(int i=0;i<num;++i)
        {
            unsigned offset = index*num+i;
            if(offset>=t[level].size())break;
            result.insert(pm.getPrime(t[level][offset]));
        }
        return result;
    }
public:
    PrimeManager pm;
    TreeProof(int exp,PrimeRep &pr):e(exp),t(exp+1),preProof(exp+1),pm(pr)
    {
        
    }
    void buildProof(std::set<ZZ> &subSet,RSAAccumulator &a)
    {
        proof.clear();
        std::vector<std::set<ZZ> > splitSet((n-1)/num+1);
        for(std::set<ZZ>::iterator it = subSet.begin();it != subSet.end();++it)
        {
            if(eleIndex.count(*it)==0)
            {
                throw runtime_error("unknown subSet element");
            }
            int index = eleIndex[*it];
            splitSet[index/num].insert(*it);
        }
        for(unsigned i=0;i<splitSet.size();++i)
        {
            std::set<ZZ> &tmpSet = splitSet[i];
            if(tmpSet.empty())continue;
            OneSetProof oneProof;
            oneProof.oneset = tmpSet;
            ZZ accu;
            int index = i;
            {
                std::set<ZZ> tmpAllPrimeSet = getPrimeSet(0,index);
                std::set<ZZ> tmpSubPrimeSet = pm.getPrime(tmpSet);
                ZZ proof = a.publicGenSubsetProof(tmpAllPrimeSet, tmpSubPrimeSet);
                accu = t[1][index];
                oneProof.proofs.push_back(std::make_pair(proof,accu));
            }
            for(int j=1;j<e;++j)
            {
                ZZ proof = preProof[j][index];
                index = index/num;
                accu = t[j+1][index];
                oneProof.proofs.push_back(std::make_pair(proof,accu));
            }
            proof.push_back(oneProof);
        }
    }
    bool verifyProof(std::set<ZZ> &subSet,RSAAccumulator &a)
    {
        //check all element appears in proof
        std::set<ZZ> checkSet;
        for(unsigned i=0;i<proof.size();++i)
        {
            checkSet.insert(proof[i].oneset.begin(),proof[i].oneset.end());
        }
        cout << "size: " << checkSet.size() <<  " " << subSet.size() << endl;
        if(!(checkSet==subSet))
        {
            outputSet(checkSet);
            outputSet(subSet);
            cout << "the elemenets in proof is not equal to the set" << endl;
            return false;
        }
        //check proof
        for(unsigned i=0;i<proof.size();++i)
        {
            OneSetProof &oneProof = proof[i];
            std::set<ZZ> primeSubset = pm.getPrime(oneProof.oneset);
            for(int j=0;j<e;++j)
            {
                if(!a.verifySubsetProof(primeSubset, oneProof.proofs[j].first, oneProof.proofs[j].second))
                {
                    std::cout << "Error:"<<j << std::endl;
                    return false;
                }
                primeSubset.clear(); 
                primeSubset.insert(pm.getPrime(oneProof.proofs[j].second));
            }
            if(finalAccu != oneProof.proofs[e-1].second)
            {
                std::cout << "accu not equal"<< std::endl;
                return false;
            }
        }
        cout << "Passed"<<endl;
        return true;
    }
    void init(std::set<ZZ> &allSet,RSAAccumulator &a)
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
                tmpSet = getPrimeSet(i-1,j);
                if(i>1)
                {
                    a.setSet(tmpSet);
                    for(int k=0;k<num;++k)
                    {
                        size_t index = j*num+k;
                        if(index>=t[i-1].size()) break;
                        preProof[i-1].push_back(a.publicGenProof(pm.getPrime(t[i-1][index])));
                    }
                }
                t[i].push_back(a.privateAccumulate(tmpSet));
            }
        }
        finalAccu = t[e][0];
        cout << "number: " << n << " weight: " << num << endl;
        checkTree(a);
    }
    void checkTree(RSAAccumulator &a)
    {
        cout << "start check tree"<< endl;
        for(unsigned i=e;i>=1;--i)
        {
            for(unsigned j=0;j<t[i].size();++j)
            {
                ZZ accu = t[i][j];
                std::set<ZZ> allPrimeSet = getPrimeSet(i-1,j);
                if(accu != a.privateAccumulate(allPrimeSet))
                {
                    cout << "check tree failed:" << i << " " << j << endl;
                    return;
                }
            }
            if(i>1)
            {
                for(unsigned j=0;j<t[i].size();++j)
                {
                    ZZ accu = t[i][j];
                    for(int k=0;k<num;++k)
                    {
                        unsigned index = j*num+k;
                        if(index>=t[i-1].size()) break;
                        if(!a.verifyMemberProof(pm.getPrime(t[i-1][index]), preProof[i-1][index], accu))
                        {
                            cout << "preProof failed:" << i << " " << j << endl;
                            return;
                        }
                    }
                }
            }
        }
        cout << "pass check tree"<< endl;
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
    for(int i=1;i<=SET_SIZE;++i)
    {
        tmp = i;
        allSet.insert(tmp);   
    }
    std::set<ZZ> primeAll = pr.getPrime(allSet);
    ZZ accu = a.privateAccumulate(primeAll);
    double timeMat1[10][10];
    double timeMat2[10][10];
    double timeMat3[10][10];
    double timeMat4[10][10];
    for(int iE=2;iE<8;++iE)
    {
        TreeProof tp(iE,pr);
        tp.init(allSet,a);
        for(int i=0;i<10;++i)timeMat1[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat2[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat3[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat4[iE][i]= 0;
        for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
        {
            for(int ii=0;ii<10;++ii)
            {
                cout << "test:" << iE << " " << testNum <<caseNum<< " " << ii<< endl;
                std::set<ZZ> subSet;
                srand(time(0));
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
                double start_time,time_point1,time_point2;
                start_time = get_time();
                tp.buildProof(subSet,a);
                time_point1 = get_time();
                tp.verifyProof(subSet,a);
                time_point2 = get_time();
                double buildtime1 = time_point1-start_time;
                double prooftime1 = time_point2-time_point1;

                ZZ z,proof;

                start_time = get_time();
                std::set<ZZ> primeAll = tp.pm.getPrime(allSet);
                std::set<ZZ> primeSubset = tp.pm.getPrime(subSet);
                proof = a.publicGenSubsetProof(primeAll, primeSubset);
                time_point1 = get_time();
                if(a.verifySubsetProof(primeSubset, proof, accu))
                {
                    std::cout << "OK" << std::endl;
                }
                else
                {
                    std::cout << "Failed" << std::endl;
                }
                time_point2 = get_time();
                double buildtime2 = time_point1-start_time;
                double prooftime2 = time_point2-time_point1;
                timeMat1[iE][caseNum] += buildtime1;
                timeMat2[iE][caseNum] += prooftime1;
                timeMat3[iE][caseNum] += buildtime2;
                timeMat4[iE][caseNum] += prooftime2;
                cout << "tick: " << buildtime1 << " " <<prooftime1 << " "
                << buildtime2 << " " << prooftime2 << endl;
            }
            timeMat1[iE][caseNum] /= 10;
            timeMat2[iE][caseNum] /= 10;
            timeMat3[iE][caseNum] /= 10;
            timeMat4[iE][caseNum] /= 10;
            cout << "size: " << testNum 
            << " tree: " << timeMat1[iE][caseNum] << " " << timeMat2[iE][caseNum]
            <<" origin: " << timeMat3[iE][caseNum]<< " " << timeMat4[iE][caseNum] << endl;
        }
    }
    for(int iE=2;iE<8;++iE)
    {
        cout << "e=" << iE << endl;
        for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
        {
            cout << "size: " << testNum 
            << " tree: " << timeMat1[iE][caseNum] << " " << timeMat2[iE][caseNum]
            <<" origin: " << timeMat3[iE][caseNum]<< " " << timeMat4[iE][caseNum] << endl;
        }
    }
    return 0;
}
