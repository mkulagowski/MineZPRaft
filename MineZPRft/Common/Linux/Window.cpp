/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  WindowManager class Linux implementation.
*/

#include "../Window.hpp"
#include <GL/glx.h>

::Display* WindowManager::mDisplay;
bool WindowManager::mWindowError;

WindowManager::WindowManager()
{
    mWindow = 0;
    mClosed = true;
    mWidth = 500;
    mHeight = 500;
    mFullscreen = false;
    mResizeCallback = nullptr;
    mResizeCallbackUserData = nullptr;
    mTitle = "Window";

    if (!mDisplay)
    {
        mDisplay = XOpenDisplay(nullptr);
        if (mDisplay == nullptr)
        {
            printf("Cannot connect to X server\n");
        }
    }
    mRoot = DefaultRootWindow(mDisplay);
    XSetScreenSaver(mDisplay, 0, 0, DontPreferBlanking, AllowExposures);

    for (int i = 0; i < 3; i++)
        mMouseButtons[i] = false;

    for (int i = 0; i < 255; i++)
        mKeys[i] = false;
}

WindowManager::~WindowManager()
{
    Close();
    XSetScreenSaver(mDisplay, -1, 0, DontPreferBlanking, AllowExposures);
    XDestroyWindow(mDisplay, mWindow);
    XCloseDisplay(mDisplay);
}

void WindowManager::SetSize(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;

    if (!mClosed)
        XResizeWindow(mDisplay, mWindow, mWidth, mHeight);
}

void WindowManager::SetTitle(const char* title)
{
    mTitle = title;
    if (!mClosed)
        XStoreName(mDisplay, mWindow, mTitle.c_str());
}

void WindowManager::SetFullscreenMode(bool enabled)
{
    if (!mClosed)
    {
        if ((mFullscreen && !enabled) || (!mFullscreen && enabled))
        {
            XEvent event;
            ::Atom wmStateAtom = XInternAtom(mDisplay, "_NET_WM_STATE", False);
            ::Atom fullscreenAtom = XInternAtom(mDisplay, "_NET_WM_STATE_FULLSCREEN", False);

            memset(&event, 0, sizeof(event));
            event.type = ClientMessage;
            event.xclient.window = mWindow;
            event.xclient.message_type = wmStateAtom;
            event.xclient.format = 32;
            event.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
            event.xclient.data.l[1] = fullscreenAtom;
            event.xclient.data.l[2] = 0;

            if (enabled)
                event.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD

            XSendEvent(mDisplay, mRoot, False, SubstructureNotifyMask, &event);
        }

        mFullscreen = enabled;

        if (mResizeCallback)
        {
            mResizeCallback(mResizeCallbackUserData);
        }
    }
}

bool WindowManager::Open()
{
    if (!mClosed)
        return false;

    ::XSetWindowAttributes xSetWAttrib;

    // Visual should be chosen with proper FB Config
    static int fbAttribs[] =
    {
        GLX_X_RENDERABLE,  True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        GLX_STENCIL_SIZE,  8,
        GLX_DOUBLEBUFFER,  True,
        None
    };

    int fbCount;
    GLXFBConfig* fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), fbAttribs, &fbCount);

    printf("Found %d matching FB configs:\n", fbCount);

    // Select the best FB Config according to highest GLX_SAMPLES attribute value
    int bestFBID = -1, maxSamples = 16;
    for (int i = 0; i < fbCount; ++i)
    {
        XVisualInfo* vi = glXGetVisualFromFBConfig(mDisplay, fbc[i]);
        if (vi)
        {
            int sampleBuffers;
            int samples;
            glXGetFBConfigAttrib(mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
            glXGetFBConfigAttrib(mDisplay, fbc[i], GLX_SAMPLES, &samples);
            printf("  #%d: visualID 0x%2lu, SAMPLE_BUFFERS = %d, SAMPLES = %d\n",
                     i, vi->visualid, sampleBuffers, samples);

            if (samples < maxSamples)
            {
                bestFBID = i;
                maxSamples = samples;
            }
        }
        XFree(vi);
    }

    printf("Choosing FB config #%d\n", bestFBID);
    GLXFBConfig bestFB = fbc[bestFBID];
    XFree(fbc);

    XVisualInfo* visual = glXGetVisualFromFBConfig(mDisplay, bestFB);
    ::Colormap colormap = XCreateColormap(mDisplay, mRoot, visual->visual, AllocNone);
    xSetWAttrib.colormap = colormap;
    xSetWAttrib.event_mask = Button1MotionMask | Button2MotionMask | ButtonPressMask |
                             ButtonReleaseMask | ExposureMask | FocusChangeMask | KeyPressMask |
                             KeyReleaseMask | PointerMotionMask | StructureNotifyMask;

    XSetErrorHandler(ErrorHandler);
    mWindow = XCreateWindow(mDisplay, mRoot, 0, 0, mWidth, mHeight, 1, visual->depth,
                            InputOutput, visual->visual, CWColormap | CWEventMask, &xSetWAttrib);
    if (WindowManager::mWindowError)
    {
        WindowManager::mWindowError = false;
        return false;
    }
    XSetErrorHandler(nullptr);

    //TODO: Create and make OGL context current here
    XStoreName(mDisplay, mWindow, mTitle.c_str());
    ::Atom WmDelete = XInternAtom(mDisplay, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(mDisplay, mWindow, &WmDelete, 1);
    XMapWindow(mDisplay, mWindow);
    mClosed = false;
    return true;
}

bool WindowManager::Close()
{
    if (mClosed)
        return false;

    XUnmapWindow(mDisplay, mWindow);
    mClosed = true;
    return true;
}

void WindowManager::MouseDown(uint32_t button, int x, int y)
{
    XGrabPointer(mDisplay, mWindow, True, 0, GrabModeAsync,
                GrabModeAsync, mWindow, None, CurrentTime);
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
        XUngrabPointer(mDisplay, CurrentTime);

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

void WindowManager::ProcessMessages()
{
    XFlush(mDisplay);
    while (XEventsQueued(mDisplay, QueuedAlready))
    {
        XEvent event;
        XNextEvent(mDisplay, &event);
        switch (event.type)
        {
            case ClientMessage:
            {
                if (static_cast<Atom>(event.xclient.data.l[0]) ==
                    XInternAtom(mDisplay, "WM_DELETE_WINDOW", false))
                {
                    this->Close();
                    this->OnClose();
                }
                break;
            }
            case KeyPress:
            {
                mKeys[event.xkey.keycode] = true;
                OnKeyPress(static_cast<int>(event.xkey.keycode));
                break;
            }
            case KeyRelease:
            {
                this->mKeys[event.xkey.keycode] = false;
                break;
            }
            case MotionNotify:
            {
                this->MouseMove(event.xmotion.x, event.xmotion.y);
                break;
            }
            case ButtonPress:
            {
                if (event.xbutton.button < 4) // 1-3 MBtns, 4-5 MWheel
                    //can be event.x_root,y_root - then it's relative to ROOT window instead
                    this->MouseDown(event.xbutton.button - 1, event.xbutton.x, event.xbutton.y);
                else if (event.xbutton.button == 4)
                    this->OnScroll(1); // btn==4 is UP,
                else
                    this->OnScroll(-1); // btn==5 is DOWN
                break;
            }
            case ButtonRelease:
            {
                this->MouseUp(event.xbutton.button - 1);
                break;
            }
            case FocusOut:
            {
                this->LostFocus();
                break;
            }
            case ConfigureNotify:
            {
                XConfigureEvent confEvent = event.xconfigure;

                if (static_cast<uint32_t>(confEvent.width) != mWidth ||
                    static_cast<uint32_t>(confEvent.height) != mHeight)
                {
                    mWidth = confEvent.width;
                    mHeight = confEvent.height;
                    this->OnResize(mWidth, mHeight);

                    if (mResizeCallback)
                        mResizeCallback(mResizeCallbackUserData);
                }
                break;
            }
        }
    }
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
    int revert;
    ::Window window;
    XGetInputFocus(mDisplay, &window, &revert);
    return mWindow == window;
}

void* WindowManager::GetHandle() const
{
    return reinterpret_cast<void*>(mWindow);
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

int WindowManager::ErrorHandler(::Display *dpy, XErrorEvent *error)
{
    char errorCode[1024];
    XGetErrorText(dpy, error->error_code, errorCode, 1024);
    printf("_X Error of failed request: %s\n", errorCode);
    WindowManager::mWindowError = true;
    return 0;
}
