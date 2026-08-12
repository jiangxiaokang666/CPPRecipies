#pragma once

#include "pch.h"

class Player
{
public:
    Player()
    {
        std::cout << "Construct" << std::endl;
    }
    ~Player()
    {
        std::cout << "Destruct" << std::endl;
    }

    int hp = 0;
    float speed = .0f;
};