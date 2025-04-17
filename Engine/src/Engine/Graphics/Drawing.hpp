#pragma once

namespace Soulcast
{
	namespace Drawing
	{
		extern int GFX_LINESIZE;

		void Init();
		void Release();
		void Present();

		void ClearScreen(uint8 color);

		uint16 GetPixel(int32 x, int32 y);
		void SetPixel(int32 x, int32 y, uint8 color);

		void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint8 color);
		void DrawCircle(int32 x, int32 y, int32 radius, uint8 color);
		void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint8 color);

		void DrawSprite(int32 x, int32 y);

		// void DrawSprite(int32 xPos, int32 yPos);
	}
}