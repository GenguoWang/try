#ifndef _TWOLEVELPROOF_H_
#define _TWOLEVELPROOF_H_

class TwoLevelProof
{
private:
    int num;
    int n;
    PrimeManager &pm;
    std::map<ZZ,int> eleIndex;
    std::vector<vector<ZZ> > t;
    std::vector<vector<ZZ> > preProof;
    RSAAccumulator &a;
    ZZ finalAccu;
    std::vector<OneSetProof> proof;
private:
    std::set<ZZ> getPrimeSet(int level,int index)
    {
        std::set<ZZ> result;
        if(level==0)
        {
            for(int i=0;i<num;++i)
            {
                unsigned offset = index*num+i;
                if(offset>=t[level].size())break;
                result.insert(pm.getPrime(t[level][offset]));
            }
        }
        else if(level==1)
        {
            for(unsigned i=0;i<t[level].size();++i) result.insert(pm.getPrime(t[level][i]));
        }
        else if(level==2)
        {
            result.insert(pm.getPrime(t[level][0]));
        }
        return result;
    }
public:
    TwoLevelProof(std::set<ZZ> &allSet,int num,PrimeManager &pm,RSAAccumulator &ac):num(num),pm(pm),t(3),preProof(3),a(ac)
    {
        n = allSet.size();
        //cout << "TLP: " << n << " size of set" << endl;
        int tmpCnt=0;
        for(std::set<ZZ>::iterator it = allSet.begin();it != allSet.end();++it)
        {
            t[0].push_back(*it);
            eleIndex[*it] = tmpCnt;
            tmpCnt++;
        }
        //cout << "TLP: eleIndex inited" << endl;
        std::set<ZZ> tmpSet;
        int cnt = (t[0].size()-1)/num+1;
        ZZ wgg;
        for(int j=0;j<cnt;++j)
        {
            tmpSet = getPrimeSet(0,j);
            //cout << "tmpSet:" << endl;
            //outputSet(tmpSet);
            t[1].push_back(a.privateAccumulate(tmpSet));
        }
        //cout << "TLP: level 1 inited" << endl;
        tmpSet = getPrimeSet(1,0);
        a.setSet(tmpSet);
        t[2].push_back(a.privateAccumulate(tmpSet));
        for(unsigned i=0;i<t[1].size();++i)
        {
            preProof[1].push_back(a.publicGenProof(pm.getPrime(t[1][i])));
        }
        finalAccu = t[2][0];
    }
    void buildProof(std::set<ZZ> &subSet)
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

            ZZ accu,tmpproof;
            std::set<ZZ> tmpAllPrimeSet = getPrimeSet(0,i);
            std::set<ZZ> tmpSubPrimeSet = pm.getPrime(tmpSet);
            tmpproof = a.publicGenSubsetProof(tmpAllPrimeSet, tmpSubPrimeSet);
            accu = t[1][i];
            oneProof.proofs.push_back(std::make_pair(tmpproof,accu));

            tmpproof = preProof[1][i];
            accu = t[2][0];
            oneProof.proofs.push_back(std::make_pair(tmpproof,accu));

            proof.push_back(oneProof);
        }
    }
    bool verifyProof(std::set<ZZ> &subSet)
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
            for(int j=0;j<2;++j)
            {
                if(!a.verifySubsetProof(primeSubset, oneProof.proofs[j].first, oneProof.proofs[j].second))
                {
                    std::cout << "Error:"<<j << std::endl;
                    return false;
                }
                primeSubset.clear(); 
                primeSubset.insert(pm.getPrime(oneProof.proofs[j].second));
            }
            if(finalAccu != oneProof.proofs[1].second)
            {
                std::cout << "accu not equal"<< std::endl;
                return false;
            }
        }
        cout << "Passed"<<endl;
        return true;
    }
    void output()
    {
        for(int i=2;i>=0;i--)
        {
            for(unsigned j=0;j<t[i].size();++j)
            {
                cout << t[i][j] << " ";
            }
            cout << endl;
        }
    }
};
#endif
