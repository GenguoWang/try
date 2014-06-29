#include "HttpServer.h"
#include "Processor.h"
#include "RSAAccumulator.h"
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "ProofCalc.h"
using namespace std;
void testClient()
{
    vector<NonProofResult> result;
    vector<SubsetNonProofCalcJob> jobList;
    vector<int> primes;
    int arr[10000];
    for(int i = 0; i<10000; ++i) arr[i]=0;
    for(int i=2; i<101;++i)
    {
        for(int j=2;i*j<10000;++j)
        {
            arr[i*j] = 1;
        }
    }
    for(int i = 2; i<10000; ++i) if(arr[i]==0) primes.push_back(i);
    int NUM=48;
    int All=50;
    int Sub=10;
    int size=primes.size();
    std::set<ZZ> all;
    std::set<ZZ> sub;
    ZZ tmp;
    srand(time(NULL));
    RSAAccumulator *accu = RSAAccumulatorService::getRSAAccumulator();
    for(int i=0;i<NUM;++i)
    {
        SubsetNonProofCalcJob job;
        for(int j=0;j<All;++j)
        {
            int index = rand()%size;
            tmp = primes[index / 2 * 2];
            all.insert(tmp);
        }
        for(int j=0; j<Sub;++j)
        {
            int index = rand()%(size-1);
            tmp = primes[index / 2 * 2+1];
            sub.insert(tmp);
        }
        job.setAll(all);
        job.setSubset(sub);
        jobList.push_back(job);
        accu->setSet(all);
        NonProofResult oneResult;
        if(!accu->publicGenNonProof(sub, oneResult.a, oneResult.d))
        {
            cerr << "public gen failed!" << endl;
        }
        result.push_back(oneResult);
    }
    ProofCalcClient<SubsetNonProofCalcJob, NonProofResult> client;
    vector<NonProofResult> workOut = client.calcProof("http://localhost:8500",jobList);
    assert(NUM==workOut.size());
    for(int i=0;i<NUM;++i)
    {
        assert(workOut[i].a==result[i].a);
        assert(workOut[i].d==result[i].d);
    }
}
int main(int argc, char *argv[])
{
    testClient();
	return 0;
}

