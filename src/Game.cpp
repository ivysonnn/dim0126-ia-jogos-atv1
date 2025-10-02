#include <iaj/Game.h>
#include <raymath.h> 

// game constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const double CAMERA_ZOOM = 2.0f;

const double PLAYER_SPEED = 200.0f;
const double ENEMY_SPEED = 140.0f;

const int MAX_TIME = 90;

const double FIRE_COOLDOWN = 0.15;
const double DAMAGE_COOLDOWN = 0.6;
const int ENEMY_MAX_QUANTITY = 4;
const int FLOOR_MAX_AMMO_QUANTITY = ENEMY_MAX_QUANTITY + 5;
const int FLOOR_MAX_HEAL_QUANTITY = ENEMY_MAX_QUANTITY;
const int PLAYER_MAX_HEALTH = 100;
const int ENEMY_HEALTH = 50;
const int FIRE_DAMAGE = 10;
const int HEAL_AMOUNT = 25;
const int AMMO_PICKUP_AMOUNT = 15;
const int MAX_AMMO = 255;

const int COLS = 3; 
const int ROWS = 3;
const int ROOM_WIDTH = SCREEN_WIDTH - (SCREEN_WIDTH / 3);
const int ROOM_HEIGHT = SCREEN_HEIGHT - (SCREEN_HEIGHT / 3);

const Vector2 MAP_CENTER = {(ROOM_WIDTH * COLS) / 2, (ROOM_HEIGHT * ROWS) / 2};

Game::Game() : lastDamageTime(0.0)
{
    initialTime = GetTime();
    state = GameState::START;
    this->player = std::make_unique<Player>(MAP_CENTER, PLAYER_MAX_HEALTH, FIRE_DAMAGE, 20, PLAYER_SPEED, 0, MAX_AMMO);

    world = std::make_unique<World>();
    world->Generate(COLS, ROWS, ROOM_WIDTH, ROOM_HEIGHT);

    GenerateEnemies();
    GenerateUtilities();
}

void Game::Run()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "IA JOGOS - ATV1");
    SetTargetFPS(120);

    camera = {0};
    camera.offset = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    camera.rotation = 0.0f;
    camera.zoom = CAMERA_ZOOM;
    camera.target = player->getPosition();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        if(state == GameState::START)
        {
            BeginDrawing();
                Start();
            EndDrawing();
        }
        else if(state == GameState::WIN)
        {
            BeginDrawing();
                Game::Win();
            EndDrawing();
        }
        else if(state == GameState::GAMEOVER)
        {
            BeginDrawing();
                Game::Over();
            EndDrawing();
        }
        else
        {
            Update(deltaTime);

            BeginDrawing();
                ClearBackground(BLACK);
                
                BeginMode2D(this->camera);
                    Draw();
                EndMode2D();

                DrawMinimap();
                DrawUI();
            EndDrawing();
        }
    }
    CloseWindow();
}

void Game::Update(float deltaTime)
{
    HandleInput(deltaTime);
    UpdateEntities(deltaTime);
    HandleCollisions();
    UpdateCamera();
    UpdateGameState();
}

void Game::Start()
{
    ClearBackground(BLACK);
    DrawText("SHOOT", SCREEN_WIDTH / 2 - MeasureText("SHOOT", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, RAYWHITE);
    DrawText("Press 'ENTER' to start", SCREEN_WIDTH / 2 - MeasureText("Press 'ENTER' to start", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, RAYWHITE);
    DrawText("Right and Left arrow keys to rotate the player", SCREEN_WIDTH / 2 - MeasureText("Right and Left arrow keys to rotate the player", 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, RAYWHITE);
    DrawText("WASD to move", SCREEN_WIDTH / 2 - MeasureText("WASD to move", 20) / 2, SCREEN_HEIGHT / 2 + 90, 20, RAYWHITE);
    DrawText("Left SHIFT to shoot", SCREEN_WIDTH / 2 - MeasureText("Left SHIFT to shoot", 20) / 2, SCREEN_HEIGHT / 2 + 120, 20, RAYWHITE);

    if(IsKeyPressed(KEY_ENTER))
    {
        state = GameState::ONGOING;
    }
}

void Game::Win()
{
    ClearBackground(BLACK);
    DrawText("YOU WIN", SCREEN_WIDTH / 2 - MeasureText("YOU WIN", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, GREEN);
    DrawText("Press 'R' to restart", SCREEN_WIDTH / 2 - MeasureText("Press 'R' to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, RAYWHITE);
    if (IsKeyPressed(KEY_R))
    {
        state = GameState::ONGOING;

        Restart();
    }
}

void Game::Over()
{
    ClearBackground(BLACK);
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, RED);
    DrawText("Press 'R' to restart", SCREEN_WIDTH / 2 - MeasureText("Press 'R' to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, RAYWHITE);

    if (IsKeyPressed(KEY_R))
    {
        // reset gamestate
        state = GameState::ONGOING;
        Restart();
    }
}

void Game::Restart()
{
    initialTime = GetTime();
        this->player = std::make_unique<Player>(MAP_CENTER, PLAYER_MAX_HEALTH, FIRE_DAMAGE, 20, PLAYER_SPEED, 0, MAX_AMMO);

        world = std::make_unique<World>();
        world->Generate(COLS, ROWS, ROOM_WIDTH, ROOM_HEIGHT);

        allEnemies.clear();
        projectiles.clear();
        floorAmmo.clear();
        floorHeal.clear();

        GenerateEnemies();
        GenerateUtilities();
}

void Game::HandleInput(float deltaTime)
{
    if (IsKeyDown(KEY_LEFT_SHIFT) && player->getAmmoQuantity() > 0)
    {
        if (GetTime() > player->fireCooldown + FIRE_COOLDOWN)
        {
            std::unique_ptr<Bullet> bullet = std::move(player->ammo.back());
            player->ammo.pop_back();

            bullet->Trajectory(player->GetVecs().at(0), player->getRotation());
            projectiles.emplace_back(std::move(bullet));
            
            player->fireCooldown = GetTime();
        }
    }
}

void Game::UpdateEntities(float deltaTime)
{
    player->Update(deltaTime);

    // keeps player inside the world
    Rectangle worldBounds = world->getWorldBounds();
    Vector2 playerPos = player->getPosition();
    playerPos.x = Clamp(playerPos.x, worldBounds.x + player->getRadius(), worldBounds.x + worldBounds.width - player->getRadius());
    playerPos.y = Clamp(playerPos.y, worldBounds.y + player->getRadius(), worldBounds.y + worldBounds.height - player->getRadius());
    player->setPosition(playerPos);

    // updates projectiles and erase them if they are out of the viewport
    for(auto& p : projectiles)
    {
        p->Update(deltaTime);
    }
    Rectangle screenBounds = { 
        camera.target.x - (SCREEN_WIDTH / camera.zoom / 2),
        camera.target.y - (SCREEN_HEIGHT / camera.zoom / 2),
        SCREEN_WIDTH / camera.zoom, 
        SCREEN_HEIGHT / camera.zoom, 
    };
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [&](const auto& p){
                return !CheckCollisionCircleRec(p->position, p->getRadius(), screenBounds);
            }),
        projectiles.end()
    );        void Run();

    // defines wich rooms are active
    for (const auto& room : world->getRooms())
    {
        room->isActive = CheckCollisionRecs(screenBounds, room->roomRec);
    }

    // updates enemies inside active rooms
    for (auto& e : allEnemies)
    {
        if (e->state == State::PURSUING)
        {
            e->Update(*player, deltaTime);

            // keep enemies inside the world
            Vector2 enemyPos = e->getPosition();
            enemyPos.x = Clamp(enemyPos.x, worldBounds.x + e->getRadius(), worldBounds.x + worldBounds.width - e->getRadius());
            enemyPos.y = Clamp(enemyPos.y, worldBounds.y + e->getRadius(), worldBounds.y + worldBounds.height - e->getRadius());
            e->setPosition(enemyPos);
        }
    }
}

void Game::HandleCollisions()
{
    Vector2 v1 = player->GetVecs().at(0);
    Vector2 v2 = player->GetVecs().at(1);
    Vector2 v3 = player->GetVecs().at(2);

    // list of active enemies
    std::vector<Enemy*> activeEnemies;
    for (auto& e : allEnemies)
    {
        for (const auto& room : world->getRooms())
        {
            if (room->isActive && CheckCollisionPointRec(e->getPosition(), room->roomRec))
            {
                activeEnemies.push_back(e.get());
                break;
            }
        }
    }

    // collision beetween enemies
    for (auto* e : activeEnemies)
    {
        // enemy vs player collision
        if (CheckCollisionPointTriangle(e->getPosition(), v1, v2, v3) ||
            CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v1, v2) ||
            CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v1, v3) ||
            CheckCollisionCircleLine(e->getPosition(), e->getRadius(), v3, v2))
        {
            if (GetTime() > lastDamageTime + DAMAGE_COOLDOWN)
            {
                player->health = std::max(player->health - e->getDamage(), 0);
                lastDamageTime = GetTime();
            }

            Vector2 playerPos = player->getPosition();
            Vector2 enemyPos = e->getPosition();
            float distance = Vector2Distance(playerPos, enemyPos);
            const float playerPhysicsRadius = 15.0f;
            float minDistance = playerPhysicsRadius + e->getRadius();
            
            if (distance < minDistance)
            {
                float overlap = minDistance - distance;
                Vector2 pushDirection = Vector2Normalize(Vector2Subtract(playerPos, enemyPos));
                Vector2 playerPush = Vector2Scale(pushDirection, overlap * 0.5f);
                Vector2 enemyPush = Vector2Scale(pushDirection, -overlap * 0.5f);
                player->setPosition(Vector2Add(playerPos, playerPush));
                e->setPosition(Vector2Add(enemyPos, enemyPush));
            }
        }

        // projectiles vs enemies
        for (auto& p : projectiles)
        {
            if (CheckCollisionCircles(e->getPosition(), e->getRadius(), p->position, p->getRadius()))
            {
                p->position.x = player->getPosition().x + SCREEN_WIDTH;
                e->health -= p->getDamage();
            }
        }
    }

    // active enemy vs enemy
    for (size_t i = 0; i < activeEnemies.size(); ++i)
    {
        for (size_t j = i + 1; j < activeEnemies.size(); ++j)
        {
            Enemy* e1 = activeEnemies[i];
            Enemy* e2 = activeEnemies[j];
            if (CheckCollisionCircles(e1->getPosition(), e1->getRadius(), e2->getPosition(), e2->getRadius()))
            {
                Vector2 e1_pos = e1->getPosition();
                Vector2 e2_pos = e2->getPosition();
                float distance = Vector2Distance(e1_pos, e2_pos);
                float minDistance = e1->getRadius() + e2->getRadius();
                float overlap = minDistance - distance;
                if (overlap > 0)
                {
                    Vector2 pushDirection = Vector2Normalize(Vector2Subtract(e2_pos, e1_pos));
                    Vector2 pushVector = Vector2Scale(pushDirection, overlap);
                    e2->setPosition(Vector2Add(e2_pos, pushVector));
                }
            }
        }
    }

    // removes enemies that are dead
    allEnemies.erase(
        std::remove_if(allEnemies.begin(), allEnemies.end(),
            [](const auto& e) { return e->getIsDead(); }),
        allEnemies.end()
    );

    // utilities are removed or moved 
    floorAmmo.erase(
        std::remove_if(floorAmmo.begin(), floorAmmo.end(),
            [&](const Vector2& ammoPos) {
                if (CheckCollisionPointTriangle(ammoPos, v1, v2, v3))
                {
                    for (int i = 0; i < AMMO_PICKUP_AMOUNT; ++i)
                    {
                        if (player->ammo.size() < 255) player->ammo.emplace_back(std::make_unique<Bullet>());
                    }
                    return true;
                }
                return false;
            }),
        floorAmmo.end()
    );
    floorHeal.erase(
        std::remove_if(floorHeal.begin(), floorHeal.end(),
            [&](const Vector2& healPos) {
                if (player->GetHealth() < PLAYER_MAX_HEALTH && CheckCollisionPointTriangle(healPos, v1, v2, v3))
                {
                    player->health = std::min(player->GetHealth() + HEAL_AMOUNT, PLAYER_MAX_HEALTH);
                    return true;
                }
                return false;
            }),
        floorHeal.end()
    );
}

void Game::UpdateCamera()
{
    camera.target = player->getPosition();
}

void Game::UpdateGameState()
{
    if(allEnemies.size() <= 0) state = GameState::WIN;
    if(player->health == 0 || GetTime() - initialTime >= MAX_TIME) state = GameState::GAMEOVER;
    for(auto& e: allEnemies)
    {
        bool isInActiveRoom = false;
        for(auto& room : world->getRooms()) // activate the enemy that are on the active room
        {
            if(room->isActive && CheckCollisionCircleRec(e->getPosition(), e->getRadius(), room->roomRec))
            {
                isInActiveRoom = true;
                break;
            }
        }

        if (isInActiveRoom)
        {
            e->state = State::PURSUING;
        }
        else
        {
            e->state = State::IDLE;
        }
    }
}

void Game::Draw()
{
    world->Draw();

    for(const auto& ammoPos : floorAmmo)
    {
        Vector2 size = {8, 12};
        Vector2 topLeft = {ammoPos.x - size.x / 2, ammoPos.y - size.y / 2};
        DrawRectangleV(topLeft, size, YELLOW);
    }
    for(const auto& heal : floorHeal)
    {
        DrawRectangle(heal.x - 5, heal.y + 5, 15, 5, GREEN);
        DrawRectangle(heal.x, heal.y, 5, 15, GREEN);
    }

    for (const auto& e : allEnemies)
    {
        for (const auto& room : world->getRooms())
        {
            if (room->isActive && CheckCollisionPointRec(e->getPosition(), room->roomRec))
            {
                e->Draw();
                break;
            }
        }
    }
    player->Draw();
    for(const auto& p : projectiles) p->Draw();
}

void Game::DrawUI()
{
    Color HPcolor = player->GetHealth() < 20 ? RED : player->GetHealth() < 50 ? YELLOW : GREEN;
    std::string hpText = "HP: " + std::to_string(player->GetHealth());
    std::string ammoText = std::to_string(player->getAmmoQuantity()) + "/" + std::to_string(MAX_AMMO);
    std::string timeText = std::to_string((int)GetTime() - (int)initialTime) + "s";
    std::string enemyLeftText = "ENEMIES LEFT: " + std::to_string((int)allEnemies.size());
    
    DrawText(hpText.c_str(), 20, SCREEN_HEIGHT - 40, 30, HPcolor);
    DrawText(ammoText.c_str(), SCREEN_WIDTH - 135, SCREEN_HEIGHT - 40, 30, RAYWHITE);
    DrawText(timeText.c_str(), 20, 40, 30, RAYWHITE);
    DrawText(enemyLeftText.c_str(), 20, (SCREEN_WIDTH / 2), 30, RAYWHITE);
}

void Game::DrawMinimap()
{
    int minimapWidth = 200;
    int minimapHeight = 200;
    int minimapX = SCREEN_WIDTH - minimapWidth - 20;
    int minimapY = 20;

    DrawRectangle(minimapX, minimapY, minimapWidth, minimapHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(minimapX, minimapY, minimapWidth, minimapHeight, WHITE);

    Rectangle worldBounds = world->getWorldBounds();
    float scaleX = (float)minimapWidth / worldBounds.width;
    float scaleY = (float)minimapHeight / worldBounds.height;

    Vector2 playerPos = player->getPosition();
    float playerMinimapX = minimapX + (playerPos.x - worldBounds.x) * scaleX;
    float playerMinimapY = minimapY + (playerPos.y - worldBounds.y) * scaleY;
    DrawCircle(playerMinimapX, playerMinimapY, 3, BLUE);

    for (const auto& enemy : allEnemies)
    {
        Vector2 enemyPos = enemy->getPosition();
        float enemyMinimapX = minimapX + (enemyPos.x - worldBounds.x) * scaleX;
        float enemyMinimapY = minimapY + (enemyPos.y - worldBounds.y) * scaleY;
        DrawCircle(enemyMinimapX, enemyMinimapY, 2, RED);
    }

    for (const auto& room : world->getRooms())
    {
        Color color = room->isActive ? RED : WHITE;
        Rectangle roomRec = room->roomRec;
        float roomMinimapX = minimapX + (roomRec.x - worldBounds.x) * scaleX;
        float roomMinimapY = minimapY + (roomRec.y - worldBounds.y) * scaleY;
        float roomMinimapWidth = roomRec.width * scaleX;
        float roomMinimapHeight = roomRec.height * scaleY;
        DrawRectangleLines(roomMinimapX, roomMinimapY, roomMinimapWidth, roomMinimapHeight, color);
    }
}

void Game::GenerateEnemies()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, ENEMY_MAX_QUANTITY);

    for(const auto& room : world->getRooms())
    {
        int numEnemies = distrib(gen);
        for (int i = 0; i < numEnemies; ++i)
        {
            std::uniform_int_distribution<> x_distrib(room->roomRec.x, room->roomRec.x + room->roomRec.width);
            std::uniform_int_distribution<> y_distrib(room->roomRec.y, room->roomRec.y + room->roomRec.height);

            Vector2 position = { (float)x_distrib(gen), (float)y_distrib(gen) };
            allEnemies.emplace_back(std::make_unique<Enemy>(position, ENEMY_HEALTH, 15, 25, ENEMY_SPEED));
        }
    }
}

void Game::GenerateUtilities()
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