#pragma once

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/Sprite.hpp"

namespace Soulcast
{
	extern int GFX_LINESIZE;

	/// The Picture Processing Unit
	class PPU
	{
	public:
		static uint16* frameBuffer;

		static void Init();
		static void Release();
		static void Present();

		static void ClearScreen(uint8 color);

		static uint16 GetPixel(int32 x, int32 y);
		static void SetPixel(int32 x, int32 y, uint8 color);

		static Vector2 GetScreenPosition();
		static void SetScreenPosition(int32 x, int32 y);

		static void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint8 color);
		static void DrawCircle(int32 x, int32 y, int32 radius, uint8 color);
		static void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint8 color);

		static void DrawBackground(Image* image, int32 x, int32 y);
		static void DrawSprite(Sprite* sprite, int32 x, int32 y);

		static void ApplyMosaicEffect(int32 size);
	};
}