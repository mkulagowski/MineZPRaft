/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  WindowManager Demo declaration.
 */

#pragma once

const std::string title = "@WindowTitle";

class DemoWindow : public WindowManager
{
public:
    DemoWindow();
private:
    int titleCounter;
    void OnMouseUp(uint32_t button);
    void OnKeyPress(int key);
};

