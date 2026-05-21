#ifndef ENTITY_H
#define ENTITY_H

#include "map.h"

enum class EnemyType
{
    Chaser,
    Stalker,
    Shadow,
    Boss
};

struct Item
{
    Position position;
    char symbol;
    bool active;
    const char* name;
    bool special;
};

struct Entity
{
    Position position;
    char symbol;
    int hp;
    EnemyType type;
    bool active;
    bool isBoss;
    bool isDouble;
};

struct Player
{
    Position position;
    int hp;
    bool hasSword;
    bool hasTreasure;
    bool hasItem;
    char carriedItem;
};

#endif // ENTITY_H
