#ifndef _PROOFMANAGER_H_
#define _PROOFMANAGER_H_
#include <string>
#include <map>
#include <fstream>
#include "VTwoLevelProof.h"
class ProofManager
{
private:
    std::map<int,VTwoLevelProof*> proofMap;
public:
    ProofManager(std::string file,PrimeManager &pm,RSAAccumulator &accu)
    {
        ifstream ifs(file.c_str());
        if(!ifs)
        {
            throw runtime_error("cannot open "+file);
        }
        int tid;
        while(ifs >> tid)
        {
            std::set<ZZ> allset = pm.getTermPrimeSet(tid);
            VTwoLevelProof *ptlp = new VTwoLevelProof(allset,TREE_NODENUM,ifs,accu);
            proofMap[tid] = ptlp;
        }
    }
    TreeProofResult buildProof(int tid,const std::set<ZZ> & subSet)
    {
        if(proofMap.count(tid)==0)
        {
            throw runtime_error("proof manager do not contain this tid");
        }
        return proofMap[tid]->buildProof(subSet);
    }
    bool verifyProof(int tid,const std::set<ZZ> &subSet,const TreeProofResult &result)
    {
        if(proofMap.count(tid)==0)
        {
            throw runtime_error("proof manager do not contain this tid");
        }
        return proofMap[tid]->verifyProof(subSet,result);
    }
    ~ProofManager()
    {
        for(std::map<int,VTwoLevelProof*>::iterator it=proofMap.begin();
            it != proofMap.end();++it)
        {
            delete it->second;
        }
    }
};
#endif

