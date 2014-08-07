#include <iostream>
#include <vector>
#include <algorithm>
#include <google/profiler.h>
using namespace std;
void testMath()
{
    int ans = 0;
    for(int i=0;i<100000000;++i)
    {
        ans += i;
        ans %= 10000007;
    }
    cout << ans << endl;
}
int main()
{
    ProfilerStart("wgg.prof");
    cout << "hello world" << endl;
    testMath();
    ProfilerStop();
    return 0;
}
