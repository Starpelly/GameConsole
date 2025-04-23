#pragma once

#include <SDL3/SDL.h>

#include <QWidget>
#include <QTimer>

class SDLRenderer final : public QObject
{
    Q_OBJECT;
    Q_DISABLE_COPY_MOVE(SDLRenderer);

    SDL_Window* wnd = nullptr;
    SDL_Renderer* renderer = nullptr;

    QTimer event_timer;

    int window_width = 0, window_height = 0;
    int last_frame_width = 0, last_frame_height = 0;

    uintptr_t getWindowHandle();
    Q_SLOT void handleSDLEvents();
    void processSDLEvent(const SDL_Event& evt);

public:
    SDLRenderer(QObject* parent = nullptr);
    ~SDLRenderer();

    QWidget* ToWidget(QWidget* parent);

    void refreshDisplay();
    void clearDisplay();
};
