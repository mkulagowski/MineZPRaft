/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  WindowManager class declaration.
*/

#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>

#if defined(__LINUX__) | defined(__linux__)
#include <X11/Xlib.h>
#elif defined(WIN32)
#define NOMINMAX
#define UNICODE
#include <Windows.h>
#include <Windowsx.h>

// TODO: Move this to some helper functions file.
static std::string UTF16ToUTF8(const std::wstring &s)
{
    const int size = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);

    std::vector<char> buf(size);
    int charsConverted = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
    if (charsConverted == 0)
        return std::string();

    return std::string(buf.data());
}

static std::wstring UTF8ToUTF16(const std::string &s)
{
    size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), NULL, 0);
    if (charsNeeded == 0)
        return std::wstring();

    std::vector<wchar_t> buffer(charsNeeded);
    int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &buffer[0], buffer.size());
    if (charsConverted == 0)
        return std::wstring();

    return std::wstring(&buffer[0], charsConverted);
}
#endif // defined(__LINUX__) | defined(__linux__)
typedef void(*WindowResizeCallback)(void*);

/**
 * Simple UI windows class.
 */
class WindowManager
{
private:
#if defined(WIN32)
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND mHandle;
    HINSTANCE mInstance;
    int mLeft;
    int mTop;
    wchar_t mWndClass[48];
#elif defined(__LINUX__) | defined(__linux__)
    static ::Display* mDisplay;
    ::Window mWindow;
    ::Window mRoot;
    static bool mWindowError;
    static int ErrorHandler(::Display* dpy, XErrorEvent *error);
#else //...
#error "Target not supported!"
#endif // defined(WIN32)

    bool mClosed;
    bool mFullscreen;
    uint32_t mWidth;
    uint32_t mHeight;
    std::string mTitle;

    bool mMouseButtons[3];
    int mMouseDownX[3];
    int mMouseDownY[3];

    bool mKeys[256];

    // used by renderer
    WindowResizeCallback mResizeCallback;
    void* mResizeCallbackUserData;

    void LostFocus();
    void MouseDown(uint32_t button, int x, int y);
    void MouseUp(uint32_t button);
    void MouseMove(int x, int y);

    WindowManager(const WindowManager&);
    WindowManager& operator= (const WindowManager&) = delete;

public:
    WindowManager();
    ~WindowManager();

    bool Open();
    bool Close();

    void* GetHandle() const;
    void GetSize(uint32_t& width, uint32_t& height) const;
    float GetAspectRatio() const;
    bool GetFullscreenMode() const;

    bool IsMouseButtonDown(uint32_t button) const;
    bool IsKeyPressed(int key) const;

    void SetSize(uint32_t width, uint32_t height);
    void SetFullscreenMode(bool enabled);
    void SetTitle(const char* title);

    // WARNING: only game manager should call this function
    void SetResizeCallback(WindowResizeCallback func, void* userData);

    void ProcessMessages();
    bool IsClosed() const;
    bool HasFocus() const;

    // callbacks
    virtual void OnClose();
    virtual void OnResize(uint32_t width, uint32_t height);
    virtual void OnKeyPress(int key);
    virtual void OnScroll(int delta);
    virtual void OnMouseDown(uint32_t button, int x, int y);
    virtual void OnMouseMove(int x, int y, int deltaX, int deltaY);
    virtual void OnMouseUp(uint32_t button);
};
