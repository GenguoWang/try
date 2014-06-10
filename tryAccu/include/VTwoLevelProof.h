#ifndef _VTWOLEVELPROOF_H_
#define _VTWOLEVELPROOF_H_

class VTwoLevelProof
{
private:
    int num;
    int n;
    std::map<ZZ,int> eleIndex;
    std::vector<vector<ZZ> > t;
    std::vector<ZZ> preProof;
    std::map<ZZ,ZZ> prePrime;
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
                result.insert(t[level][offset]);
            }
        }
        else if(level==1)
        {
            for(unsigned i=0;i<t[level].size();++i) result.insert(prePrime[t[1][i]]);
        }
        return result;
    }
public:
    void saveToStream(ostream &out)
    {
        out << t[2][0] << '\n';
        out << t[1].size() << '\n';
        for(unsigned i=0;i < t[1].size();++i)
        {
            out << t[1][i] << ' ' << preProof[i] << ' ' << prePrime[t[1][i]] << '\n';
        }
    }
    void loadFromStream(istream &in)
    {
        ZZ accu,prf,prm;
        in >> accu;
        t[2].push_back(accu);
        int size;
        in >> size;
        for(int i=0;i<size;++i)
        {
            in >> accu >> prf >> prm;
            t[1].push_back(accu);
            preProof.push_back(prf);
            prePrime[accu] = prm;
        }
    }
    VTwoLevelProof(std::set<ZZ> &allSet,int num,PrimeManager &pm,RSAAccumulator &ac):num(num),t(3),a(ac)
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
            ZZ tmpAccu = a.privateAccumulate(tmpSet);
            t[1].push_back(tmpAccu);
            prePrime[tmpAccu] = pm.getPrime(tmpAccu);
        }
        
        //cout << "TLP: level 1 inited" << endl;
        tmpSet = getPrimeSet(1,0);
        a.setSet(tmpSet);
        t[2].push_back(a.privateAccumulate(tmpSet));
        for(unsigned i=0;i<t[1].size();++i)
        {
            preProof.push_back(a.publicGenProof(prePrime[t[1][i]]));
        }
        finalAccu = t[2][0];
    }

    VTwoLevelProof(std::set<ZZ> &allSet,int num,istream &in,RSAAccumulator &ac):num(num),t(3),a(ac)
    {
        n = allSet.size();
        int tmpCnt=0;
        for(std::set<ZZ>::iterator it = allSet.begin();it != allSet.end();++it)
        {
            t[0].push_back(*it);
            eleIndex[*it] = tmpCnt;
            tmpCnt++;
        }
        loadFromStream(in);
        finalAccu = t[2][0];
    }
    
    TreeProofResult buildProof(const std::set<ZZ> &subSet)
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
            tmpproof = a.publicGenSubsetProof(tmpAllPrimeSet, tmpSet);
            accu = t[1][i];
            oneProof.proofs.push_back(std::make_pair(tmpproof,accu));

            tmpproof = preProof[i];
            accu = t[2][0];
            oneProof.proofs.push_back(std::make_pair(tmpproof,accu));

            proof.push_back(oneProof);
        }
        TreeProofResult tpr(TreeProofResult::TYPE_TREE);
        tpr.tProof = proof;
        return tpr;
    }
    bool verifyProof(const std::set<ZZ> &subSet,const TreeProofResult &result)
    {
        proof = result.tProof;
        return verifyProof(subSet);
    }
    bool verifyProof(const std::set<ZZ> &subSet)
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
            std::set<ZZ> primeSubset = oneProof.oneset;
            for(int j=0;j<2;++j)
            {
                cout << "verify:" << oneProof.proofs[j].first << " " << oneProof.proofs[j].second << endl;
                
                if(!a.verifySubsetProof(primeSubset, oneProof.proofs[j].first, oneProof.proofs[j].second))
                {
                    std::cout << "Error:"<<j << std::endl;
                    return false;
                }
                primeSubset.clear(); 
                primeSubset.insert(prePrime[oneProof.proofs[j].second]);
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
    int getProofSize()
    {
        return proof.size();
    }
};
#endif

