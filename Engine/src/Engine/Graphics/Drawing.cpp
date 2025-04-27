#include "Engine/Core/Engine.hpp"

using namespace Soulcast;

int GFX_LINESIZE;

bool screenRelative = false;

// uint16* Drawing::frameBuffer;
// uint16* Drawing::debugFrameBuffer;

ScreenInfo* activeScreen;

ColorMode colorMode;

void Drawing::InitScreenInfo(ScreenInfo* screen, int32 width, int32 height, bool gameScreen)
{
    screen->size.x = width;
    screen->size.y = height;

    if (gameScreen)
    {
        // The game screen uses memory pre allocated that the user can read and write to.
        screen->frameBuffer = reinterpret_cast<uint16*>(&Memory::memory[Memory::FRAMEBUFFER_START]);
        screen->ownsFrameBuffer = false;
    }
    else
    {
        screen->frameBuffer = new uint16[screen->size.x * screen->size.y];
        memset(screen->frameBuffer, 0, (screen->size.x * screen->size.y) * sizeof(uint16));
        screen->ownsFrameBuffer = true;
    }

    screen->pitch = screen->size.x;

    screen->clipBound_X1 = 0;
    screen->clipBound_X2 = screen->size.x;
    screen->clipBound_Y1 = 0;
    screen->clipBound_Y2 = screen->size.y;
}

void Drawing::Init()
{
    colorMode = ColorMode::Indirect;
}

void Drawing::Release()
{
}

void Drawing::SetActiveScreen(ScreenInfo* screen)
{
    activeScreen = screen;
}

void Drawing::Present(const ScreenInfo& screen, int32 x, int32 y, int32 w, int32 h)
{
#if SOULCAST_USING_SDL3
    SDL_FRect destScreenPos = { 0 };
    destScreenPos.x = x;
    destScreenPos.y = y;
    destScreenPos.w = w;
    destScreenPos.h = h;

    // Update screen buffer
    {
        SDL_UpdateTexture(screen.screenTexture, NULL, (void*)screen.frameBuffer, screen.size.x * sizeof(uint16));
    }

    // Actually render the game to the screen
    {
        SDL_RenderTexture(Engine.runningEmulator->renderer, screen.screenTexture, NULL, &destScreenPos);
    }
#endif
}

void Drawing::RenderPalette(int32 bank, int32 y)
{
    const int32 windowPadding = 4;
    const int32 swatchPadding = 0;

    const int32 swatchSize = 2;
    const int32 swatchSpacing = 1;

    const int32 width = DEBUG_XSIZE - (windowPadding * 2);
    const int32 rectsPerLine = width / (swatchSize + swatchSpacing);
    int32 height = 0;
    for (int i = 0; i < PALETTE_BANK_SIZE; i++)
    {
        if ((i) % rectsPerLine == 0)
        {
            height += swatchSize + swatchSpacing;
        }
    }

    int32 rectX = 0;
    int32 rectY = y;
    
    // Background
    // DrawRectangle(windowPadding, windowPadding, width + swatchPadding, height + swatchPadding, RGB888_TO_RGB565(25, 25, 25));

    // Actually draw swatches
    for (int32 i = 0; i < PALETTE_BANK_SIZE; i++)
    {
        if (i != 0) // 0 is transparent
        {
            DrawRectangle(rectX + (windowPadding + swatchPadding), rectY + (windowPadding + swatchPadding), swatchSize, swatchSize, PALETTE_ENTRY_TO_RGB565(bank, i));
        }

        rectX += swatchSize + swatchSpacing;

        if ((i + 1) % rectsPerLine == 0)
        {
            rectX = 0;
            rectY += swatchSize + swatchSpacing;
        }
    }
}

void Drawing::ClearScreen(uint16 color)
{
    // uint16 color = PALETTE_ENTRY_TO_RGB565(colIndex);
    auto* frameBuffer = activeScreen->frameBuffer;
    int32 cnt = activeScreen->size.x * activeScreen->size.y;
    while (cnt--)
    {
        *frameBuffer = color;
        ++frameBuffer;
    }
}

void Drawing::SetColorMode(ColorMode mode)
{
    colorMode = mode;
}

uint16 Drawing::GetPixel(int32 x, int32 y)
{
    if (x < activeScreen->clipBound_X1 || y < activeScreen->clipBound_Y1 || x >= activeScreen->clipBound_X2 || y >= activeScreen->clipBound_Y2) return 0;
    return activeScreen->frameBuffer[x + y * activeScreen->pitch];
}

void Drawing::SetPixel(int32 x, int32 y, uint8 color)
{
    if (x < activeScreen->clipBound_X1 || y < activeScreen->clipBound_Y1 || x >= activeScreen->clipBound_X2 || y >= activeScreen->clipBound_Y2) return;
    activeScreen->frameBuffer[x + y * activeScreen->pitch] = ACTIVE_PALETTE_ENTRY_TO_RGB565(color);
}

Vector2 Drawing::GetScreenPosition()
{
    return activeScreen->position;
}

void Drawing::SetScreenPosition(int32 x, int32 y)
{
    activeScreen->position = { x, y };
}

void Drawing::DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint16 color)
{
    // uint16 color16 = PALETTE_ENTRY_TO_RGB565(colIndex);
    // uint16 color16 = RGB888_TO_RGB565(color.r, color.g, color.b);
    uint16 color16 = color;

    if (width + x > activeScreen->clipBound_X2)
    {
        width = activeScreen->clipBound_X2 - x;
    }

    if (x < activeScreen->clipBound_X1)
    {
        width += x - activeScreen->clipBound_X1;
        x = activeScreen->clipBound_X1;
    }

    if (height + y > activeScreen->clipBound_Y2)
    {
        height = activeScreen->clipBound_Y2 - y;
    }

    if (y < activeScreen->clipBound_Y1)
    {
        height += y - activeScreen->clipBound_Y1;
        y = activeScreen->clipBound_Y1;
    }

    if (width <= 0 || height <= 0)
        return;

    int32 pitch = activeScreen->pitch - width;
    uint16* frameBuffer = &activeScreen->frameBuffer[x + (y * activeScreen->pitch)];

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

void Drawing::DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint16 color)
{
    // uint16 color16 = PALETTE_ENTRY_TO_RGB565(color);
    // uint16 color16 = RGB888_TO_RGB565(color.r, color.g, color.b);
    uint16 color16 = color;

    int32 drawY1 = y1;
    int32 drawX1 = x1;
    int32 drawY2 = y2;
    int32 drawX2 = x2;

    int32 flags1 = 0;
    if (drawX1 >= activeScreen->clipBound_X2)
        flags1 = 2;
    else if (drawX1 < activeScreen->clipBound_X1)
        flags1 = 1;

    if (drawY1 >= activeScreen->clipBound_Y2)
        flags1 |= 8;
    else if (drawY1 < activeScreen->clipBound_Y1)
        flags1 |= 4;

    int32 flags2 = 0;
    if (drawX2 >= activeScreen->clipBound_X2)
        flags2 = 2;
    else if (drawX2 < activeScreen->clipBound_X1)
        flags2 = 1;

    if (drawY2 >= activeScreen->clipBound_Y2)
        flags2 |= 8;
    else if (drawY2 < activeScreen->clipBound_Y1)
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
            x = drawX1 + ((drawX2 - drawX1) * (((activeScreen->clipBound_Y2 - drawY1) << 8) / div) >> 8);
            y = activeScreen->clipBound_Y2;
        }
        else if (curFlags & 4) {
            int32 div = (drawY2 - drawY1);
            if (!div)
                div = 1;
            x = drawX1 + ((drawX2 - drawX1) * (((activeScreen->clipBound_Y1 - drawY1) << 8) / div) >> 8);
            y = activeScreen->clipBound_Y1;
        }
        else if (curFlags & 2) {
            int32 div = (drawX2 - drawX1);
            if (!div)
                div = 1;
            x = activeScreen->clipBound_X2;
            y = drawY1 + ((drawY2 - drawY1) * (((activeScreen->clipBound_X2 - drawX1) << 8) / div) >> 8);
        }
        else if (curFlags & 1) {
            int32 div = (drawX2 - drawX1);
            if (!div)
                div = 1;
            x = activeScreen->clipBound_X1;
            y = drawY1 + ((drawY2 - drawY1) * (((activeScreen->clipBound_X1 - drawX1) << 8) / div) >> 8);
        }

        if (curFlags == flags1) {
            drawX1 = x;
            drawY1 = y;
            flags1 = 0;
            if (x > activeScreen->clipBound_X2) {
                flags1 = 2;
            }
            else if (x < activeScreen->clipBound_X1) {
                flags1 = 1;
            }

            if (y < activeScreen->clipBound_Y1) {
                flags1 |= 4;
            }
            else if (y > activeScreen->clipBound_Y2) {
                flags1 |= 8;
            }
        }
        else {
            drawX2 = x;
            drawY2 = y;
            flags2 = 0;
            if (x > activeScreen->clipBound_X2) {
                flags2 = 2;
            }
            else if (x < activeScreen->clipBound_X1) {
                flags2 = 1;
            }

            if (y < activeScreen->clipBound_Y1) {
                flags2 |= 4;
            }
            else if (y > activeScreen->clipBound_Y2) {
                flags2 |= 8;
            }
        }
    }

    if (drawX1 > activeScreen->clipBound_X2)
        drawX1 = activeScreen->clipBound_X2;
    else if (drawX1 < activeScreen->clipBound_X1)
        drawX1 = activeScreen->clipBound_X1;

    if (drawY1 > activeScreen->clipBound_Y2)
        drawY1 = activeScreen->clipBound_Y2;
    else if (drawY1 < activeScreen->clipBound_Y1)
        drawY1 = activeScreen->clipBound_Y1;

    if (drawX2 > activeScreen->clipBound_X2)
        drawX2 = activeScreen->clipBound_X2;
    else if (drawX2 < activeScreen->clipBound_X1)
        drawX2 = activeScreen->clipBound_X1;

    if (drawY2 > activeScreen->clipBound_Y2)
        drawY2 = activeScreen->clipBound_Y2;
    else if (drawY2 < activeScreen->clipBound_Y1)
        drawY2 = activeScreen->clipBound_Y1;

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

    uint16* frameBuffer = &activeScreen->frameBuffer[drawX1 + (drawY1 * activeScreen->pitch)];

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
                frameBuffer -= activeScreen->pitch;
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
                    frameBuffer += activeScreen->pitch;
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
	color = fullPalette[paletteIndex].Packed();\
	transparent = paletteIndex == 0;\

void Drawing::DrawBackground(Bitmap* bitmap, int32 x, int32 y)
{
    // These need to be casted to signed integers because we need negative numbers to wrap properly.
    const auto bitmapWidth = (int32)bitmap->width;
    const auto bitmapHeight = (int32)bitmap->height;

    int32 drawWidth = bitmapWidth;
    int32 drawHeight = bitmapHeight;

    if (!screenRelative)
    {
        x -= activeScreen->position.x;
        y -= activeScreen->position.y;
    }

    if (drawWidth > activeScreen->clipBound_X2)
        drawWidth = activeScreen->clipBound_X2;

    if (drawHeight > activeScreen->clipBound_Y2)
        drawHeight = activeScreen->clipBound_Y2;

    if (drawWidth <= 0 || drawHeight <= 0)
        return;

    const int32 hstart = x;
    const int32 vstart = y;

    // Wrapping
    int32 sprX = hstart % bitmapWidth;
    int32 sprY = vstart % bitmapHeight;
    if (sprX < 0)
        sprX += bitmapWidth;
    if (sprY < 0)
        sprY += bitmapHeight;

    // Pitch
    const int32 pitch = activeScreen->pitch - drawWidth;
    const int32 gfxPitch = bitmapWidth - drawWidth;

    // Colors
    const auto fullPalette = activePalette;
    auto frameBuffer = &activeScreen->frameBuffer[0];

    // Blitting/painting
    int nscan = 0;
    auto h = drawHeight;
    while (h--)
    {
        const int32 tx1 = 0;
        const int32 tx2 = activeScreen->clipBound_X2;

        const int32 ypos = (sprY + nscan) % bitmapHeight;
        int32 xpos = (sprX + 0) % bitmapWidth;

        int32 x = tx1;

        while (x < tx2)
        {
            int32 width = bitmapWidth - xpos;
            int32 x1 = x + width;
            if (x1 > tx2)
                x1 = tx2;
            width = x1 - x;

            uint16* dstPixel = (uint16*)frameBuffer;
            uint8* srcPixel = (uint8*)(bitmap->pixels + (ypos)*bitmap->pitch + (xpos)); // palette index

            int32 blitWidth = width;
            while (blitWidth)
            {
                if (*srcPixel)
                {
                    *dstPixel = fullPalette[*srcPixel].Packed();
                }
                srcPixel += 1;
                dstPixel++;
                blitWidth--;
            }

            x += width;
            frameBuffer += width;
            xpos = 0;
        }

        nscan++;
    }
}

void Drawing::DrawSprite(Sprite* sprite, int32 x, int32 y)
{
    if (sprite->bitmap == nullptr)
        return;
    Drawing::DrawSpriteRegion(sprite, x, y, 0, 0, sprite->bitmap->width, sprite->bitmap->height);
}

void Drawing::DrawSpriteRegion(Sprite* sprite, int32 x, int32 y, int32 sprX, int32 sprY, int32 sprWidth, int32 sprHeight, SpriteFlip flip)
{
    if (sprite->bitmap == nullptr) return;

    if (!screenRelative)
    {
        x += activeScreen->position.x;
        y += activeScreen->position.y;
    }

    auto* texture = sprite->bitmap;

    int width = sprWidth;
    int height = sprHeight;

    int widthFlip = width;
    int heightFlip = height;

    if (width + x > activeScreen->clipBound_X2)
        width = activeScreen->clipBound_X2 - x;

    if (x < activeScreen->clipBound_X1) {
        int32 val = x - activeScreen->clipBound_X1;
        sprX -= val;
        width += val;
        widthFlip += 2 * val;
        x = activeScreen->clipBound_X1;
    }

    if (height + y > activeScreen->clipBound_Y2)
        height = activeScreen->clipBound_Y2 - y;

    if (y < activeScreen->clipBound_Y1) {
        int32 val = y - activeScreen->clipBound_Y1;
        sprY -= val;
        height += val;
        heightFlip += 2 * val;
        y = activeScreen->clipBound_Y1;
    }

    if (width <= 0 || height <= 0)
        return;

    auto surface = texture;

    int32 pitch = activeScreen->pitch - width;
    int32 gfxPitch = 0;
    uint8* lineBuffer = NULL;
    uint16* frameBuffer = &activeScreen->frameBuffer[x + activeScreen->pitch * y];

    uint8* indices = texture->pixels;
    PaletteEntry* fullPalette = (colorMode == ColorMode::Indirect) ? activePalette : texture->palette;

    bool flippedX = flip & FLIP_X;
    bool flippedY = flip & FLIP_Y;

    if (!flippedX && !flippedY)
    {
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
    else if (flippedX && !flippedY)
    {
        gfxPitch = width + surface->width;

        indices = &indices[widthFlip - 1 + sprX + texture->width * sprY];

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
                MIXIN_INCREMENT_PIXELS(--);
            }
            frameBuffer += pitch;
            MIXIN_INCREMENT_PIXELS(+= gfxPitch);
        }
    }
    else if (flippedY && !flippedX)
    {
        gfxPitch = width + surface->width;

        indices = &indices[sprX + texture->width * (sprY + heightFlip - 1)];

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
            MIXIN_INCREMENT_PIXELS(-= gfxPitch);
        }
    }
    else
    {
        gfxPitch = surface->width - width;

        indices = &indices[widthFlip - 1 + sprX + texture->width * (sprY + heightFlip - 1)];

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
                MIXIN_INCREMENT_PIXELS(--);
            }
            frameBuffer += pitch;
            MIXIN_INCREMENT_PIXELS(-= gfxPitch);
        }
    }
}

void Drawing::ApplyMosaicEffect(int32 size)
{
    if (size <= 1) return; // A value of 1 is visually indistinguishable

    for (int32 y = 0; y < SCREEN_YSIZE; y += size)
    {
        for (int32 x = 0; x < SCREEN_XSIZE; x += size)
        {
            // Sample the top-left pixel of the block
            int32 topLeftIndex = y * SCREEN_XSIZE + x;
            uint16 color = activeScreen->frameBuffer[topLeftIndex];

            // Fill the block with the sampled pixel
            for (int32 dy = 0; dy < size; ++dy)
            {
                for (int32 dx = 0; dx < size; ++dx)
                {
                    int32 px = x + dx;
                    int32 py = y + dy;
                    if (px < SCREEN_XSIZE && py < SCREEN_YSIZE)
                    {
                        activeScreen->frameBuffer[py * SCREEN_XSIZE + px] = color;
                    }
                }
            }
        }
    }
}
