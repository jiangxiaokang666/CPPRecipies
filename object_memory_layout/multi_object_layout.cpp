#include "pch.h"

class A
{
    int a = 0;
};

class B
{
    int b = 0;
};

class C : public A, public B
{
    int c = 0;
};

class A0
{
    int a = 0;
    virtual void funcA() {}
};

class B0
{
    int b = 0;
    virtual void funcB() {}
};

class C0 : public A0, public B0
{
    int c = 0;
};

class A1
{
    int a = 0;
};

class B1 : public A1
{
    int b = 0;
};

class C1 : public A1
{
    int c = 0;
};

class D1 : public B1, public C1
{
    int d = 0;
};

class A2
{
    int a = 0;
};

class B2 : virtual public A2
{
    int b = 0;
};

class C2 : public A2
{
    int c = 0;
};

int main()
{
    // x64 platform
    std::cout << "size of C " << sizeof(C) << std::endl;
    /*
        A::a  -> 4 -> align 8
        A::vptr -> 8
        B::b -> 4 -> align 8
        B::vptr -> 8
        C::c -> 4 -> align 8
     */
    std::cout << "size of C0 " << sizeof(C0) << std::endl;
    /*
        A1::a -> 4
        B1::b -> 4
        A1::a -> 4
        C1::c -> 4
        D1::d -> 4
    */
    std::cout << "size of D1 " << sizeof(D1) << std::endl;
    /*
        B2::vptr -> 8
        B2::b    -> 4 align -> 8
        A2::a    -> 4 align -> 8
    */
    std::cout << "size of B2 " << sizeof(B2) << std::endl;
    /*
        A2::a   -> 4
        C2::c   -> 4
    */
    std::cout << "size of C2 " << sizeof(C2) << std::endl;

    return 0;
}