#include "pch.h"
#include "object_pool.hpp"


struct Bullet
{
    Bullet(int id)
        : id(id)
    {
        std::cout
            << "Bullet "
            << id
            << " constructed\n";
    }

    ~Bullet()
    {
        std::cout
            << "Bullet "
            << id
            << " destroyed\n";
    }

    int id;
};

int main()
{
    ObjectPool<Bullet, 3> pool;

    Bullet* b1 = pool.Create(1);
    Bullet* b2 = pool.Create(2);
    std::cout<<"b2 = " << b2 << "\n";
    Bullet* b3 = pool.Create(3);

    Bullet* b4 = pool.Create(4);

    std::cout << "b4 = " << b4 << "\n";

    pool.Destroy(b2);

    Bullet* b5 = pool.Create(5);
    std::cout<<"b5 = " << b5 << "\n";

    pool.Destroy(b1);
    pool.Destroy(b3);
    pool.Destroy(b5);
    return 0;
}