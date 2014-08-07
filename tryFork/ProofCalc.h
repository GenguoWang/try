#ifndef _PROOFCALC_H_
#define _PROOFCALC_H_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <ZZ.h>
#include "RSAAccumulator.h"
#include "HttpClient.h"
#include "Processor.h"
class RSAAccumulatorService
{
private:
    static RSAAccumulator *a;
public:
    static RSAAccumulator *getRSAAccumulator();
};
class ProofCalcJob:public JobInterface
{
protected:
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

class NonProofCalcJob:public JobInterface
{
private:
    typedef std::set<NTL::ZZ> SetType;
    typedef SetType::iterator SetItType;
    SetType primeAll;
    NTL::ZZ v;
public:
    virtual void readFromStream(std::istream &in);
    virtual void writeToStream(std::ostream &out);
    virtual std::string run();
    void setAll(SetType z);
    void setV(NTL::ZZ v);
};

class SubsetNonProofCalcJob:public ProofCalcJob
{
public:
    virtual std::string run();
};

class NonProofResult
{
public:
    ZZ a, d;
};
istream & operator >> (istream &in, NonProofResult &res);
ostream & operator << (ostream &out, NonProofResult &res);

template<typename JobType, typename ResultType>
class ManageProccessorWrapper
{
private:
    int manageFd;
    static const int BUF_SIZE=1048576;
public:
    ManageProccessorWrapper(std::string name, int numOfWorker)
    {
        manageFd = ManageProcessor<JobType>::createManager("main", numOfWorker);
    }
    std::vector<ResultType> calcProof(std::vector<JobType> jobList)
    {
        int size = jobList.size();
        std::ostringstream oStr;
        oStr << size;
        for(typename std::vector<JobType>::iterator it = jobList.begin(); it != jobList.end(); ++it)
        {
            oStr << " ";
            (*it).writeToStream(oStr);
        }
        std::string resStr = calcProof(oStr.str());
        std::istringstream iStr(resStr);
        std::vector<ResultType> proofList;
        for(int i=0;i<size;++i)
        {
            ResultType tmp;
            iStr >> tmp;
            proofList.push_back(tmp);
        }
        return proofList;
    }
    std::string calcProof(const std::string &jobStr)
    {
        write(manageFd, jobStr.c_str(), jobStr.size()+1);
        char buf[BUF_SIZE+1]; //leave space to add tail '\0'
        int num;
        std::ostringstream oStr;
        while(true)
        {
            num =  read(manageFd, buf, BUF_SIZE);
            buf[num] = '\0';//add '\0' for output
            oStr << buf;
            if(num==0 || buf[num-1]=='\0')break; //if already end with '\0', means have read all data
        }
        return oStr.str();
    }
};

template<typename JobType, typename ResultType>
class ProofCalcClient
{
private:
    HttpClient hc;
public:
    std::vector<ResultType> calcProof(std::string url, std::vector<JobType> jobList)
    {
        int size = jobList.size();
        std::ostringstream oStr;
        oStr << size;
        for(typename vector<JobType>::iterator it = jobList.begin(); it != jobList.end(); ++it)
        {
            oStr << " ";
            (*it).writeToStream(oStr);
        }
        std::map<string,string> kv;
        kv["job"] = oStr.str();
        std::string resStr = hc.post(url, kv);
        std::istringstream iStr(resStr);
        vector<ResultType> proofList;
        for(int i=0;i<size;++i)
        {
            ResultType tmp;
            iStr >> tmp;
            proofList.push_back(tmp);
        }
        return proofList;
    }

};
#endif

