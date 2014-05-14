#include <iostream>
#include <vector>
#include <cstdio>
using namespace std;
class Base1
{
public:
    int b1;
    virtual void print(){cout<<"Base1"<<endl;}
};
class Base2
{
public:
    int b2;
    virtual void print(){cout<<"Base2"<<endl;}
};
class A:public Base2,public Base1
{
int a;
public:
    //virtual void print(){cout<<"A"<<endl;}
};
int main(int argc,char * argv[])
{
    cout << sizeof(Base1) <<endl;
    cout << sizeof(Base2) <<endl;
    cout << sizeof(A) <<endl;
    Base2 *p = new A();
    p->print();
    Base1 *pp = new A();
    ((Base2*)pp)->print();
    A* a = new A();
    a->b1 = 1;
    a->b2 = 2;
    ((Base1*)a)->print();
    ((Base1*)(Base2*)a)->print();
    cout << a << endl
    <<(Base1*)a<<endl
    <<(Base2*)a<<endl;
    cout << a->b1 << endl
    <<((Base1*)a)->b1<<endl
    <<((Base1*)(Base2*)a)->b1<<endl;
    return 0;

}
