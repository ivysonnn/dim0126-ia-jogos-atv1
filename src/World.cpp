#include <iaj/World.h>

World::World() {}

void World::Generate(int cols, int rows, int roomWidth, int roomHeight) {
    worldBounds = {0, 0, (float)cols * roomWidth, (float)rows * roomHeight};
    m_rooms.clear();  // to restart the game
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            std::unique_ptr<Room> room = std::make_unique<Room>();
            float x = col * (roomWidth);
            float y = row * (roomHeight);

            room->roomRec = {x, y, (float)roomWidth, (float)roomHeight};
            m_rooms.push_back(std::move(room)); 
        }
    }
}

void World::Draw() {
    for (const auto& room : m_rooms) 
    {
        DrawRectangleRec(room->roomRec, BLANK); 
        DrawRectangleLinesEx(room->roomRec, 2, DARKGRAY);
    }
}

const std::vector<std::unique_ptr<Room>>& World::getRooms() const {
    return m_rooms;
}
