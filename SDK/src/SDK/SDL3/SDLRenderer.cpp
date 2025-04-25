#include "SDLRenderer.hpp"

#include <QWindow>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>
#include <QDebug>
#define TESTING_ENGINE (true)

#if TESTING_ENGINE
#include <Engine/Core/Engine.hpp>
#include "SDK.hpp"
#else
#include <array>
#include "SDLKeymap.hpp"
#endif

static uintptr_t getSDLWindowHandle(SDL_Window* sdl_wnd)
{
    uintptr_t retrieved_handle = 0U;

#if defined(SDL_PLATFORM_WIN32)
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_wnd), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd) {
        retrieved_handle = reinterpret_cast<uintptr_t>(hwnd);
    }
#elif defined(SDL_PLATFORM_MACOS)
    NSWindow *nswindow = (__bridge NSWindow *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    if (nswindow) {
        retrieved_handle = nswindow;
    }
#elif defined(SDL_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        const auto xwindow = SDL_GetNumberProperty(SDL_GetWindowProperties(sdl_wnd), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        retrieved_handle = (uintptr_t)xwindow;
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        struct wl_display *display = (struct wl_display *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_wnd), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        struct wl_surface *surface = (struct wl_surface *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_wnd), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        if (display && surface) {
            retrieved_handle = (uintptr_t)surface;
        }
    } else{
        qDebug() << "Failed to recognize the windowing system!";
    }
#elif defined(SDL_PLATFORM_IOS)
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    UIWindow *uiwindow = (__bridge UIWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
    if (uiwindow) {
        retrieved_handle = uiwindow;
    }
#endif

    return retrieved_handle;
}

SDLRenderer::SDLRenderer(QObject* parent) : QObject(parent) {
#if TESTING_ENGINE
    Soulcast::Engine.debugMode = false;
    Soulcast::Engine.windowContained = true;
    Soulcast::Engine.borderless = true;

    Soulcast::Engine.Init(SDK::GetProjectPath().toStdString().c_str());

    wnd = Soulcast::Engine.window;
    renderer = Soulcast::Engine.renderer;
#else
    auto cleanup = [this]
        {
            if (renderer)
                SDL_DestroyRenderer(renderer);
            if (wnd)
                SDL_DestroyWindow(wnd);
        };

    wnd = SDL_CreateWindow("SDLRenderer", 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    if (!wnd)
    {
        cleanup();
        throw std::runtime_error(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(wnd, nullptr);
    if (!renderer)
    {
        cleanup();
        throw std::runtime_error(SDL_GetError());
    }
#endif

    SDL_ShowWindow(wnd);
    SDL_SyncWindow(wnd);
}

SDLRenderer::~SDLRenderer()
{
#if TESTING_ENGINE
    Soulcast::Engine.Release();
#endif
}

QWidget* SDLRenderer::ToWidget(QWidget* parent)
{
    const auto win_id = getSDLWindowHandle(wnd);
    const auto qwnd = QWindow::fromWinId(win_id);
    auto* widget = QWidget::createWindowContainer(qwnd, parent);

    // Turn off double buffering for this widget. Double buffering
    // interferes with the ability for SDL to be properly displayed
    // on the QWidget.
    widget->setAttribute(Qt::WA_PaintOnScreen);
    widget->setAttribute(Qt::WA_NativeWindow, true);
    widget->setAttribute(Qt::WA_OpaquePaintEvent);
    widget->setAttribute(Qt::WA_NoSystemBackground);

    widget->setMouseTracking(true);

    widget->setFocusPolicy(Qt::ClickFocus);    // Ensure the widget receives keyboard input

    /* setting setUpdatesEnabled to false is MOST important here!
     * if you set it to true or comment it out entirely then when you
     * resize the SDL2Widget by dragging it from corners you will
     * notice the widget will flicker with white frames which are
     * produced by the widget updates.
     */
    widget->setUpdatesEnabled(false);

    return widget;
}

void SDLRenderer::DoOneFrame()
{
    Soulcast::Engine.DoOneFrame();
}
