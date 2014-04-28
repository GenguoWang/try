#include <iostream>
#include <vector>
using namespace std;
struct Node
{
public:
    int value;
    Node * pNode;
    ~Node()
    {
        value = -199;
    }
};
int main()
{
    vector<Node> v(2);
    v[0].pNode = &v[1];
    v[1].value = 199;
    cout << v[0].pNode << ' ' << &v[1] << endl;
    cout << "value: " << v[0].pNode->value << endl;
    cout << "capacity: " << v.capacity() << endl;
    v.push_back(Node());
    cout << v[0].pNode << ' ' << &v[1] << endl;
    cout << "value: " << v[0].pNode->value << endl;
    cout << "capacity: " << v.capacity() << endl;
    return 0;
}
