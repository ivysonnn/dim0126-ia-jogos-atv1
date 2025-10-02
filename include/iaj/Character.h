#pragma once

#include <cstdint>
#include <math.h>

#include <raylib.h>


class Character
{
    public: 
        int8_t health;

        virtual ~Character() = default;
        virtual void Update(float deltaTime) {};
        virtual void Draw() = 0;
        
        int8_t getDamage() const {return damage;}
        float getRadius() const {return radius;}
        
        Vector2 getPosition() const {return position;}
        void setPosition(Vector2 newPosition) { this->position = newPosition; }

        bool getIsDead() const {return health <= 0;}

        bool collisionDetected = false;

    protected:
        int8_t damage;
        float speed;
        float radius;

        Vector2 position;

        bool isDead;
        bool isColiding;
};