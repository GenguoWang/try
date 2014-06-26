#ifndef _PROOFCALC_H_
#define _PROOFCALC_H_

#include <string>
#include <vector>
#include <set>
#include <NTL/ZZ.h>
#include "RSAAccumulator.h"
#include "HttpClient.h"
#include "Processor.h"
#define BUF_SIZE 10000
class RSAAccumulatorService
{
private:
    static RSAAccumulator *a;
public:
    static RSAAccumulator *getRSAAccumulator();
};
class ProofCalcJob:public JobInterface
{
private:
    typedef std::set<NTL::ZZ> SetType;
    typedef SetType::iterator SetItType;
    SetType primeAll;
    SetType primeSubset;
public:
    virtual void readFromStream(std::istream &in);
    virtual void writeToStream(std::ostream &out);
    virtual std::string run();
    void setAll(SetType z);
    void setSubset(SetType z);
};

class ManageProccessorWrapper
{
private:
    int manageFd;
public:
    ManageProccessorWrapper(std::string name, int numOfWorker);
    std::vector<NTL::ZZ> calcProof(std::vector<ProofCalcJob> jobList);
    std::string calcProof(const std::string &jobStr);
};

class ProofCalcClient
{
private:
    HttpClient hc;
public:
    std::vector<NTL::ZZ> calcProof(std::string url, std::vector<ProofCalcJob> jobList);
};
#endif

