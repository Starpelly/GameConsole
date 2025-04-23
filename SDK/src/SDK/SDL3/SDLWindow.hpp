#pragma once

#include <QWidget>
#include <QWindow>
#include <QTimer>
#include <QThread>

#include <SDL3/SDL.h>

class SDLWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SDLWindow(QWidget* parent = nullptr);
    ~SDLWindow();

private:
    void initialize();
    void shutdown();

    QTimer m_Timer;
    SDL_Window* m_Window;

private slots:
    void render();

private:
    QPaintEngine* paintEngine() const override;
    bool focusNextPrevChild(bool next) override;
};
