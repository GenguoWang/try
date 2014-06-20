#ifndef _TPROOF_H_
#define _TPROOF_H_

/*
const int TREE_NODENUM = 10;
const int TREE_MIN = 3000;
struct OneSetProof
{
    std::set<ZZ> oneset;
    std::vector<std::pair<ZZ,ZZ> >proofs;
};
struct TreeProofResult
{
    enum TYPE{TYPE_NORMAL,TYPE_TREE};
    int type;
    ZZ nProof;
    std::vector<OneSetProof> tProof;
    TreeProofResult(int type):type(type)
    {
        
    }
    TreeProofResult(const TreeProofResult &old)
    {
        type = old.type;
        nProof = old.nProof;
        tProof = old.tProof;
    }
    TreeProofResult& operator=(const TreeProofResult &old)
    {
        type = old.type;
        nProof = old.nProof;
        tProof = old.tProof;
        return *this;
    }
};
*/
#include <vector>
#include <set>
#include <NTL/ZZ.h>

const int TREE_NODENUM = 10;
const int TREE_MIN = 3000;
struct OneSetProof
{
    std::set<ZZ> oneset;
    std::vector<std::pair<ZZ,ZZ> >proofs;
};
struct TreeProofResult
{
    enum TYPE{TYPE_NORMAL,TYPE_TREE};
    int type;
    ZZ nProof;
    std::vector<OneSetProof> tProof;
    TreeProofResult(){}
    TreeProofResult(int type):type(type)
    {
        
    }
    TreeProofResult(const TreeProofResult &old)
    {
        type = old.type;
        nProof = old.nProof;
        tProof = old.tProof;
    }
    TreeProofResult& operator=(const TreeProofResult &old)
    {
        type = old.type;
        nProof = old.nProof;
        tProof = old.tProof;
        return *this;
    }
};
ostream& operator<< (ostream &out, OneSetProof &r)
{
    out << r.oneset.size() << " ";
    for(std::set<ZZ>::iterator it=r.oneset.begin(); it != r.oneset.end(); ++it)
    {
        out << *it << " ";
    }
    out << "\n";
    out << r.proofs.size() << " ";
    for(std::vector<std::pair<ZZ,ZZ> >::iterator it = r.proofs.begin(); it != r.proofs.end(); ++it)
    {
        out << (*it).first << " " << (*it).second << " ";
    }
    return out;
}
istream& operator>> (istream &in, OneSetProof &r)
{
    int size;
    ZZ tmpZZ,tmpZZ1;
    in >> size;
    r.oneset.clear();
    for(int i=0; i < size; ++i)
    {
        in >> tmpZZ;
        r.oneset.insert(tmpZZ);
    }
    in >> size;
    r.proofs.clear();
    for(int i=0; i < size; ++i)
    {
        in >> tmpZZ >> tmpZZ1;
        r.proofs.push_back(std::make_pair(tmpZZ,tmpZZ1));
    }
    return in;
}
ostream& operator<< (ostream &out, TreeProofResult &r)
{
    out << r.type << " ";
    if(r.type == TreeProofResult::TYPE_NORMAL)
    {
        out << r.nProof;
    }
    else
    {
        out << r.tProof.size() << "\n";
        for(unsigned i=0; i < r.tProof.size(); ++i)
        {
            out << r.tProof[i] << " ";
        }
    }
    return out;
}
istream& operator>> (istream &in, TreeProofResult &r)
{
    in >> r.type;
    if(r.type == TreeProofResult::TYPE_NORMAL)
    {
        in >> r.nProof;
    }
    else
    {
        int size;
        OneSetProof tmpProof;
        in >> size;
        r.tProof.clear();
        for(int i=0; i < size; ++i)
        {
            in >> tmpProof;
            r.tProof.push_back(tmpProof);
        }
    }
    return in;
}

#endif
