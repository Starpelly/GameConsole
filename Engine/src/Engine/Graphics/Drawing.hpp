#pragma once

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/Sprite.hpp"

namespace Soulcast
{
	extern int GFX_LINESIZE;

	struct ScreenInfo
	{
		uint16* frameBuffer = nullptr;
		Vector2 position{};
		Vector2 size{};

		int32 pitch = 0;
		int32 clipBound_X1 = 0;
		int32 clipBound_Y1 = 0;
		int32 clipBound_X2 = 0;
		int32 clipBound_Y2 = 0;

		bool ownsFrameBuffer = false; // Used for the destructor

#if SOULCAST_USING_SDL3
		SDL_Texture* screenTexture = nullptr;
#endif

		~ScreenInfo()
		{
			if (ownsFrameBuffer)
			{
				free(frameBuffer);
			}
		}
	};

	enum class ColorMode
	{
		Indirect,	// "Indirect color mode", Use palettes when drawing
		Direct,		// "Direct color mode", Use the bitmap colors when drawing
	};

	enum SpriteFlip
	{
		FLIP_NONE = 1 << 0,
		FLIP_X = 1 << 1,
		FLIP_Y = 1 << 2,
	};

	/// The Picture Processing Unit
	class Drawing
	{
	public:
		static void Init();
		static void Release();
		static void Present(const ScreenInfo& screen, int32 x, int32 y, int32 w, int32 h);
		static void SetActiveScreen(ScreenInfo* screen);

		static void RenderPalette(int32 bank, int32 y);

		static void ClearScreen(uint16 color);

		static void SetColorMode(ColorMode mode);

		static uint16 GetPixel(int32 x, int32 y);
		static void SetPixel(int32 x, int32 y, uint8 color);

		static Vector2 GetScreenPosition();
		static void SetScreenPosition(int32 x, int32 y);

		static void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint16 color);
		static void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint16 color);

		static void DrawBackground(Bitmap* bitmap, int32 x, int32 y);
		static void DrawSprite(Sprite* sprite, int32 x, int32 y);
		static void DrawSpriteRegion(Sprite* sprite, int32 x, int32 y, int32 sprX, int32 sprY, int32 sprWidth, int32 sprHeight, SpriteFlip flip = FLIP_NONE);

		static void ApplyMosaicEffect(int32 size);

	public:
		static void InitScreenInfo(ScreenInfo* screen, int32 width, int32 height, bool gameScreen);
	};
}