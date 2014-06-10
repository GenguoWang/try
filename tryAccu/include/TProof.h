#ifndef _TPROOF_H_
#define _TPROOF_H_

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

#endif
