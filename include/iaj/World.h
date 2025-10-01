#pragma once

#include <vector>
#include <memory>
#include <raylib.h>

#include <iaj/Enemy.h>

typedef struct Room
{
    Rectangle roomRec;
    std::vector<std::unique_ptr<Enemy>> enemys;
    bool isActive = false;
} room;


class World
{
public:
    World();

    void Generate(int cols, int rows, int roomWidth, int roomHeight);
    
    void Draw();

    const std::vector<std::unique_ptr<Room>>& getRooms() const;

private:
    Rectangle worldBounds;
    std::vector<std::unique_ptr<Room>> m_rooms;
};
