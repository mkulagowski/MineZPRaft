/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  WindowManager Demo implementation.
 */

#include "Window.hpp"
#include "main.hpp"

#if defined(__LINUX__) | defined(__linux__)
#define RMB 2
#elif defined(WIN32)
#define RMB 1
#endif

class DemoWindow;

DemoWindow::DemoWindow()
    : WindowManager(),
    titleCounter(0)
{
}

void DemoWindow::OnMouseUp(uint32_t button)
{
    if (button == RMB)
        titleCounter++;
    else if (button == 0)
        if (titleCounter > 0)
            titleCounter--;

    SetTitle((title + std::to_string(titleCounter)).c_str());
}

void DemoWindow::OnKeyPress(int key)
{
    std::string keyPressed = " - pressed key: ";
    keyPressed += std::to_string(key);
    SetTitle((title + std::to_string(titleCounter) + keyPressed).c_str());
}

int main()
{
    DemoWindow window;
    window.SetSize(500, 500);
    window.Open();
    while (!window.IsClosed())
    {
        window.ProcessMessages();
    }
    window.Close();
    return 0;
}