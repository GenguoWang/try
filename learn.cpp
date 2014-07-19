#include <iostream>
#include <string>
#include <vector>
#include <cassert>
using namespace std;
class Test
{
public:
    Test(const Test&)
    {
        cout << "what" << endl;
        v = 100;
    }
    Test(int a)
    {
        cout << "int" << endl;
        v = a;
    }
    Test()
    {
    }
    int v;
};
void testCopyConstructor()
{
    Test t2 = 10;//optimized by compiler, copy construct will not be used, but is necessary
    cout << t2.v << endl; //10
    Test t3(t2);//copy construct will be userd
    cout << t3.v << endl; //100
    assert(t2.v == 10);
    assert(t3.v == 100);
}
int main(int argc, char *argv[])
{
    testCopyConstructor();
	return 0;
}

