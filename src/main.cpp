#include <string>

#include <raylib.h>
#include <iaj/Game.h>

int main(void)
{
    Game *game = new Game();
    game->Run();
    delete game;
    return 0;
}
