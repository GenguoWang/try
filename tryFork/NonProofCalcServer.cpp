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


void startNonProofCalcServer(int port);
void http_handle(ManageProccessorWrapper<NonProofCalcJob, NonProofResult> *proccessor, HttpRequest *req, HttpResponse *resp);

int main(int argc, char *argv[])
{
    try
    {
        int port;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("port,p",  po::value<int>(&port)->default_value(8500),"specify port to give http accessory")
            ;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
        startNonProofCalcServer(port);
    }
    catch(exception &ex)
    {
        cerr << ex.what();
    }
	return 0;
}

void startNonProofCalcServer(int port)
{
    cout << port << endl;
    HttpServer server(port);
    ManageProccessorWrapper<NonProofCalcJob, NonProofResult> proccessor("main",5);
    boost::function<void(HttpRequest *, HttpResponse *)> handle;
    handle = boost::bind(http_handle, &proccessor, _1, _2);
    server.run(handle);
}

void http_handle(ManageProccessorWrapper<NonProofCalcJob, NonProofResult> *proccessor, HttpRequest *req, HttpResponse *resp)
{
    string jobStr = req->getFormString("job");
    if (jobStr!="")
    {
        string proofStr = proccessor->calcProof(jobStr);
        resp->write(proofStr);
    }
}

