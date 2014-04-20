#include <iostream>
#include <vector>
using namespace std;
struct node
{
    int val;
    node* p;
};
int main()
{
    vector<node> tt(2);
    tt[0].p = &tt[1];
    tt[1].val =2;
    tt.push_back(node());
    cout<<tt.capacity()<<endl;
    tt.push_back(node());
    tt.push_back(node());
    tt.push_back(node());
    tt.push_back(node());
    tt.push_back(node());
    cout<<tt.capacity()<<endl;
    tt.push_back(node());
    cout << tt[0].p->val <<endl;
    return 0;
}
