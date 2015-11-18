/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  WindowManager class implementation.
 */

#include "../Window.hpp"

namespace
{
const DWORD gWindowedExStyle = WS_EX_WINDOWEDGE;
const DWORD gWindowedStyle = WS_OVERLAPPEDWINDOW;
const DWORD gFullscreenExStyle = 0;
const DWORD gFullscreenStyle = WS_POPUP | WS_SYSMENU;

} // namespace

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


WindowManager::WindowManager()
{
    mInstance = GetModuleHandle(0);
    mHandle = 0;
    mClosed = true;
    mWidth = 200;
    mHeight = 200;
    mLeft = 10;
    mTop = 10;
    mFullscreen = false;
    mResizeCallback = nullptr;
    mResizeCallbackUserData = nullptr;

    mTitle = "WindowManager";

    swprintf_s(mWndClass, L"%ws_%p", L"MineZPRaft_WndClass", this);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mInstance;
    wcex.hIcon = 0; //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME));
    wcex.hIconSm = 0; //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = 0; //(HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = mWndClass;
    RegisterClassEx(&wcex);

    for (int i = 0; i < 3; i++)
        mMouseButtons[i] = false;

    for (int i = 0; i < 255; i++)
        mKeys[i] = false;
}

WindowManager::~WindowManager()
{
    Close();
    UnregisterClass(mWndClass, mInstance);
}

void WindowManager::SetSize(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;

    if (!mClosed)
        MoveWindow(mHandle, mLeft, mTop, width, height, TRUE);
}

void WindowManager::SetTitle(const char* title)
{
    mTitle = title;
    if (!mClosed)
    {
        SetWindowText(mHandle, UTF8ToUTF16(mTitle).c_str());
    }
}

void WindowManager::SetFullscreenMode(bool enabled)
{
    if (mFullscreen && !enabled)
    {
        // escape fullscreen
        RECT WindowRect;
        WindowRect.left = mLeft;
        WindowRect.right = mLeft + mWidth;
        WindowRect.top = mTop;
        WindowRect.bottom = mTop + mHeight;
        AdjustWindowRectEx(&WindowRect, gWindowedStyle, FALSE, gWindowedExStyle);

        SetWindowLong(mHandle, GWL_EXSTYLE, gWindowedExStyle);
        SetWindowLong(mHandle, GWL_STYLE, gWindowedStyle);
        SetWindowPos(mHandle, HWND_NOTOPMOST,
                     mTop, mTop,
                     WindowRect.right - WindowRect.left,
                     WindowRect.bottom - WindowRect.top,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    else if (!mFullscreen && enabled)
    {
        // enter fullscreen
        SetWindowLong(mHandle, GWL_EXSTYLE, gFullscreenExStyle);
        SetWindowLong(mHandle, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(mHandle, nullptr, 0, 0, mWidth, mHeight, SWP_NOZORDER);
    }

    mFullscreen = enabled;

    if (mResizeCallback)
    {
        mResizeCallback(mResizeCallbackUserData);
    }
}


bool WindowManager::Open()
{
    if (!mClosed)
        return false;
    if (mFullscreen)
    {
        mHandle = CreateWindowEx(gFullscreenExStyle, mWndClass, UTF8ToUTF16(mTitle).c_str(),
                                 gFullscreenStyle, 0, 0, mWidth, mHeight,
                                 nullptr, nullptr, mInstance, nullptr);
    }
    else
    {
        RECT WindowRect;
        WindowRect.left = (long)mLeft;
        WindowRect.right = (long)(mWidth + mLeft);
        WindowRect.top = (long)mTop;
        WindowRect.bottom = (long)(mHeight + mTop);
        AdjustWindowRectEx(&WindowRect, gWindowedStyle, FALSE, gWindowedExStyle);

        mLeft = -WindowRect.left;
        mTop = -WindowRect.top;

        mHandle = CreateWindowEx(gWindowedExStyle,
                                 mWndClass, UTF8ToUTF16(mTitle).c_str(),
                                 gWindowedStyle,
                                 mTop, mTop,
                                 WindowRect.right - WindowRect.left,
                                 WindowRect.bottom - WindowRect.top,
                                 nullptr, nullptr, mInstance, nullptr);
    }

    if (!mHandle)
        return false;

    //TODO: Create and make OGL context current here
    SetWindowLongPtr(mHandle, GWLP_USERDATA, (LONG_PTR)this);
    SetWindowText(mHandle, UTF8ToUTF16(mTitle).c_str());
    ShowWindow(mHandle, SW_SHOW);
    UpdateWindow(mHandle);
    SetFocus(mHandle);

    mClosed = false;
    return true;
}

bool WindowManager::Close()
{
    if (mClosed)
        return false;

    ShowWindow(mHandle, SW_HIDE);
    mClosed = true;
    return true;
}

void WindowManager::MouseDown(uint32_t button, int x, int y)
{
    SetCapture(mHandle);
    mMouseButtons[button] = true;
    mMouseDownX[button] = x;
    mMouseDownY[button] = y;

    OnMouseDown(button, x, y);
}

void WindowManager::MouseUp(uint32_t button)
{
    mMouseButtons[button] = false;

    bool ButtonsReleased = true;
    for (int i = 0; i < 3; i++)
        if (mMouseButtons[i])
            ButtonsReleased = false;

    if (ButtonsReleased)
        ReleaseCapture();

    OnMouseUp(button);
}

void WindowManager::MouseMove(int x, int y)
{
    OnMouseMove(x, y, x - mMouseDownX[0], y - mMouseDownY[0]);
    mMouseDownX[0] = x;
    mMouseDownY[0] = y;
}

bool WindowManager::IsKeyPressed(int Key) const
{
    return mKeys[Key];
}

LRESULT CALLBACK WindowManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowManager* Window = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (!Window)
        return DefWindowProc(hWnd, message, wParam, lParam);

    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0;
            }
            break;
        }

        case WM_CLOSE:
        {
            Window->Close();
            Window->OnClose();
            return 0;
        }

        case WM_SIZE:
        {
            if (wParam != SIZE_MINIMIZED)
            {
                Window->mWidth = LOWORD(lParam);
                Window->mHeight = HIWORD(lParam);
                Window->OnResize(Window->mWidth, Window->mHeight);

                if (Window->mResizeCallback)
                    Window->mResizeCallback(Window->mResizeCallbackUserData);
            }

            return 0;
        }

        case WM_KEYDOWN:
        {
            Window->mKeys[wParam] = true;
            Window->OnKeyPress((int)wParam);
            return 0;
        }

        case WM_KEYUP:
        {
            Window->mKeys[wParam] = false;
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            Window->OnScroll(GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;
        }

        // MOUSE
        case WM_LBUTTONDOWN:
        {
            Window->MouseDown(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }

        case WM_LBUTTONUP:
        {
            Window->MouseUp(0);
            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            Window->MouseDown(2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }

        case WM_MBUTTONUP:
        {
            Window->MouseUp(2);
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            Window->MouseDown(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }

        case WM_RBUTTONUP:
        {
            Window->MouseUp(1);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            Window->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }

        case WM_ACTIVATE:
        {
            if (wParam == WA_INACTIVE)
                Window->LostFocus();
            return 0;
        }

    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void WindowManager::LostFocus()
{
    MouseUp(0);
    MouseUp(1);
    MouseUp(2);

    for (int i = 0; i < 256; i++)
        mKeys[i] = false;
}

bool WindowManager::IsClosed() const
{
    return mClosed;
}

bool WindowManager::HasFocus() const
{
    return GetActiveWindow() == mHandle;
}

void* WindowManager::GetHandle() const
{
    return static_cast<void*>(mHandle);
}

void WindowManager::GetSize(uint32_t& Width, uint32_t& Height) const
{
    Width = mWidth;
    Height = mHeight;
}

float WindowManager::GetAspectRatio() const
{
    return (float)mWidth / (float)mHeight;
}

bool WindowManager::GetFullscreenMode() const
{
    return mFullscreen;
}

bool WindowManager::IsMouseButtonDown(uint32_t button) const
{
    return mMouseButtons[button];
}

void WindowManager::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            Close();
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void WindowManager::SetResizeCallback(WindowResizeCallback func, void* userData)
{
    mResizeCallback = func;
    mResizeCallbackUserData = userData;
}

void WindowManager::OnClose()
{
}

void WindowManager::OnResize(uint32_t width, uint32_t height)
{
    (void)width;
    (void)height;
}

void WindowManager::OnKeyPress(int key)
{
    (void)key;
}

void WindowManager::OnScroll(int delta)
{
    (void)delta;
}

void WindowManager::OnMouseDown(uint32_t button, int x, int y)
{
    (void)button;
    (void)x;
    (void)y;
}

void WindowManager::OnMouseMove(int x, int y, int deltaX, int deltaY)
{
    (void)x;
    (void)y;
    (void)deltaX;
    (void)deltaY;
}

void WindowManager::OnMouseUp(uint32_t button)
{
    (void)button;
}

