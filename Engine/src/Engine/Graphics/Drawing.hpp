#pragma once

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/Sprite.hpp"

namespace Soulcast
{
	namespace PPU
	{
		extern int GFX_LINESIZE;

		void Init();
		void Release();
		void Present();

		void ClearScreen(uint8 color);

		uint16 GetPixel(int32 x, int32 y);
		void SetPixel(int32 x, int32 y, uint8 color);

		Vector2 GetScreenPosition();
		void SetScreenPosition(int32 x, int32 y);

		void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint8 color);
		void DrawCircle(int32 x, int32 y, int32 radius, uint8 color);
		void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint8 color);

		void DrawBackground(Image* image, int32 x, int32 y);
		void DrawSprite(Sprite* sprite, int32 x, int32 y);

		void ApplyMosaicEffect(int32 size);
	}
}