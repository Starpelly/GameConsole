#include "Engine/Core/SoulcastEngine.hpp"

using namespace Soulcast;

int GFX_LINESIZE;

struct ScreenInfo
{
    int32 pitch;
    int32 clipBound_X1;
    int32 clipBound_Y1;
    int32 clipBound_X2;
    int32 clipBound_Y2;
};

ScreenInfo m_CurrentScreen;

void Drawing::Init()
{
    m_CurrentScreen.pitch = SCREEN_XSIZE;

    m_CurrentScreen.clipBound_X1 = 0;
    m_CurrentScreen.clipBound_X2 = SCREEN_XSIZE;
    m_CurrentScreen.clipBound_Y1 = 0;
    m_CurrentScreen.clipBound_Y2 = SCREEN_YSIZE;
}

void Drawing::Shutdown()
{

}

void Drawing::Present()
{
#if SOULCAST_USING_SDL3
    SDL_FRect destScreenPos = { 0 };
    destScreenPos.x = 0;
    destScreenPos.y = 0;
    destScreenPos.w = SCREEN_XSIZE;
    destScreenPos.h = SCREEN_YSIZE;

    // Update screen buffer
    {
        SDL_UpdateTexture(Engine.screenBuffer, NULL, (void*)Engine.frameBuffer, SCREEN_XSIZE * sizeof(uint16));
    }

    // Actually render that shit to the screen
    {
        SDL_SetRenderDrawColor(Engine.renderer, 0, 0, 0, 255);
        SDL_RenderClear(Engine.renderer);

        SDL_RenderTexture(Engine.renderer, Engine.screenBuffer, NULL, &destScreenPos);

        SDL_RenderPresent(Engine.renderer);
    }
#endif
}

void Drawing::ClearScreen()
{
    uint16 color = 0xF800;
    auto* frameBuffer = Engine.frameBuffer;
    int cnt = SCREEN_XSIZE * SCREEN_YSIZE;
    while (cnt--)
    {
        *frameBuffer = color;
        ++frameBuffer;
    }
}

void Drawing::DrawRectangle(int32 x, int32 y, int32 width, int32 height)
{
    uint16 color = 0xFFFF;

    if (width + x > m_CurrentScreen.clipBound_X2)
    {
        width = m_CurrentScreen.clipBound_X2 - x;
    }

    if (x < m_CurrentScreen.clipBound_X1)
    {
        width += x - m_CurrentScreen.clipBound_X1;
        x = m_CurrentScreen.clipBound_X1;
    }

    if (height + y > m_CurrentScreen.clipBound_Y2)
    {
        height = m_CurrentScreen.clipBound_Y2 - y;
    }

    if (y < m_CurrentScreen.clipBound_Y1)
    {
        height += y - m_CurrentScreen.clipBound_Y1;
        y = m_CurrentScreen.clipBound_Y1;
    }

    if (width <= 0 || height <= 0)
        return;

    int32 pitch = m_CurrentScreen.pitch - width;
    uint16* frameBuffer = &Engine.frameBuffer[x + (y * m_CurrentScreen.pitch)];

    int32 h = height;
    while (h--)
    {
        int32 w = width;
        while (w--)
        {
            *frameBuffer = color;
            ++frameBuffer;
        }

        frameBuffer += pitch;
    }
}
