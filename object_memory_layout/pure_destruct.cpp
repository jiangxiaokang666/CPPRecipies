#include "pch.h"

class Base
{
public:
    virtual ~Base() = 0;
};

Base::~Base() = default;//compile error if not exist

class Derived : public Base
{

};



int main()
{
    Derived d;

    return 0;
}