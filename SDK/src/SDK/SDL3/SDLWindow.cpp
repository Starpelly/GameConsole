#include "SDLWindow.hpp"

#include "Engine/Core/Engine.hpp"
#include "SDK.hpp"

SDLWindow::SDLWindow(QWidget* parent) : QWidget(parent), m_Timer(this)
{
    // Turn off double buffering for this widget. Double buffering
    // interferes with the ability for SDL to be properly displayed
    // on the QWidget.
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NativeWindow, true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);

    setFocusPolicy(Qt::ClickFocus);    // Ensure the widget receives keyboard input

    /* setting setUpdatesEnabled to false is MOST important here!
     * if you set it to true or comment it out entirely then when you
     * resize the SDL2Widget by dragging it from corners you will
     * notice the widget will flicker with white frames which are
     * produced by the widget updates.
     */
    setUpdatesEnabled(false);

    initialize();

    // connect(&m_Timer, SIGNAL(timeout()), this, SLOT(render()));
    // m_Timer.start(1000 / 60);
}

SDLWindow::~SDLWindow()
{
    shutdown();
}

void SDLWindow::initialize()
{
    // Create window
    {
        auto winID = reinterpret_cast<void*>(winId());

        auto properties = SDL_CreateProperties();
        SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, winID);

        m_Window = SDL_CreateWindowWithProperties(properties);

        Soulcast::Engine.debugMode = false;
        Soulcast::Engine.windowContained = true;
        Soulcast::Engine.Init(SDK::GetProjectPath().toStdString().c_str(), m_Window);

        SDL_DestroyProperties(properties);
    }

    QMetaObject::invokeMethod(this, "render", Qt::QueuedConnection);
}

void SDLWindow::shutdown()
{
    m_Timer.stop();

    Soulcast::Engine.Release();
}

// ===============
// Private methods
// ===============

void SDLWindow::render()
{
    if (!Soulcast::Engine.initialized)
        return;

    Soulcast::Engine.DoOneFrame();

    // Schedule the next frame as soon as possible
    QMetaObject::invokeMethod(this, "render", Qt::QueuedConnection);
}

// Override default system paint engine to remove flickering
QPaintEngine* SDLWindow::paintEngine() const
{
    return reinterpret_cast<QPaintEngine*>(0);
}

// https://stackoverflow.com/questions/18160051/intercepting-tab-key-press-to-manage-focus-switching-manually
bool SDLWindow::focusNextPrevChild(bool next)
{
    return false;
}