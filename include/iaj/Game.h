#pragma once

#include <vector>
#include <math.h>
#include <memory>
#include <string>
#include <algorithm>
#include <random>

#include <iaj/Character.h>
#include <iaj/Player.h>
#include <iaj/Enemy.h>
#include <iaj/Bullet.h>
#include <iaj/World.h>

class Game 
{
    public: 
        Camera2D camera;

        Game();

        void GenerateEnemys();
        void GenerateUtilitys();

        void Run();

        void Update(float deltaTime);
        void Draw();

    private:
        std::unique_ptr<Player> player;
        std::vector<std::unique_ptr<Enemy>> enemys; 

        std::vector<std::unique_ptr<Bullet>> projectiles;
        std::vector<std::unique_ptr<Bullet>> ammo; 

        std::unique_ptr<World> world;

        std::vector<Vector2> floorAmmo;
        std::vector<Vector2> floorHeal; 
};