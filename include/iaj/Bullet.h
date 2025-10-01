#pragma once

#include <cstdint>
#include <raylib.h>
#include <raymath.h>

class Bullet
{
    public:
        Vector2 position;

        Bullet();
        void Update(float deltaTime);
        void Draw();
        void Trajectory(Vector2 position, float rotation);

        int8_t getDamage();
        Vector2 getPosition();
        float getRadius();
    private:
        int8_t damage;

        float radius;
        float rotation;
        Vector2 velocity;
};