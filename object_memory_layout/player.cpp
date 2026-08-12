#include "player.h"


void Test()
{
    Player p;
}

void Test1()
{
    //placement new
    char buffer[sizeof(Player)];
    Player* p = new(buffer) Player();
    p->~Player();
}

int main()
{
    std::cout<< sizeof(Player) << std::endl;//8

    Test();

    Test1();

    return 0;
}