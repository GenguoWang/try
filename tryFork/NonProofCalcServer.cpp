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
#include "ProofCalc.h"
using namespace std;
namespace po = boost::program_options;


void startNonProofCalcServer(int port, int worker);
void http_handle(ManageProccessorWrapper<SubsetNonProofCalcJob, NonProofResult> *proccessor, HttpRequest *req, HttpResponse *resp);

int main(int argc, char *argv[])
{
    try
    {
        int port,worker;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("port,p",  po::value<int>(&port)->default_value(8500),"specify port to give http accessory")
            ("worker,w",  po::value<int>(&worker)->default_value(5),"specify worker number")
            ;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
        startNonProofCalcServer(port, worker);
    }
    catch(exception &ex)
    {
        cerr << ex.what();
    }
	return 0;
}

void startNonProofCalcServer(int port, int worker)
{
    cout << port << endl;
    HttpServer server(port);
    ManageProccessorWrapper<SubsetNonProofCalcJob, NonProofResult> proccessor("main", worker);
    boost::function<void(HttpRequest *, HttpResponse *)> handle;
    handle = boost::bind(http_handle, &proccessor, _1, _2);
    server.run(handle);
}

void http_handle(ManageProccessorWrapper<SubsetNonProofCalcJob, NonProofResult> *proccessor, HttpRequest *req, HttpResponse *resp)
{
    string jobStr = req->getFormString("job");
    if (jobStr!="")
    {
        string proofStr = proccessor->calcProof(jobStr);
        resp->write(proofStr);
    }
}

