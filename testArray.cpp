#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Base
{
public:
    int a;
};
class Sub:public Base
{
public:
    int b;
};

void pa(Base a)
{
    cout << a.a << endl;
}
void parr(Base a[], int size)
{
    for(int i = 0; i < size; ++i)
    {
        cout << a[i].a << endl;
    }
}
int main(int argc, char *argv[])
{
    Base b;
    pa(b);
    Base c[10];
    for(int i=0; i < 10; ++i)
    {
        c[i].a = i;
    }
    parr(c, 10);
	return 0;
}

