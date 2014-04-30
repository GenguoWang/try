#include <iostream>
using namespace std;
class Base
{
public:
    Base()
    {
        cout << "con" << endl;
    }
    virtual void output()
    {
        cout << "here" << endl;
    }
};

class A:public Base
{
public:
    virtual void output()
    {
        cout << "a" << endl;
    }
};

void todo(Base b)
{
    b.output();
}
int main()
{
    Base b;
    todo(b);
    A a;
    todo(a);
    enum wgg{BB,CC=0};
    wgg t=CC;
    cout << t<<(BB==CC) << endl;
}
