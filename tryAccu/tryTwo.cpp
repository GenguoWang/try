#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include <NTL/ZZ.h>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <ctime>
#include <numeric>
#include <sys/time.h>
#include "PrimeManager.h"
#include "TreeProof.h"
#include "VTwoLevelProof.h"
using std::cout;
using std::endl;
const int SET_SIZE=1000;
//const int SUB_SIZE=9000;
const int NUM=10;
double get_time()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + static_cast<double>(tv.tv_usec*1e-6);
}
int main()
{
    init_prime_rep_32_64();
    PrimeRep pr(32, a32, b32, c32, prime32, ar32);

    init_rsa1024_accu_param();
    RSAAccumulator a(rsaP, rsaQ, rsaG);

    std::set<ZZ> allSet;
    ZZ tmp;
    for(int i=1;i<=SET_SIZE;++i)
    {
        tmp = i;
        allSet.insert(tmp);   
    }
    std::set<ZZ> primeAll = pr.getPrime(allSet);
    ZZ accu = a.privateAccumulate(primeAll);
    PrimeManager pm(pr);

    double timeMat1[10];
    double timeMat2[10];
    double timeMat3[10];
    double timeMat4[10];
    double psize[10];
    double init_start = get_time();
    VTwoLevelProof tlp(primeAll,NUM,pm,a);
    ofstream out("wgg.dat");
    tlp.saveToStream(out);
    out.close();

    /*
    ifstream in("wgg.dat");
    VTwoLevelProof tlp(primeAll,NUM,in,a);
    in.close();
    */

    double init_end = get_time();
    cout << "inited,time:" << init_end - init_start << endl;

    for(int i=0;i<10;++i)timeMat1[i]= 0;
    for(int i=0;i<10;++i)timeMat2[i]= 0;
    for(int i=0;i<10;++i)timeMat3[i]= 0;
    for(int i=0;i<10;++i)timeMat4[i]= 0;
    for(int i=0;i<10;++i)psize[i]= 0;
    for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
    {
        for(int ii=0;ii<10;++ii)
        {
            cout << "test:" <<  " " << testNum <<caseNum<< " " << ii<< endl;
            std::set<ZZ> subSet;
            srand(time(0));
            for(int i=SET_SIZE-testNum+1;i<=SET_SIZE;++i)
            {
                tmp = rand()%i+1;
                if(subSet.find(tmp)==subSet.end())
                {
                    subSet.insert(tmp);
                }
                else
                {
                    tmp = i;
                    subSet.insert(tmp);
                }
            }
            double start_time,time_point1,time_point2;
            start_time = get_time();
            tlp.buildProof(pm.getPrime(subSet));
            time_point1 = get_time();
            psize[caseNum] += tlp.getProofSize();
            tlp.verifyProof(pm.getPrime(subSet));
            time_point2 = get_time();
            double buildtime1 = time_point1-start_time;
            double prooftime1 = time_point2-time_point1;

            ZZ z,proof;

            start_time = get_time();
            std::set<ZZ> primeAll = pm.getPrime(allSet);
            std::set<ZZ> primeSubset = pm.getPrime(subSet);
            proof = a.publicGenSubsetProof(primeAll, primeSubset);
            time_point1 = get_time();
            if(a.verifySubsetProof(primeSubset, proof, accu))
            {
                std::cout << "OK" << std::endl;
            }
            else
            {
                std::cout << "Failed" << std::endl;
            }
            time_point2 = get_time();
            double buildtime2 = time_point1-start_time;
            double prooftime2 = time_point2-time_point1;
            timeMat1[caseNum] += buildtime1;
            timeMat2[caseNum] += prooftime1;
            timeMat3[caseNum] += buildtime2;
            timeMat4[caseNum] += prooftime2;
            cout << "tick: " << buildtime1 << " " <<prooftime1 << " "
            << buildtime2 << " " << prooftime2 << endl;
        }
        timeMat1[caseNum] /= 10;
        timeMat2[caseNum] /= 10;
        timeMat3[caseNum] /= 10;
        timeMat4[caseNum] /= 10;
        psize[caseNum] /= 10;
        cout << "size: " << testNum 
        << " tree: " << timeMat1[caseNum] << " " << timeMat2[caseNum]
        <<" origin: " << timeMat3[caseNum]<< " " << timeMat4[caseNum] << endl;
    }
    cout << "result:" <<  endl;
    cout << "size\tpsize\ttree\t\torigin"<<endl;
    for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
    {
        cout << testNum 
        << "\t" << psize[caseNum] <<"\t" <<timeMat1[caseNum] << "\t" << timeMat2[caseNum]
        <<"\t" << timeMat3[caseNum]<< "\t" << timeMat4[caseNum] << endl;
    }
    return 0;
}
