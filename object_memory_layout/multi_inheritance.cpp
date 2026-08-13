#include "pch.h"


class A
{
    int a = 0;
};

class B
{
    int b = 0;
};


class C : public B, public A
{
    int c = 0;
};

class D : public A, public B
{
    int d = 0;
};


class A1
{
public:
    virtual void funcA() {}

    int a = 0;
};

class B1
{
public:
    virtual void funcB() { std::cout<<this<<std::endl;}

    int b = 0;
};

class C1 : public A1, public B1
{
public:
    void funcA() override {}
    int c = 0;
};

int main()
{
    {
        C c;
        C* pC = &c;
        B* pB = &c;
        A* pA = &c;
        std::cout << "pA = " << pA <<std::endl;
        std::cout << "pB = " << pB <<std::endl;
        std::cout << "pC = " << pC <<std::endl;
    }
    {
        D d;
        D* pD = &d;
        B* pB = &d;
        A* pA = &d;
        std::cout <<" pD = "<< pD << std::endl;
        std::cout <<" pB = "<< pB << std::endl;
        std::cout <<" pA = "<< pA << std::endl;
    }
    {
        C1 c1;
        C1* pC1 = &c1;
        B1* pB1 = &c1;
        A1* pA1 = &c1;
        std::cout <<" pC1 = "<< pC1 << std::endl;
        std::cout <<" pB1 = "<< pB1 << std::endl;
        std::cout <<" pA1 = "<< pA1 << std::endl;

        std::cout << " pC1 func this: ";
        pC1->funcB();
        std::cout << " pB1 func this: ";
        pB1->funcB();
    }
    return 0;
}