#include <QApplication>
#include <phantomstyle.h>
#include <SDL3/SDL.h>

#include "SDK/MainWindow.hpp"

static void ApplyStyle()
{
    PhantomStyle* style = new PhantomStyle();
    QApplication::setStyle(style);
}

static void ApplyPalette()
{
    QPalette darkPal;
    darkPal.setColor(QPalette::Window, QColor(0xFF292929));
    darkPal.setColor(QPalette::Base, QColor(0xFF232323));
    darkPal.setColor(QPalette::AlternateBase, QColor(0xFF2B2C2D));
    darkPal.setColor(QPalette::ToolTipBase, QColor(0xFF171819));
    darkPal.setColor(QPalette::ToolTipText, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::Text, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::WindowText, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::Button, QColor(0xFF333333));
    darkPal.setColor(QPalette::ButtonText, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::BrightText, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::Highlight, QColor(34, 91, 149, 150));
    darkPal.setColor(QPalette::Light, QColor(0xFF292A2B));
    darkPal.setColor(QPalette::Midlight, QColor(0xFF333333));
    darkPal.setColor(QPalette::Mid, QColor(0xFF292929));
    darkPal.setColor(QPalette::Dark, QColor(0xFF232323));
    darkPal.setColor(QPalette::HighlightedText, QColor(0xFFD4D1D0));
    darkPal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x60A4A6A8));
    darkPal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x60A4A6A8));
    darkPal.setColor(QPalette::Disabled, QPalette::Text, QColor(0x60A4A6A8));
    darkPal.setColor(QPalette::Shadow, Qt::black);

    QApplication::setPalette(darkPal);
}

int main(int argc, char *argv[])
{
    bool sdl_initialized = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    if(!sdl_initialized) sdl_initialized = SDL_Init(SDL_INIT_VIDEO);
    if(!sdl_initialized) return -1;

    QApplication a(argc, argv);

    ApplyStyle();
    ApplyPalette();

    MainWindow w;
    w.show();

    const auto ret = a.exec();

    SDL_Quit();

    return ret;
}
