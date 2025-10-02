#pragma once

#include <cmath> 
#include <math.h>
#include <vector>
#include <memory>

#include <iaj/Character.h>
#include <iaj/Bullet.h>

class Player : public Character
{
    public:
        std::vector<std::unique_ptr<Bullet>> ammo;
        double fireCooldown = 0;
        double damageCooldown = 0;

        Player(Vector2 position, int8_t health, int8_t damage, float radius, double speed, float rotation, int maxAmmo);
        void Update(float deltaX) override;
        void Draw() override;

        std::vector<Vector2> GetVecs();
        int8_t GetHealth();
        int getAmmoQuantity();

        float getRotation();

    private:
        float rotation;
        Vector2 v1, v2, v3;
};
