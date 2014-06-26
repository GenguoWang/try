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
void ProofCalcJob::readFromStream(std::istream &in)
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
void ProofCalcJob::writeToStream(std::ostream &out)
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
std::string ProofCalcJob::run()
{
    std::ostringstream oStr;
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

ManageProccessorWrapper::ManageProccessorWrapper(string name, int numOfWorker)
{
    manageFd = ManageProcessor<ProofCalcJob>::createManager("main", 5);
}
vector<ZZ> ManageProccessorWrapper::calcProof(vector<ProofCalcJob> jobList)
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
string ManageProccessorWrapper::calcProof(const string &jobStr)
{
    write(manageFd, jobStr.c_str(), jobStr.size()+1);
    char buf[BUF_SIZE];
    read(manageFd, buf, BUF_SIZE);
    string res(buf);
    return res;
}

vector<ZZ> ProofCalcClient::calcProof(string url, vector<ProofCalcJob> jobList)
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

