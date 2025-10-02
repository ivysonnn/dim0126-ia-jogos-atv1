#include <iaj/Enemy.h>
#include <iaj/Player.h>
#include <raymath.h>

Enemy::Enemy(Vector2 position, int8_t health, int8_t damage, float radius, double speed)
{
    this->position = position;
    this->health = health;
    this->damage = damage;
    this->radius = radius;
    this->speed = speed;
}

Enemy::~Enemy()
{
}
void Enemy::Update(Player& player, float deltaTime)
{
    state = State::PURSUING;
    Vector2 direction = Vector2Subtract(player.getPosition(), position);
    direction = Vector2Normalize(direction);
    position = Vector2Add(position, Vector2Scale(direction, speed * deltaTime));
}

void Enemy::Draw()
{
    DrawCircle(position.x, position.y, radius, BLACK);

    int segments = 36; 
    for (int i = 0; i < segments; i++) { // to make the lines thicker
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
}