#include <iostream>
#include <string>
#include <vector>
#include <cassert>
using namespace std;
/*Constructor {{{*/
namespace TestConstruct
{

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

}
/*}}}*/
/*Inherit {{{*/
namespace Inherit
{

class A
{
public:
    int a;
    virtual char name(){return 'A';}
};
class B:virtual public A
{
public:
    virtual char name(){return 'B';}
};

class C:virtual public A
{
public:
    virtual char name(){return 'C';}
};

class D:public B, public C
{
public:
    virtual char name(){return 'D';}
};

int g = 123;
class Base
{
public:
    const string getName(){return getNameImpl();}
    int match(double x){return x;}
private:
    virtual const string getNameImpl(){return "Base";}
    int g;
    int match(int x){return x+1;}
};
class Derived:public Base
{
public:
    const string getNameImpl(){return "Derived";}
    int getInt()
    {
        //return g;//will hide Inherit::g, though Base::g is inaccessible here
        return Inherit::g;
    }
private:
};
char getChar(A* p) { return p->name();}
void test()
{
    A * p = new D;
    assert(getChar(p)=='D');
    B b;
    b.a = 100;
    static_cast<A>(b).a = 10;//staitc_cast will make temporary variable, b.a will not be changed
    assert(b.a == 100);
    Base *pB = new Derived();
    assert(pB->getName()=="Derived");//can override a private member of Base
    delete pB;
    Derived d;
    //d.match(1);//will choose Base::match(int), which is inaccessible
}

}
/*}}}*/
/*{{{ name finding*/
namespace NameFinding
{
    void print(int x){cout << "NameFinding int" <<x << endl;}
    namespace sub{ void print(int x){ cout << "sub:" << x << endl;}}
    //using sub::print;//error: there is already print_int in this scope
    using namespace sub;//OK! we can see sub::print now
    class Base 
    {
    private:
        int x;
    public:
        void mf1(int v){cout << "base mf1:" << v<<endl;}
    };
    class Derived: public Base
    {
    public:
        void mf1(int b){cout << "Derived mf1"<<b<<endl;}
        using Base::mf1;//!!not like namespace, it's ok here,just make Base::mf1 visible,it will just be hided by Derived::mf1
    };

    namespace nested
    {
        void print(double x){cout << "nested double" << x << endl;}
        void test()
        {
            print(10);
            using namespace sub;
            print(10);//print(double), because using namespace sub will make ::NameFinding::sub visible in ::NameFinding, ::NameFinding::nested::print will be find first
            using sub::print;
            print(10);//sub::print(int), using declaration just import the name to current scope, so sub::print(int) will be found
        }
        class NestedType{};
        void adl(NestedType x){cout << "adl" <<endl;}
    }
    //void adl(nested::NestedType x){cout << "adl out:"<<endl;}//will make test()::adl(x) ambiguious
    void test()
    {
        //print(10);//error, ambiguious:NameFinding::print or NameFinding::sub::print(import by using namespace sub)
        Derived d;
        d.mf1(10);
        nested::test();
        nested::NestedType x;
        adl(x);//arugment dependent lookup, will find nested::adl
    }
}
/*}}}*/
/*{{{Template*/
namespace Temp
{
template<typename _T>
_T add(_T a,_T b)
{
    return a+b;
}
template<>
int add(int a,int b)
{
    return a+b+1;
}

int add(int a,int b)
{
    return a+b+2;
}

template<typename _T>
class Base
{
public:
    string getName(){return "Base";}
};
template<typename _T>
class Derived:public Base<_T>
{
public:
    string getBaseName()
    {
        //return getName();//without using declaration, error,compiler will not find name in a templated Base
        //return Base<_T>::getName();//OK,tell compiler to find,but lost virtaul
        //using Base<_T>::getName // cannot use class member using declaration this way
        return this->getName();//or using a this-> to tell compiler, it's the best way
    }
    using Base<_T>::getName;//or using a declaration
};

void test()
{
    assert(add(1,2)==5);//Temp::add
    assert(add<int>(1,2)==4);//Temp::template<>int add
    assert(add(1.0,2)==5);//Temp::add, add(double,int) not match template, but it can call add(int,int)
    assert(add(1.0,2.0)==3);//Temp::template add
}

}
/*}}}*/
/*{{{main*/
int main(int argc, char *argv[])
{
    NameFinding::test();
    Inherit::test();
    Temp::test();
	return 0;
}
/*}}}*/
