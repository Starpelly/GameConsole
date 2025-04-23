#include "GameWindow.hpp"
#include "ui_gamewindow.h"

#include <QWindow>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>
#include <QScreen>

#include <QVBoxLayout>

#include "Engine/Core/Engine.hpp"

// #include "SDK/SDL3/SDLRenderer.hpp"
// SDLRenderer* sdlRenderer;
#include "SDK/SDL3/SDLWindow.hpp"

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    // Initial widget setup
    {
        const int statusBarHeight = 22;
        const int menuBarHeight = 20;

        setWindowTitle("Soulcast");
        resize(SCREEN_XSIZE * Soulcast::Engine.windowScale, (SCREEN_YSIZE * Soulcast::Engine.windowScale) + statusBarHeight + menuBarHeight);
    }

    // SDL setup
    {
        // sdlRenderer = new SDLRenderer(this);
        // auto widget = sdlRenderer->ToWidget(this);

        auto widget = new SDLWindow(this);
        ui->centralwidget->layout()->addWidget(widget);
    }

    // Start timer
    {
        connect(&timer, &QTimer::timeout, this, &GameWindow::doOneFrame);
        timer.start(1000 / 60);
    }
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::doOneFrame()
{
    // sdlRenderer->DoOneFrame();
}

void GameWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // Center the window to the screen
    QRect screenGeometry = screen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void GameWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);

    emit whenClosed();
}
