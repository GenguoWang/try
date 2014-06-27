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
using namespace std;
#include "ProofCalc.h"
void testClient()
{
    vector<ZZ> result;
    vector<ProofCalcJob> jobList;
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
    int Sub=2;
    int All=5;
    int size=primes.size();
    std::set<ZZ> sub;
    std::set<ZZ> all;
    ZZ tmp;
    srand(time(NULL));
    for(int i=0;i<NUM;++i)
    {
        ProofCalcJob job;
        for(int j=0;j<All;++j)
        {
            int index = rand()%size;
            tmp = primes[index];
            if(j<Sub)sub.insert(tmp);
            all.insert(tmp);
        }
        job.setAll(all);
        job.setSubset(sub);
        jobList.push_back(job);
        result.push_back(RSAAccumulatorService::getRSAAccumulator()->publicGenSubsetProof(all, sub));
    }
    ProofCalcClient<ProofCalcJob, ZZ> client;
    vector<ZZ> workOut = client.calcProof("http://localhost:8400",jobList);
    assert(NUM==workOut.size());
    for(int i=0;i<NUM;++i)
    {
        assert(workOut[i]==result[i]);
    }
}
int main(int argc, char *argv[])
{
    testClient();
	return 0;
}

