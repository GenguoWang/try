#include <iostream>
#include <vector>
using namespace std;
class BigNum
{
public:
    static const int SIGN_POSITIVE = 0;
    static const int SIGN_NEGTIVE = 1;
    BigNum():size(1),sign(SIGN_POSITIVE){data.push_back(0);}
    BigNum(int value)
    {
        if(value==0)
        {
            size = 1;
            sign = SIGN_POSITIVE;
            data.push_back(0);
        }
        else
        {
            size = 0;
            if(value>0) sign = SIGN_POSITIVE;
            else
            {
                value = -value;
                sign = SIGN_NEGTIVE;
            }
            int temp;
            while(value>0)
            {
                temp = value % BASE;
                value /= BASE;
                size++;
                data.push_back(temp);
            }
        }
    }
    BigNum operator + (const BigNum &num)
    {
        BigNum res;
    }
private:
    static const int BASE = 10000;
    vector<unsigned int> data;
    int size;
    int sign;
    friend ostream& operator << (ostream& out,const BigNum & num);
};
ostream& operator << (ostream& out,const BigNum & num)
{
    if(num.sign == BigNum::SIGN_NEGTIVE) out << "-";
    out << num.data[num.size-1];
    for(int i=num.size-2;i>=0;--i)
    {
        unsigned int temp = num.data[i];
        if(temp<10) out << "000";
        else if(temp<100) out << "00";
        else if(temp<1000) out << '0';
        out << temp;
    }
    return out;
}
int main()
{
    BigNum num(1230067);
    cout << num << endl;
    return 0;
}
