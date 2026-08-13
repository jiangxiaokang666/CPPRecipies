#include "pch.h"

struct Test
{
    char a = '0';
    int b = 0;
    char c = '0';
};

struct EmptyTest
{

};

struct VirtTest
{
    virtual void foo() {}
};

int main()
{
    std::cout << "Size of Test " << sizeof(Test) << std::endl;
    std::cout << "Size of EmptyTest "<< sizeof(EmptyTest) << std::endl;
    std::cout << "Size of VirtTest "<< sizeof(VirtTest) << std::endl;
    return 0;
}