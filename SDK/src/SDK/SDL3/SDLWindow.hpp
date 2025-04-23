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

private slots:
    void render();
};
