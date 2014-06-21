#include <iostream>
#include <string>
#include <vector>
using namespace std;
template<typename _T>
class Test
{
    
public:
    void output();
};

template<typename _T>
void Test<_T>::output()
{
    _T a=10;
    cout << a;
}
int main(int argc, char *argv[])
{
    Test<int> test;
    test.output();
	return 0;
}

