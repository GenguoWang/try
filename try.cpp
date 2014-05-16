#include <iostream>
#include <vector>
#include <cstdio>
using namespace std;
class String
{
private:
    char data[10];
public:
    operator char *()
    {
        return data;
    }
    ~String()
    {
        data[0] = 'w';
        data[1] = '\0';
        cout << "DD" << endl;
    }
    friend String operator +(String &, String &);
};
String operator +(String& s1, String &s2)
{
    String str;
    str.data[0] = 'n';
    str.data[1] = '\0';
    return str;
}
int main(int argc,char * argv[])
{
    String s1, s2;
    cout << s1 + s2;
    return 0;
}
