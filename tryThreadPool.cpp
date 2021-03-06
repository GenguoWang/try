#include "threadpool.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <google/profiler.h>
using namespace boost::threadpool;
using namespace std;

void first_task(int val)
{
    cout << "here1" <<val<< endl;
}
void second_task(int val)
{
    cout << "here2" <<val<< endl;
}
void third_task(int* val)
{
    cout << "here3" <<*val<< endl;
    sleep(1);
    *val = 100;
}
void fourth_task(int* val)
{
    cout << "here4" <<*val<< endl;
}
void testSchedule(int &val)
{
    /*
    // no parameter
    tp.schedule(&first_task);
    tp.schedule(&second_task);
    tp.schedule(&third_task);
    */
    pool tp(2);
    tp.schedule(boost::bind(first_task,1));
    tp.schedule(boost::bind(second_task,2));
    tp.schedule(boost::bind(third_task,&val));
    tp.wait();
    tp.schedule(boost::bind(fourth_task,&val));
}
class wgg
{
public:
    int a;
    void run()
    {
        cout << "run: "<<a << endl;
    }
    void foo(int bar)
    {
        cout << "foo: " << bar << endl;
    }
    void foo(float bar)
    {
        cout << "foofloat: " << bar << endl;
    }
};
void testWithClass()
{
    pool tp(3);
    wgg test;
    test.a = 1001;
    tp.schedule(boost::bind(&wgg::run,&test));
    tp.schedule(boost::bind(static_cast< void (wgg::*) (int) >(&wgg::foo),&test,10));
    tp.schedule(boost::bind(static_cast< void (wgg::*) (float) >(&wgg::foo),&test,10.01f));
}
void setInt(int *pos,int value)
{
    *pos = value;
}
void testWork()
{
    pool tp(3);
    vector<int> v(10);
    for(int i=0;i<10;++i)
    {
        tp.schedule(boost::bind(setInt,&v[i],i));
    }
    tp.wait();
    for(int i=0;i<10;++i)
    {
        cout << v[i] << '\t';
    }
    cout << endl;

}
void consumeSomeCPUTime1(int input){ 
  int i = 0; 
  input++; 
  while(i++ < 10000){ 
    i--;  i++;  i--;  i++; 
  } 
 }; 

 void consumeSomeCPUTime2(int input){ 
  input++; 
  consumeSomeCPUTime1(input); 
  int i = 0; 
  while(i++ < 10000){ 
    i--;  i++;  i--;  i++; 
  } 
 }; 

 int stupidComputing(int a, int b){ 
  int i = 0; 
  while( i++ < 10000){  
    consumeSomeCPUTime1(i); 
  } 
  int j = 0; 
  while(j++ < 5000){ 
    consumeSomeCPUTime2(j); 
  } 
  return a+b; 
 }; 

 int smartComputing(int a, int b){ 
  return a+b; 
 }; 
int main()
{
    ProfilerStart("threadpool.prof");
    int val=4;
    testSchedule(val);
    testWithClass();
    testWork();
    cout << val << endl;
    /*
int i=0;
while( i++ < 10){ 
    printf("Stupid computing return : %d\n",stupidComputing(i, i+1)); 
    printf("Smart computing return %d\n",smartComputing(i+1, i+2)); 
  }
  */
    sleep(4);
    ProfilerStop();
    return 0;
}
