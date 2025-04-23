#include "SDLWindow.hpp"

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

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(Render()));
    m_Timer.start(1000 / 60);
}

SDLWindow::~SDLWindow()
{
    // Shutdown();
}

void SDLWindow::initialize()
{
    // Create window
    {
        auto winID = reinterpret_cast<void*>(winId());

        auto properties = SDL_CreateProperties();
        SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, winID);

        SDL_DestroyProperties(properties);
    }
}

void SDLWindow::shutdown()
{
    m_Timer.stop();
}

// ===============
// Private methods
// ===============

void SDLWindow::render()
{
}
