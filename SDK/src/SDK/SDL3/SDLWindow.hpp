#pragma once

#include <QWidget>
#include <QWindow>
#include <QTimer>
#include <QThread>

class SDLWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SDLWindow(Soulcast::Emulator* emu, QWidget* parent = nullptr);
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

    Soulcast::Emulator* m_emulator = nullptr;
};