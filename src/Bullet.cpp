#include <iaj/Bullet.h>

Bullet::Bullet()
{
    this->radius = 2;
    this->damage = 10;
}

void Bullet::Update(float deltaTime)
{
    this->position = Vector2Add(this->position, Vector2Scale(this->velocity, deltaTime));
}

void Bullet::Draw()
{
    DrawCircle(position.x, position.y, radius, RED);
}

void Bullet::Trajectory(Vector2 position, float rotation)
{
    this->position = position;
    this->rotation = rotation;
    float speed =350.0f;

    Vector2 forward = {0, -1};
    Vector2 direction = Vector2Rotate(forward, rotation * DEG2RAD);

    this->velocity = Vector2Scale(direction, speed);
}

int8_t Bullet::getDamage()
{
    return damage;
}

Vector2 Bullet::getPosition()
{
    return position;
}

float Bullet::getRadius()
{
    return radius;
}