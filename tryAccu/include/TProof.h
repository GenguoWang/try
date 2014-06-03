#ifndef _TPROOF_H_
#define _TPROOF_H_

struct OneSetProof
{
    std::set<ZZ> oneset;
    std::vector<std::pair<ZZ,ZZ> >proofs;
};

#endif
