#ifndef _TREEPROOF_H_
#define _TREEPROOF_H_

#include "utility.h"
#include "TProof.h"


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

#endif
