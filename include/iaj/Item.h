#pragma once

#include <string>
#include <cstdint>

enum class IType
{
    HEAL,
    DAMAGE,
};

class Item 
{
    public:
        Item(std::string name, IType t, int32_t effectValue, int32_t x, int32_t y) : name(name), t(t), effectValue(effectValue), posX(x), posY(y) {}

        std::string getName() const {return name;}
        IType getT() const {return t;}

        int32_t getPosX() const {return posX;}
        int32_t getPosY() const {return posY;}

        void setPosX(int32_t x) { posX = x; }
        void setPosY(int32_t y) { posY = y; }
    private:
        std::string name;
        IType t;
        int32_t effectValue;
    
        int32_t posX;
        int32_t posY;
};