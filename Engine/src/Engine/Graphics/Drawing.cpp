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

ScreenInfo currentScreen;

Image* testImage;

#define PALETTE_ENTRY_TO_RGB565(entry) \
    RGB888_TO_RGB565(activePalette[entry].r, activePalette[entry].g, activePalette[entry].b);

void Drawing::Init()
{
    currentScreen.pitch = SCREEN_XSIZE;

    currentScreen.clipBound_X1 = 0;
    currentScreen.clipBound_X2 = SCREEN_XSIZE;
    currentScreen.clipBound_Y1 = 0;
    currentScreen.clipBound_Y2 = SCREEN_YSIZE;

    Palette::LoadPaletteBank(activePalette, "D:/Soulcast/test/Waves/Data/Palettes/sonic.pal");
    testImage = new Image("D:/Soulcast/test/Waves/Data/Sprites/Sonic.png");
}

void Drawing::Release()
{
    delete testImage;
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

void Drawing::ClearScreen(uint8 colIndex)
{
    uint16 color = PALETTE_ENTRY_TO_RGB565(colIndex);
    auto* frameBuffer = Engine.frameBuffer;
    int32 cnt = SCREEN_XSIZE * SCREEN_YSIZE;
    while (cnt--)
    {
        *frameBuffer = color;
        ++frameBuffer;
    }
}

uint16 Drawing::GetPixel(int32 x, int32 y)
{
    if (x < currentScreen.clipBound_X1 || y < currentScreen.clipBound_Y1 || x >= currentScreen.clipBound_X2 || y >= currentScreen.clipBound_Y2) return 0;
    return Engine.frameBuffer[x + y * currentScreen.pitch];
}

void Drawing::SetPixel(int32 x, int32 y, uint8 color)
{
    if (x < currentScreen.clipBound_X1 || y < currentScreen.clipBound_Y1 || x >= currentScreen.clipBound_X2 || y >= currentScreen.clipBound_Y2) return;
    Engine.frameBuffer[x + y * currentScreen.pitch] = PALETTE_ENTRY_TO_RGB565(color);
}

void Drawing::DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint8 colIndex)
{
    uint16 color16 = PALETTE_ENTRY_TO_RGB565(colIndex);

    if (width + x > currentScreen.clipBound_X2)
    {
        width = currentScreen.clipBound_X2 - x;
    }

    if (x < currentScreen.clipBound_X1)
    {
        width += x - currentScreen.clipBound_X1;
        x = currentScreen.clipBound_X1;
    }

    if (height + y > currentScreen.clipBound_Y2)
    {
        height = currentScreen.clipBound_Y2 - y;
    }

    if (y < currentScreen.clipBound_Y1)
    {
        height += y - currentScreen.clipBound_Y1;
        y = currentScreen.clipBound_Y1;
    }

    if (width <= 0 || height <= 0)
        return;

    int32 pitch = currentScreen.pitch - width;
    uint16* frameBuffer = &Engine.frameBuffer[x + (y * currentScreen.pitch)];

    int32 h = height;
    while (h--)
    {
        int32 w = width;
        while (w--)
        {
            *frameBuffer = color16;
            ++frameBuffer;
        }

        frameBuffer += pitch;
    }
}

void Drawing::DrawCircle(int32 x, int32 y, int32 radius, uint8 color)
{
    if (radius <= 0) return;
}

void Drawing::DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint8 color)
{
    uint16 color16 = PALETTE_ENTRY_TO_RGB565(color);

    int32 drawY1 = y1;
    int32 drawX1 = x1;
    int32 drawY2 = y2;
    int32 drawX2 = x2;

    int32 flags1 = 0;
    if (drawX1 >= currentScreen.clipBound_X2)
        flags1 = 2;
    else if (drawX1 < currentScreen.clipBound_X1)
        flags1 = 1;

    if (drawY1 >= currentScreen.clipBound_Y2)
        flags1 |= 8;
    else if (drawY1 < currentScreen.clipBound_Y1)
        flags1 |= 4;

    int32 flags2 = 0;
    if (drawX2 >= currentScreen.clipBound_X2)
        flags2 = 2;
    else if (drawX2 < currentScreen.clipBound_X1)
        flags2 = 1;

    if (drawY2 >= currentScreen.clipBound_Y2)
        flags2 |= 8;
    else if (drawY2 < currentScreen.clipBound_Y1)
        flags2 |= 4;

    while (flags1 || flags2) {
        if (flags1 & flags2)
            return;

        int32 curFlags = flags2;
        if (flags1)
            curFlags = flags1;

        int32 x = 0;
        int32 y = 0;
        if (curFlags & 8) {
            int32 div = (drawY2 - drawY1);
            if (!div)
                div = 1;
            x = drawX1 + ((drawX2 - drawX1) * (((currentScreen.clipBound_Y2 - drawY1) << 8) / div) >> 8);
            y = currentScreen.clipBound_Y2;
        }
        else if (curFlags & 4) {
            int32 div = (drawY2 - drawY1);
            if (!div)
                div = 1;
            x = drawX1 + ((drawX2 - drawX1) * (((currentScreen.clipBound_Y1 - drawY1) << 8) / div) >> 8);
            y = currentScreen.clipBound_Y1;
        }
        else if (curFlags & 2) {
            int32 div = (drawX2 - drawX1);
            if (!div)
                div = 1;
            x = currentScreen.clipBound_X2;
            y = drawY1 + ((drawY2 - drawY1) * (((currentScreen.clipBound_X2 - drawX1) << 8) / div) >> 8);
        }
        else if (curFlags & 1) {
            int32 div = (drawX2 - drawX1);
            if (!div)
                div = 1;
            x = currentScreen.clipBound_X1;
            y = drawY1 + ((drawY2 - drawY1) * (((currentScreen.clipBound_X1 - drawX1) << 8) / div) >> 8);
        }

        if (curFlags == flags1) {
            drawX1 = x;
            drawY1 = y;
            flags1 = 0;
            if (x > currentScreen.clipBound_X2) {
                flags1 = 2;
            }
            else if (x < currentScreen.clipBound_X1) {
                flags1 = 1;
            }

            if (y < currentScreen.clipBound_Y1) {
                flags1 |= 4;
            }
            else if (y > currentScreen.clipBound_Y2) {
                flags1 |= 8;
            }
        }
        else {
            drawX2 = x;
            drawY2 = y;
            flags2 = 0;
            if (x > currentScreen.clipBound_X2) {
                flags2 = 2;
            }
            else if (x < currentScreen.clipBound_X1) {
                flags2 = 1;
            }

            if (y < currentScreen.clipBound_Y1) {
                flags2 |= 4;
            }
            else if (y > currentScreen.clipBound_Y2) {
                flags2 |= 8;
            }
        }
    }

    if (drawX1 > currentScreen.clipBound_X2)
        drawX1 = currentScreen.clipBound_X2;
    else if (drawX1 < currentScreen.clipBound_X1)
        drawX1 = currentScreen.clipBound_X1;

    if (drawY1 > currentScreen.clipBound_Y2)
        drawY1 = currentScreen.clipBound_Y2;
    else if (drawY1 < currentScreen.clipBound_Y1)
        drawY1 = currentScreen.clipBound_Y1;

    if (drawX2 > currentScreen.clipBound_X2)
        drawX2 = currentScreen.clipBound_X2;
    else if (drawX2 < currentScreen.clipBound_X1)
        drawX2 = currentScreen.clipBound_X1;

    if (drawY2 > currentScreen.clipBound_Y2)
        drawY2 = currentScreen.clipBound_Y2;
    else if (drawY2 < currentScreen.clipBound_Y1)
        drawY2 = currentScreen.clipBound_Y1;

    int32 sizeX = abs(drawX2 - drawX1);
    int32 sizeY = abs(drawY2 - drawY1);
    int32 max = sizeY;
    int32 hSize = sizeX >> 2;
    if (sizeX <= sizeY)
        hSize = -sizeY >> 2;

    if (drawX2 < drawX1) {
        int32 v = drawX1;
        drawX1 = drawX2;
        drawX2 = v;

        v = drawY1;
        drawY1 = drawY2;
        drawY2 = v;
    }

    uint16* frameBuffer = &Engine.frameBuffer[drawX1 + (drawY1 * currentScreen.pitch)];

    if (drawY1 > drawY2) {
        while (drawX1 < drawX2 || drawY1 >= drawY2) {
            *frameBuffer = color16;

            if (hSize > -sizeX) {
                hSize -= max;
                ++drawX1;
                ++frameBuffer;
            }

            if (hSize < max) {
                --drawY1;
                hSize += sizeX;
                frameBuffer -= currentScreen.pitch;
            }
        }
    }
    else {
        while (true) {
            *frameBuffer = color16;

            if (drawX1 < drawX2 || drawY1 < drawY2) {
                if (hSize > -sizeX) {
                    hSize -= max;
                    ++drawX1;
                    ++frameBuffer;
                }

                if (hSize < max) {
                    hSize += sizeX;
                    ++drawY1;
                    frameBuffer += currentScreen.pitch;
                }
            }
            else {
                break;
            }
        }
    }
}

#define MIXIN_INCREMENT_PIXELS(incr) indices incr

#define MIXIN_PIXEL_COLOR() \
	uint16 color; \
	bool transparent = false; \
	\
    uint8 paletteIndex = *indices;\
	color = fullPalette[paletteIndex];\
	transparent = paletteIndex == 0;\

void Drawing::DrawSprite(int32 x, int32 y)
{
    x *= -1;
    y *= -1;

    auto* texture = testImage;
    bool flippedX = false;
    bool flippedY = false;

    int32 sprX = 0;
    int32 sprY = 0;

    int width = texture->width;
    int height = texture->height;

    int widthFlip = width;
    int heightFlip = height;

    if (width + x > currentScreen.clipBound_X2)
        width = currentScreen.clipBound_X2 - x;

    if (x < currentScreen.clipBound_X1) {
        int32 val = x - currentScreen.clipBound_X1;
        sprX -= val;
        width += val;
        widthFlip += 2 * val;
        x = currentScreen.clipBound_X1;
    }

    if (height + y > currentScreen.clipBound_Y2)
        height = currentScreen.clipBound_Y2 - y;

    if (y < currentScreen.clipBound_Y1) {
        int32 val = y - currentScreen.clipBound_Y1;
        sprY -= val;
        height += val;
        heightFlip += 2 * val;
        y = currentScreen.clipBound_Y1;
    }

    if (width <= 0 || height <= 0)
        return;

    auto surface = texture;

    int32 pitch = currentScreen.pitch - width;
    int32 gfxPitch = 0;
    uint8* lineBuffer = NULL;
    uint16* frameBuffer = &Engine.frameBuffer[x + currentScreen.pitch * y];

    uint8* indices = texture->pixels;
    uint16* fullPalette = texture->palette;

    gfxPitch = surface->width - width;

    indices = &indices[sprX + texture->width * sprY];

    while (height--)
    {
        int32 w = width;
        while (w--)
        {
            MIXIN_PIXEL_COLOR();
            if (!transparent)
            {
                *frameBuffer = color;
            }
            ++frameBuffer;
            MIXIN_INCREMENT_PIXELS(++);
        }
        frameBuffer += pitch;
        MIXIN_INCREMENT_PIXELS(+= gfxPitch);
    }
}
