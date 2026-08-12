#include "pch.h"

class Animal
{
public:
    virtual void Speak()
    {
        std::cout << "animal" << std::endl;
    }
};

class Dog : public Animal
{
public:
    void Speak() override
    {
        std::cout << "dog" << std::endl;
    }
};


int main()
{
    Dog* p = new Dog();
    p->Speak();
    delete p;
    return 0;
}