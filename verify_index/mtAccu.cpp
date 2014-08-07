#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "util.h"
#include "boost/threadpool.hpp"
#include "boost/bind.hpp"
using namespace std;
using namespace boost::threadpool;
template<typename _T>
double add(const _T &todo)
{
    double result = 1.0;
    for(typename _T::const_iterator it = todo.begin();it != todo.end();++it)
    {
        result *= *it;
    }
    return result;
}

void dowork(const vector<double> &todo, double &result)
{
    result = add(todo);
}
double mtadd(const set<double> &todo, int n)
{
    double bb = get_time();
    pool tp(n);
    vector<vector<double> > v(n);
    vector<double> result(n);
    int cnt=0;
    double b0 = get_time();
    for(set<double>::const_iterator it = todo.begin();it != todo.end();++it)
    {
        v[cnt%n].push_back(*it);
        cnt++;
    }
    double b = get_time();
    for(int i=0;i<n;++i)
    {
        tp.schedule(boost::bind(dowork,boost::cref(v[i]),boost::ref(result[i])));
    }
    tp.wait();
    double r=1.0;
    for(int i=0;i<n;++i)
    {
        r*=result[i];
    }
    double e = get_time();
    cout << "Ti:" <<b0-bb << " " <<b-b0 << " "<< e-b << endl;
    return r;
}
int main(int argc, char *argv[])
{
    set<double> work;
    double b0 = get_time();
    for(int i=0;i<5000000;++i) work.insert(1+1e-12*i);
    double b = get_time();
    cout << add(work) << endl;
    double e = get_time();
    cout << b-b0 <<" " << e-b<<endl;
    b = get_time();
    cout << mtadd(work,4) << endl;
    e = get_time();
    cout << e-b<<endl;
	return 0;
}

