#include <iaj/Player.h>
#include <raymath.h>
#include <thread>

#define DEFAULT_ROTATION 1.5

Player::Player(Vector2 position, int8_t health, int8_t damage, float radius, float rotation)
{
    this->rotation = rotation;

    this->position = position;
    this->health = health;
    this->damage = damage;
    this->speed = 200.0f; 
    this->radius = radius;

    for(int i = 0; i < 45; i++)
    {
        ammo.emplace_back(std::move(std::make_unique<Bullet>()));
    }
}
void Player::Update(float deltaTime)
{
    Vector2 movement = {0.0f, 0.0f};

    if (IsKeyDown(KEY_D)) movement.x += 1.0f;
    if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_S)) movement.y += 1.0f;
    if (IsKeyDown(KEY_W)) movement.y -= 1.0f;

    if (IsKeyDown(KEY_RIGHT)) rotation += DEFAULT_ROTATION; 
    if (IsKeyDown(KEY_LEFT)) rotation -= DEFAULT_ROTATION;

    //normalized vector to prevent diagonal movement extra speed
    if (movement.x != 0.0f && movement.y != 0.0f)
    {
        float magnitude = sqrtf(movement.x * movement.x + movement.y * movement.y);
        movement.x /= magnitude;
        movement.y /= magnitude;
    }

    position.x += movement.x * speed * deltaTime;
    position.y += movement.y * speed * deltaTime;
}

void Player::Draw() // draws a triangle
{
    float h = radius * 1.5f; 
    float w = radius;      
    Vector2 p1 = {0, -h / 2.0f}; // top point
    Vector2 p2 = {-w / 2.0f, h / 2.0f}; 
    Vector2 p3 = {w / 2.0f, h / 2.0f}; 

    float angle_rad = rotation * DEG2RAD;
    v1 = Vector2Add(Vector2Rotate(p1, angle_rad), position);
    v2 = Vector2Add(Vector2Rotate(p2, angle_rad), position);
    v3 = Vector2Add(Vector2Rotate(p3, angle_rad), position);

    DrawLineEx(v1, v2, 2, WHITE);
    DrawLineEx(v1, v3, 2, WHITE);
    DrawLineEx(v3, v2, 2, WHITE);
    DrawTriangle(v1, v2, v3, BLACK);
}

std::vector<Vector2> Player::GetVecs()
{
    return std::vector<Vector2>({v1, v2, v3});
}

int8_t Player::GetHealth()
{
    return health;
}

u_int8_t Player::getAmmoQuantity()
{
    return ammo.size();
}

float Player::getRotation()
{
    return rotation;
}
