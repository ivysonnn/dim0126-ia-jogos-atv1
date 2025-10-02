#pragma once

#include <iaj/Character.h>

class Player;

enum class State
{
    IDLE,
    PURSUING,
};

class Enemy : public Character 
{
    public:
        State state = State::IDLE;

        using Character::Update;
        Enemy(Vector2 position, int8_t health, int8_t damage, float radius, double speed);
        ~Enemy();
        void Update(Player& player, float deltaTime);
        void Draw() override;
};
