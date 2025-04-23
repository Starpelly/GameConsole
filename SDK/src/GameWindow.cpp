#include "GameWindow.hpp"
#include "ui_gamewindow.h"

#include <QWindow>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>
#include <QScreen>

#include <QVBoxLayout>

#include "SDLRenderer.hpp"

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    setWindowTitle("Soulcast");

    // SDL setup
    {
        auto sdlRenderer = new SDLRenderer(this);
        auto widget = sdlRenderer->ToWidget(this);

        ui->centralwidget->layout()->addWidget(widget);
    }
}

GameWindow::~GameWindow()
{
    delete ui;
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
