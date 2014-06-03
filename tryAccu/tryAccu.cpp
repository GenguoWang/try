#include "RSAAccumulator.h"
#include "PrimeRep.h"
#include <NTL/ZZ.h>
#include <set>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <ctime>
#include <numeric>
#include <sys/time.h>
#include "PrimeManager.h"
#include "TreeProof.h"
#include "TwoLevelProof.h"
using std::cout;
using std::endl;
const int SET_SIZE=10000;
//const int SUB_SIZE=9000;
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
    //PrimeRep pr(64, a64, b64, c64, prime64, ar64);

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
    double timeMat1[10][10];
    double timeMat2[10][10];
    double timeMat3[10][10];
    double timeMat4[10][10];
    for(int iE=2;iE<8;++iE)
    {
        TreeProof tp(iE,pr);
        tp.init(allSet,a);
        for(int i=0;i<10;++i)timeMat1[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat2[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat3[iE][i]= 0;
        for(int i=0;i<10;++i)timeMat4[iE][i]= 0;
        for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
        {
            for(int ii=0;ii<10;++ii)
            {
                cout << "test:" << iE << " " << testNum <<caseNum<< " " << ii<< endl;
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
                tp.buildProof(subSet,a);
                time_point1 = get_time();
                tp.verifyProof(subSet,a);
                time_point2 = get_time();
                double buildtime1 = time_point1-start_time;
                double prooftime1 = time_point2-time_point1;

                ZZ z,proof;

                start_time = get_time();
                std::set<ZZ> primeAll = tp.pm.getPrime(allSet);
                std::set<ZZ> primeSubset = tp.pm.getPrime(subSet);
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
                timeMat1[iE][caseNum] += buildtime1;
                timeMat2[iE][caseNum] += prooftime1;
                timeMat3[iE][caseNum] += buildtime2;
                timeMat4[iE][caseNum] += prooftime2;
                cout << "tick: " << buildtime1 << " " <<prooftime1 << " "
                << buildtime2 << " " << prooftime2 << endl;
            }
            timeMat1[iE][caseNum] /= 10;
            timeMat2[iE][caseNum] /= 10;
            timeMat3[iE][caseNum] /= 10;
            timeMat4[iE][caseNum] /= 10;
            cout << "size: " << testNum 
            << " tree: " << timeMat1[iE][caseNum] << " " << timeMat2[iE][caseNum]
            <<" origin: " << timeMat3[iE][caseNum]<< " " << timeMat4[iE][caseNum] << endl;
        }
    }
    for(int iE=2;iE<8;++iE)
    {
        cout << "e=" << iE << endl;
        for(int testNum = SET_SIZE/100,caseNum=0; testNum < SET_SIZE; testNum *= 2,caseNum++)
        {
            cout << "size: " << testNum 
            << " tree: " << timeMat1[iE][caseNum] << " " << timeMat2[iE][caseNum]
            <<" origin: " << timeMat3[iE][caseNum]<< " " << timeMat4[iE][caseNum] << endl;
        }
    }
    return 0;
}
