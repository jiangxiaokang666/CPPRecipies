#include "pch.h"

#include <new>

class Player
{
public:
    explicit Player(int hp): m_hp(hp)
    {
        std::cout<<"Construct\n";
    }
    ~Player()
    {
        std::cout<<"Destruct\n";
    }
private:
    int m_hp = 0;
};

int main()
{
    alignas(Player)
    unsigned char buffer[sizeof(Player)];
    Player* p  = new(buffer) Player(100);
    p->~Player();
    return 0;
};