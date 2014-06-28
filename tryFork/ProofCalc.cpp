#include "ProofCalc.h"
using namespace std;
using namespace NTL;
RSAAccumulator *RSAAccumulatorService::getRSAAccumulator()
{
    if(a==NULL)
    {
        init_rsa1024_accu_param();
        a = new RSAAccumulator(rsaP, rsaQ, rsaG);
    }
    return a;
}
RSAAccumulator *RSAAccumulatorService::a = NULL;
void ProofCalcJob::readFromStream(istream &in)
{
    int size;
    ZZ tmp;
    primeAll.clear();
    primeSubset.clear();
    in >> size;
    for(int i=0; i<size;++i)
    {
        in >> tmp;
        primeAll.insert(tmp);
    }
    in >> size;
    for(int i=0; i<size;++i)
    {
        in >> tmp;
        primeSubset.insert(tmp);
    }
}
void ProofCalcJob::writeToStream(ostream &out)
{
    out << primeAll.size();
    for(SetItType it = primeAll.begin(); it != primeAll.end(); ++it)
    {
        out << " " << *it;
    }
    out << " " << primeSubset.size();
    for(SetItType it = primeSubset.begin(); it != primeSubset.end(); ++it)
    {
        out << " " << *it;
    }
}
string ProofCalcJob::run()
{
    ostringstream oStr;
    oStr << RSAAccumulatorService::getRSAAccumulator()->publicGenSubsetProof(primeAll, primeSubset);
    return oStr.str();
}
void ProofCalcJob::setAll(SetType z)
{
    primeAll = z;
}
void ProofCalcJob::setSubset(SetType z)
{
    primeSubset = z;
}

void NonProofCalcJob::readFromStream(istream &in)
{
    int size;
    ZZ tmp;
    primeAll.clear();
    in >> size;
    for(int i=0; i<size;++i)
    {
        in >> tmp;
        primeAll.insert(tmp);
    }
    in >> v;
}

void NonProofCalcJob::writeToStream(ostream &out)
{
    out << primeAll.size();
    for(SetItType it = primeAll.begin(); it != primeAll.end(); ++it)
    {
        out << " " << *it;
    }
    out << " " << v;
}

string NonProofCalcJob::run()
{
    ostringstream oStr;
    RSAAccumulator *accu = RSAAccumulatorService::getRSAAccumulator();
    accu->setSet(primeAll);
    ZZ a, d;
    if(!accu->publicGenNonProof(v, a, d))
    {
        cerr << "Non Proof Job Failed to public gen non proof" << endl;
    }
    oStr << a << " " << d;
    return oStr.str();
}
void NonProofCalcJob::setAll(SetType z)
{
    primeAll = z;
}
void NonProofCalcJob::setV(NTL::ZZ v)
{
    this->v = v;
}

istream & operator >> (istream &in, NonProofResult &res)
{
    in >> res.a >> res.d;
    return in;
}

ostream & operator << (ostream &out, NonProofResult &res)
{
    out << res.a << " " << res.d;
    return out;
}
