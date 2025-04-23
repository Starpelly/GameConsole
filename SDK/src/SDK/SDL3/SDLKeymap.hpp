#pragma once

#include <QKeyEvent>
#include <SDL3/SDL_scancode.h>

Qt::Key toQtKey(SDL_Scancode sdl_key);
