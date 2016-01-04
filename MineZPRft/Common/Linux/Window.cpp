/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  WindowManager class Linux implementation.
 */

#include "../Window.hpp"

#include "../Common.hpp"
#include  "../Logger.hpp"

#include <iostream>
#include <iomanip>

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
    mContext = 0;
    mDrawable = 0;

    if (!mDisplay)
    {
        mDisplay = XOpenDisplay(nullptr);
        if (mDisplay == nullptr)
        {
            LOG_E("Cannot connect to X server\n");
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
    glXMakeCurrent(mDisplay, None, 0);
    glXDestroyContext(mDisplay, mContext);
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

    LOG_I("Found " << fbCount << " matching FB configs:\n");

    // Select the best FB Config according to lowest GLX_SAMPLES attribute value
    // TODO enable Multisampling if needed
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
            LOG_I("  #" << i << ": visualID 0x" << std::setfill('0')
                << std::setw(2) << vi->visualid << ", SAMPLE_BUFFERS = "
                << sampleBuffers << ", SAMPLES = " << samples);

            if (samples < maxSamples)
            {
                bestFBID = i;
                maxSamples = samples;
            }
        }
        XFree(vi);
    }

    LOG_I("Choosing FB config #" << bestFBID);
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

    XStoreName(mDisplay, mWindow, mTitle.c_str());
    ::Atom WmDelete = XInternAtom(mDisplay, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(mDisplay, mWindow, &WmDelete, 1);
    XMapWindow(mDisplay, mWindow);

    // Acquire glX extension needed for initialization
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
        reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
            glXGetProcAddress((const GLubyte*) "glXCreateContextAttribsARB")
        );

    if (glXCreateContextAttribsARB)
    {
        // try reaching for 3.3 core OpenGL context
        int attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            None
        };

        mContext = glXCreateContextAttribsARB(mDisplay, bestFB, NULL, GL_TRUE, attribs);
        if (!mContext)
        {
            LOG_W("GL 3.3 core profile not acquired. Falling back to old profile.");
            LOG_W("Keep in mind, the renderer MIGHT NOT WORK due to too old OGL version!");
            // failed, fallback to classic method
            mContext = glXCreateNewContext(mDisplay, bestFB, GLX_RGBA_TYPE, NULL, GL_TRUE);
            if (!mContext)
            {
                LOG_E("Cannot create OpenGL Context.");
                // TODO exception
                return false;
            }
        }
    }
    else
    {
        LOG_W("glXCreateContextAttribsARB not available. Creating OGL context the old way.");
        LOG_W("Keep in mind, the renderer MIGHT NOT WORK due to too old OGL version!");
        mContext = glXCreateNewContext(mDisplay, bestFB, GLX_RGBA_TYPE, NULL, GL_TRUE);
        if (!mContext)
        {
            LOG_E("Cannot create OpenGL Context.");
            // TODO exception
            return false;
        }
    }
    glXMakeCurrent(mDisplay, mWindow, mContext);
    mDrawable = glXGetCurrentDrawable();

    // retrieve glXSwapIntervalEXT for v-sync control
    PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
            reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(
                glXGetProcAddress((const GLubyte*) "glXSwapIntervalEXT")
            );

    // Disable v-sync if possible. Refer to Win/Window.cpp for explanation.
    if (glXSwapIntervalEXT)
        glXSwapIntervalEXT(mDisplay, mDrawable, 0);

    // for information purposes
    if (!glXIsDirect(mDisplay, mContext))
        LOG_I("Indirect GLX rendering context obtained");
    else
        LOG_I("Direct GLX rendering context obtained");

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

void WindowManager::GetDesktopResolution()
{
    XWindowAttributes desktopAttr;
    XGetWindowAttributes(mDisplay, mRoot, &desktopAttr);

    mDesktopWidth = desktopAttr.width;
    mDesktopHeight = desktopAttr.height;
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
                    Close();
                    OnClose();
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
                mKeys[event.xkey.keycode] = false;
                OnKeyUp(static_cast<int>(event.xkey.keycode));
                break;
            }
            case MotionNotify:
            {
                MouseMove(event.xmotion.x, event.xmotion.y);
                break;
            }
            case ButtonPress:
            {
                if (event.xbutton.button < 4) // 1-3 MBtns, 4-5 MWheel
                    //can be event.x_root,y_root - then it's relative to ROOT window instead
                    MouseDown(event.xbutton.button - 1, event.xbutton.x, event.xbutton.y);
                else if (event.xbutton.button == 4)
                    OnScroll(1); // btn==4 is UP,
                else
                    OnScroll(-1); // btn==5 is DOWN
                break;
            }
            case ButtonRelease:
            {
                MouseUp(event.xbutton.button - 1);
                break;
            }
            case FocusOut:
            {
                LostFocus();
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
                    OnResize(mWidth, mHeight);

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

void WindowManager::SwapBuffers() const
{
    glXSwapBuffers(mDisplay, mDrawable);
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
    UNUSED(width);
    UNUSED(height);
}

void WindowManager::OnKeyPress(int key)
{
    UNUSED(key);
}

void WindowManager::OnKeyUp(int key)
{
    UNUSED(key);
}

void WindowManager::OnScroll(int delta)
{
    UNUSED(delta);
}

void WindowManager::OnMouseDown(uint32_t button, int x, int y)
{
    UNUSED(button);
    UNUSED(x);
    UNUSED(y);
}

void WindowManager::OnMouseMove(int x, int y, int deltaX, int deltaY)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(deltaX);
    UNUSED(deltaY);
}

void WindowManager::OnMouseUp(uint32_t button)
{
    UNUSED(button);
}

int WindowManager::ErrorHandler(::Display *dpy, XErrorEvent *error)
{
    char errorCode[1024];
    XGetErrorText(dpy, error->error_code, errorCode, 1024);
    LOG_E("_X Error of failed request: " << errorCode);
    WindowManager::mWindowError = true;
    return 0;
}
