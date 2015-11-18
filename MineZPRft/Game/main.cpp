#include "GameWindow.hpp"

int main()
{
    GameWindow window;
    window.SetSize(800, 600);
    window.SetTitle("MineZPRaft Game Window");
    window.Open();
    while (!window.IsClosed())
    {
        window.ProcessMessages();
    }
    window.Close();
    return 0;
}