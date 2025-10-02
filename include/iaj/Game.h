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
        void Run();

    private:
        std::unique_ptr<Player> player;

        std::vector<std::unique_ptr<Bullet>> projectiles;
        std::vector<std::unique_ptr<Bullet>> ammo; 

        std::unique_ptr<World> world;

        std::vector<std::unique_ptr<Enemy>> allEnemies;
        std::vector<Vector2> floorAmmo;
        std::vector<Vector2> floorHeal;

        double initialTime;

        double lastDamageTime;

        void GenerateEnemies();
        void GenerateUtilities();

        void HandleInput(float deltaTime);
        void UpdateEntities(float deltaTime);
        void HandleCollisions();
        void UpdateCamera();
        void UpdateGameState();

        void Start();
        void Update(float deltaTime);
        void Win();
        void Over();
        void Restart();

        void DrawUI();
        void Draw();
        void DrawMinimap();

        enum class GameState
        {
            START,
            WIN,
            GAMEOVER,
            ONGOING
        };
        GameState state;
};