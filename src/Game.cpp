#include <iaj/Game.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const double FIRE_COOLDOWN = .1;
const double DAMAGE_COOLDOWN = .4;
const int ENEMY_MAX_QUANTITY = 4;
const int FLOOR_MAX_AMMO_QUANTITY = ENEMY_MAX_QUANTITY + 5;
const int FLOOR_MAX_HEAL_QUANTITY = ENEMY_MAX_QUANTITY;
const int PLAYER_MAX_HEALTH = 100;
const int ENEMY_HEALTH = 50;
const int FIRE_DAMAGE = 10;
const int HEAL = 25;
const int COLS = 3; 
const int ROWS = 3;
const int ROOM_WIDTH = SCREEN_WIDTH / 2;
const int ROOM_HEIGHT = SCREEN_HEIGHT / 2;

const Vector2 center = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

Game::Game()
{
    this->player = std::make_unique<Player>(center, PLAYER_MAX_HEALTH, FIRE_DAMAGE, 20, 0);

    world = std::make_unique<World>();
    world->Generate(COLS, ROWS, ROOM_WIDTH, ROOM_HEIGHT);

    GenerateEnemys();
    GenerateUtilitys();
}

void Game::GenerateEnemys()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, ENEMY_MAX_QUANTITY);

    for(auto& room : world->getRooms())
    {
        int numEnemies = distrib(gen);
        for (int i = 0; i < numEnemies; ++i)
        {
            std::uniform_int_distribution<> x_distrib(room->roomRec.x, room->roomRec.x + room->roomRec.width);
            std::uniform_int_distribution<> y_distrib(room->roomRec.y, room->roomRec.y + room->roomRec.height);

            Vector2 position = { (float)x_distrib(gen), (float)y_distrib(gen) };
            enemys.emplace_back(std::make_unique<Enemy>(position, ENEMY_HEALTH, 15, 25));
        }
    }
}

void Game::GenerateUtilitys()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> room_distrib(0, world->getRooms().size() - 1);

    for (int i = 0; i < FLOOR_MAX_AMMO_QUANTITY; ++i)
    {
        int room_index = room_distrib(gen);
        auto& room = world->getRooms().at(room_index);
        std::uniform_int_distribution<> x_distrib(room->roomRec.x, room->roomRec.x + room->roomRec.width);
        std::uniform_int_distribution<> y_distrib(room->roomRec.y, room->roomRec.y + room->roomRec.height);
        floorAmmo.push_back({(float)x_distrib(gen), (float)y_distrib(gen)});
    }

    for (int i = 0; i < FLOOR_MAX_HEAL_QUANTITY; ++i)
    {
        int room_index = room_distrib(gen);
        auto& room = world->getRooms().at(room_index);
        std::uniform_int_distribution<> x_distrib(room->roomRec.x, room->roomRec.x + room->roomRec.width);
        std::uniform_int_distribution<> y_distrib(room->roomRec.y, room->roomRec.y + room->roomRec.height);
        floorHeal.push_back({(float)x_distrib(gen), (float)y_distrib(gen)});
    }
}

void Game::Run()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "IA JOGOS - ATV1");

    SetTargetFPS(120);

    Camera2D camera = {0};
    camera.offset = center;
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    this->camera = camera;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        Update(deltaTime);
       

        this->camera.target = player->getPosition();

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode2D(this->camera);

                Draw();

            EndMode2D();

            std::string hpText = "HP: " + std::to_string(player->GetHealth());
            std::string ammoText = std::to_string(player->getAmmoQuantity()) + "/255";
            DrawText(hpText.c_str(), 20, SCREEN_HEIGHT - 40, 30, RAYWHITE);
            DrawText(ammoText.c_str(), SCREEN_WIDTH - 125, SCREEN_HEIGHT - 40, 30, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
}

void Game::Update(float deltaTime)
{
    Vector2 v1 = player->GetVecs().at(0);
    Vector2 v2 = player->GetVecs().at(1);
    Vector2 v3 = player->GetVecs().at(2);

    Vector2 tl = {player->getPosition().x - (SCREEN_WIDTH / 2), player->getPosition().y - (SCREEN_HEIGHT / 2)};

    Rectangle screen = {tl.x, tl.y, SCREEN_WIDTH, SCREEN_HEIGHT}; //screen rectangle


    double lastHit = GetTime();
    for(auto& e : enemys)
    {
        //checks if any enemy is colliding with the player
        if(CheckCollisionPointTriangle(e->getPosition(), v1, v2, v3) ||
        CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v1, v2) ||
        CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v1, v3) ||
        CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v3, v2))
        {
            if(GetTime() > lastHit + DAMAGE_COOLDOWN)
                player->health -= e->getDamage();
        }

        for(auto& p : projectiles)
        {
            p->Update(deltaTime);
            if(CheckCollisionCircles(e->getPosition(), e->getRadius(), p->position, p->getRadius()))
            {
                p->position.x = camera.offset.x + SCREEN_WIDTH;
                e->health -= p->getDamage();
            }
        }

        
    }

    enemys.erase(
        std::remove_if(enemys.begin(), enemys.end(),
            [&](const auto& e){
                return e->getIsDead();
            }),
        enemys.end()
    );
    
    if(IsKeyDown(KEY_LEFT_SHIFT))
    {
        if(GetTime() > player->cooldown + FIRE_COOLDOWN && player->getAmmoQuantity() > 0)
        {
            std::unique_ptr<Bullet> bullet = std::move(player->ammo.back());
            player->ammo.pop_back();

            bullet->Trajectory(player->GetVecs().at(0), player->getRotation());

            projectiles.emplace_back(std::move(bullet));
            player->cooldown = GetTime();
        }

    }

    for(auto& p : projectiles)
    {
        p->Update(deltaTime);
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [&](const auto& p){
                return !CheckCollisionCircleRec(p->position, p->getRadius(), screen);
            }),
        projectiles.end()
    );

    player->Update(deltaTime);

    for(auto& ammo : floorAmmo)
    {
        if(CheckCollisionPointTriangle(ammo, v1, v2, v3))
        {
            for(int i = 0; i < 15; ++i)
            {
                if(player->ammo.size() < 255) player->ammo.emplace_back(std::make_unique<Bullet>());
            }
        }
    }

    floorAmmo.erase(
        std::remove_if(floorAmmo.begin(), floorAmmo.end(),
            [&](const auto& a){
                return CheckCollisionPointTriangle(a, v1, v2, v3);
        }),
        floorAmmo.end()
    );

    for(auto& heal : floorHeal)
    {
        if(CheckCollisionPointTriangle(heal, v1, v2, v3) && player->GetHealth() < 100)
        {
            player->health = std::min(player->GetHealth() + HEAL, PLAYER_MAX_HEALTH);
        }
    }

    floorHeal.erase(
        std::remove_if(floorHeal.begin(), floorHeal.end(),
            [&](const auto& h){
                return CheckCollisionPointTriangle(h, v1, v2, v3) && player->GetHealth() < 100; 
        }),
        floorHeal.end()
    );
}

void Game::Draw()
{
    world->Draw();
    for(const auto& e : enemys)
    {
        e->Draw();
    }
    for(auto& p : projectiles)
    {
        p->Draw();
    }

    for(const auto& ammo : floorAmmo)
    {
        DrawCircleV(ammo, 5, YELLOW);
    }

    for(const auto& heal : floorHeal)
    {
        DrawCircleV(heal, 7, GREEN);
    }

    player->Draw();
}
