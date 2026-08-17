#include "pch.h"

void PrintVPtr(void* obj)
{
    void* vptr = *reinterpret_cast<void**>(obj);
    std::cout << "vptr = " << vptr << '\n';
}

class Base
{
public:
    Base()
    {
        PrintVPtr(this);
        Func();
    }
    ~Base()
    {
        Func();
    }

    virtual void Func()
    {
        std::cout<<"Base::Func\n";
    }
};

class Derived : public Base
{
public:
    Derived()
    {
        PrintVPtr(this);
        Func();
    }
    ~Derived()
    {
        Func();
    }

    void Func() override
    {
        std::cout<<"Derived::Func\n";
    } 
};

int main()
{
    Derived d;
    return 0;
}