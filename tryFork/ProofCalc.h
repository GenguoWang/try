#ifndef _PROOFCALC_H_
#define _PROOFCALC_H_

#include "HttpClient.h"
#define BUF_SIZE 10000
class RSAAccumulatorService
{
private:
    static RSAAccumulator *a;
public:
    static RSAAccumulator *getRSAAccumulator()
    {
        if(a==NULL)
        {
            init_rsa1024_accu_param();
            a = new RSAAccumulator(rsaP, rsaQ, rsaG);
        }
        return a;
    }
};
RSAAccumulator *RSAAccumulatorService::a = NULL;
class ProofCalcJob:public JobInterface
{
private:
    typedef std::set<ZZ> SetType;
    typedef SetType::iterator SetItType;
    SetType primeAll;
    SetType primeSubset;
public:
    virtual void readFromStream(std::istream &in)
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
    virtual void writeToStream(std::ostream &out)
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
    virtual std::string run()
    {
        std::ostringstream oStr;
        oStr << RSAAccumulatorService::getRSAAccumulator()->publicGenSubsetProof(primeAll, primeSubset);
        return oStr.str();
    }
    void setAll(SetType z)
    {
        primeAll = z;
    }
    void setSubset(SetType z)
    {
        primeSubset = z;
    }
};

class ManageProccessorWrapper
{
private:
    int manageFd;
public:
    ManageProccessorWrapper(string name, int numOfWorker)
    {
        manageFd = ManageProcessor<ProofCalcJob>::createManager("main", 5);
    }
    vector<ZZ> calcProof(vector<ProofCalcJob> jobList)
    {
        int size = jobList.size();
        ostringstream oStr;
        oStr << size;
        for(vector<ProofCalcJob>::iterator it = jobList.begin(); it != jobList.end(); ++it)
        {
            oStr << " ";
            (*it).writeToStream(oStr);
        }
        string resStr = calcProof(oStr.str());
        istringstream iStr(resStr);
        vector<ZZ> proofList;
        ZZ tmp;
        for(int i=0;i<size;++i)
        {
            iStr >> tmp;
            proofList.push_back(tmp);
        }
        return proofList;
    }
    string calcProof(const string &jobStr)
    {
        write(manageFd, jobStr.c_str(), jobStr.size()+1);
        char buf[BUF_SIZE];
        read(manageFd, buf, BUF_SIZE);
        string res(buf);
        return res;
    }
};

class ProofCalcClient
{
private:
    HttpClient hc;
public:
    vector<ZZ> calcProof(string url, vector<ProofCalcJob> jobList)
    {
        int size = jobList.size();
        ostringstream oStr;
        oStr << size;
        for(vector<ProofCalcJob>::iterator it = jobList.begin(); it != jobList.end(); ++it)
        {
            oStr << " ";
            (*it).writeToStream(oStr);
        }
        map<string,string> kv;
        kv["job"] = oStr.str();
        string resStr = hc.post(url, kv);
        istringstream iStr(resStr);
        vector<ZZ> proofList;
        ZZ tmp;
        for(int i=0;i<size;++i)
        {
            iStr >> tmp;
            proofList.push_back(tmp);
        }
        return proofList;
        
    }
};
#endif

