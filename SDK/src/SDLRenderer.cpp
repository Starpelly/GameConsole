#include "SDLRenderer.hpp"

#include <QWindow>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>
#include <QDebug>

#include <array>

#include "SDLKeymap.hpp"

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

SDLRenderer::SDLRenderer(QObject* parent) : QObject(parent), event_timer(this) {
    auto cleanup = [this]
    {
        if(renderer)
            SDL_DestroyRenderer(renderer);
        if(wnd)
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

    SDL_ShowWindow(wnd);
    SDL_SyncWindow(wnd);

    event_timer.setInterval(30);
    event_timer.setTimerType(Qt::CoarseTimer);
    connect(&event_timer, &QTimer::timeout, this, &SDLRenderer::handleSDLEvents);
    event_timer.start();
}

SDLRenderer::~SDLRenderer()
{
    clearDisplay();
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(wnd)
        SDL_DestroyWindow(wnd);
}

QWidget* SDLRenderer::ToWidget(QWidget* parent)
{
    const auto win_id = getSDLWindowHandle(wnd);
    const auto qwnd = QWindow::fromWinId(win_id);
    return QWidget::createWindowContainer(qwnd, parent);
}

void SDLRenderer::handleSDLEvents()
{
    constexpr auto SDL_EVT_COUNT = 3;
    std::array<SDL_Event, SDL_EVT_COUNT> evts{};
    SDL_PumpEvents();
    const int evts_received = SDL_PeepEvents(evts.data(), evts.size(), SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST);
    for(int i = 0; i < evts_received; ++i)
        processSDLEvent(evts[i]);
}

void SDLRenderer::processSDLEvent(const SDL_Event& evt)
{
    switch(evt.type){
    case SDL_EVENT_KEY_DOWN:
    {
        const auto qtKey = toQtKey(evt.key.scancode);
        if(qtKey != Qt::Key_unknown)
        {
            QApplication::postEvent(QApplication::instance(), new QKeyEvent(QEvent::KeyPress, qtKey, Qt::NoModifier));
        }
    }
    break;
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        // SDL_ShowCursor();
        // QTimer::singleShot(900, this, []{SDL_HideCursor();});
    }
    break;
    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_EXPOSED:
    case SDL_EVENT_WINDOW_RESIZED:
        if(evt.type == SDL_EVENT_WINDOW_RESIZED){
            window_width = evt.window.data1;
            window_height = evt.window.data2;
            qDebug() << "Window wxh: " << window_width << "x" <<window_height;
        }
        refreshDisplay();
        break;
    }
}

void SDLRenderer::refreshDisplay()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    // if(!vid_texture){ SDL_RenderPresent(renderer); return; }

    // const auto rect = calculate_display_rect(0, 0, window_width, window_height, last_frame_width, last_frame_height, last_sar);

    // const auto res = SDL_RenderTextureRotated(renderer, vid_texture, NULL, &rect, 0, NULL, flip_v ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);
}

void SDLRenderer::clearDisplay()
{
    /*
    if(sub_texture)
        SDL_DestroyTexture(sub_texture);
    if(vid_texture)
        SDL_DestroyTexture(vid_texture);
    */
    last_frame_width = last_frame_height = 0;
}
