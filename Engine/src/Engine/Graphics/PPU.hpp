#pragma once

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/Sprite.hpp"

namespace Soulcast
{
	extern int GFX_LINESIZE;

	struct ScreenInfo
	{
		uint16* frameBuffer;
		Vector2 position;
		Vector2 size;

		int32 pitch;
		int32 clipBound_X1;
		int32 clipBound_Y1;
		int32 clipBound_X2;
		int32 clipBound_Y2;
	};

	/// The Picture Processing Unit
	class PPU
	{
	public:
		// static uint16* frameBuffer;
		// static uint16* debugFrameBuffer;
		static ScreenInfo gameScreen;
		static ScreenInfo debugScreen;

		static void Init();
		static void Release();
		static void Present();
		static void SetActiveScreen(ScreenInfo* screen);

		static void RenderPalette(int32 bank);

		static void ClearScreen(uint16 color);

		static uint16 GetPixel(int32 x, int32 y);
		static void SetPixel(int32 x, int32 y, uint8 color);

		static Vector2 GetScreenPosition();
		static void SetScreenPosition(int32 x, int32 y);

		static void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint16 color);
		static void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint16 color);

		static void DrawBackground(Image* image, int32 x, int32 y);
		static void DrawSprite(Sprite* sprite, int32 x, int32 y);

		static void ApplyMosaicEffect(int32 size);
	};
}