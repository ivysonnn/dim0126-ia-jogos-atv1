#pragma once

#include <iaj/Character.h>

enum class State
{
    IDLE,
    PURSUING,
};

class Enemy : public Character 
{
    public:
        Enemy(Vector2 position, int8_t health, int8_t damage, float radius);
        ~Enemy();
        void Update(float deltaTime) override;
        void Draw() override;

        State getState();

    private:
        State state = State::IDLE;
};
