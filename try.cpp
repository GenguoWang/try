#include <iostream>
#include <vector>
using namespace std;
class Ttt
{
   public:
   ~Ttt()
   {
    cout <<"dd"<<endl;
   }
};
struct Wgg
{
    
    int w;
    char c;
    Wgg * next;
};
int main(int argc,char * argv[])
{
    Ttt * p = new Ttt[10];
    //delete [] p;
    Ttt *q = p;
    //delete  [] (q+1);
    cout << "hello github" << endl;
    cout << sizeof(Wgg) << endl;
    vector<Wgg> nn(3);
    nn[0].next = &nn[1];
    nn[1].w = 2;
    cout << &nn[0] << ' ' <<&nn[1] << endl;
    cout << &nn[0] << ' ' <<&nn[1] << endl;
    cout << &nn[0].next  << endl;
    cout << nn[0].next->w  << endl;
    cout << &nn[1] - &nn[0] << endl;
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    nn.push_back(Wgg());
    cout << &nn[0] <<' '<< &nn[1] << endl;
    cout << &nn[0].next  << endl;
    cout << nn[0].next->w  << endl;
    cout << nn[1].w  << endl;
}
