#include "threadpool.hpp"
#include <iostream>
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
    *val = 100;
}
void testSchedule(int &val)
{
    pool tp(2);
    tp.schedule(boost::bind(first_task,1));
    tp.schedule(boost::bind(second_task,2));
    tp.schedule(boost::bind(third_task,&val));
}
int main()
{
    /*
    tp.schedule(&first_task);
    tp.schedule(&second_task);
    tp.schedule(&third_task);
    */
    int val=4;
    testSchedule(val);
    cout << val << endl;
    return 0;
}
