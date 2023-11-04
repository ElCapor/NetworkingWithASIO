#pragma once
#include <cstdint>
#include <raylib.h>

struct player
{
    uint32_t id{};
    uint32_t avatar_id{};

    uint32_t health = 100;
    uint32_t ammo = 20;
    uint32_t kills = 0;
    uint32_t deaths = 0;

    Vector2 velocity{};
    Vector2 position{};
};
