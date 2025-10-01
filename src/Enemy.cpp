#include <iaj/Enemy.h>

Enemy::Enemy(Vector2 position, int8_t health, int8_t damage, float radius)
{
    this->position = position;
    this->health = health;
    this->damage = damage;
    this->radius = radius;
}

Enemy::~Enemy()
{
}
void Enemy::Update(float deltaTime)
{
    // For now, the enemy doesn't move on its own.
    // AI logic will be added later.
}

void Enemy::Draw()
{
    int segments = 36;
    for (int i = 0; i < segments; i++) {
        float angle1 = (2 * PI * i) / segments;
        float angle2 = (2 * PI * (i + 1)) / segments;
        
        Vector2 point1 = {
            position.x + cosf(angle1) * radius,
            position.y + sinf(angle1) * radius
        };
        
        Vector2 point2 = {
            position.x + cosf(angle2) * radius,
            position.y + sinf(angle2) * radius
        };
        
        DrawLineEx(point1, point2, 2, WHITE);
    }

    DrawCircle(position.x, position.y, radius, BLACK);
}

State Enemy::getState()
{
    return state;
}