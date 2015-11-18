#include "GameManager.hpp"

int main()
{
    GameManager& game = GameManager::GetInstance();
    game.GameLoop();

    return 0;
}