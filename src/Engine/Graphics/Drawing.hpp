#pragma once

namespace Soulcast
{

namespace Drawing
{
	extern int GFX_LINESIZE;

	void Init();
	void Shutdown();
	void Present();

	void ClearScreen();
	void DrawRectangle(int32 x, int32 y, int32 width, int32 height, const Color& color);
	void DrawLine(int32 x1, int32 y1, int32 x2, int32 y2);
}

}