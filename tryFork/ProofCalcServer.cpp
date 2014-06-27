#include <iostream>
#include <string>
#include <vector>
#include "HttpServer.h"
#include "Processor.h"
#include "RSAAccumulator.h"
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <ctime>
using namespace std;
namespace po = boost::program_options;


#include "ProofCalc.h"
void testWrapper()
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
    int NUM=10;
    int Sub=3;
    int All=10;
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
    ManageProccessorWrapper<ProofCalcJob, ZZ> proccessor("main",3);
    vector<ZZ> workOut = proccessor.calcProof(jobList);
    assert(NUM==workOut.size());
    for(int i=0;i<NUM;++i)
    {
        assert(workOut[i]==result[i]);
    }
}
void startProofCalcServer(int port);
void http_handle(ManageProccessorWrapper<ProofCalcJob, ZZ> *proccessor, HttpRequest *req, HttpResponse *resp);

int main(int argc, char *argv[])
{
    //testWrapper();
    try
    {
        int port;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("port,p",  po::value<int>(&port)->default_value(8400),"specify port to give http accessory")
            ;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
        startProofCalcServer(port);
    }
    catch(exception &ex)
    {
        cerr << ex.what();
    }
	return 0;
}

void startProofCalcServer(int port)
{
    cout << port << endl;
    HttpServer server(port);
    ManageProccessorWrapper<ProofCalcJob, ZZ> proccessor("main",5);
    boost::function<void(HttpRequest *, HttpResponse *)> handle;
    handle = boost::bind(http_handle, &proccessor, _1, _2);
    server.run(handle);
}

void http_handle(ManageProccessorWrapper<ProofCalcJob, ZZ> *proccessor, HttpRequest *req, HttpResponse *resp)
{
    string jobStr = req->getFormString("job");
    if (jobStr!="")
    {
        string proofStr = proccessor->calcProof(jobStr);
        resp->write(proofStr);
    }
}
