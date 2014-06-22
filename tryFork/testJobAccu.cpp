#include <iostream>
#include <string>
#include <vector>
#include "Processor.h"
#include "RSAAccumulator.h"
using namespace std;
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
class AccuJob:public JobInterface
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

int main(int argc, char *argv[])
{
    int fd = ManageProcessor<AccuJob>::createManager("main",5);
    AccuJob job;
    std::set<ZZ> primeAll,primeSubset;
    ZZ a;
    a = 7;
    primeAll.insert(a);
    primeSubset.insert(a);
    a = 11;
    primeAll.insert(a);
    primeSubset.insert(a);
    job.setSubset(primeSubset);
    a = 13;
    primeAll.insert(a);
    job.setAll(primeAll);
    ostringstream oStr;
    oStr << 1 << " ";
    job.writeToStream(oStr);
    string str = oStr.str();
    write(fd,str.c_str(),str.size()+1);
    char buf[1000];
    read(fd,buf,1000);
    string res(buf);
    cout << res<<endl;
    ZZ gt = RSAAccumulatorService::getRSAAccumulator()->publicGenSubsetProof(primeAll, primeSubset);
    cout << gt << endl;
    return 0;
}

