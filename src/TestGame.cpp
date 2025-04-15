#include "Engine/Core/SoulcastEngine.hpp"
#include "TestGame.hpp"

using namespace Soulcast;

void TestGame::Update()
{
	if (Input::CheckButtonDown(INPUT_LEFT))
	{
		x -= 1;
	}
	if (Input::CheckButtonDown(INPUT_RIGHT))
	{
		x += 1;
	}
	if (Input::CheckButtonDown(INPUT_UP))
	{
		y -= 1;
	}
	if (Input::CheckButtonDown(INPUT_DOWN))
	{
		y += 1;

		// Drawing::SetPaletteColor(0, 0xFF00FF);
	}
}

void TestGame::Render()
{
	Drawing::ClearScreen(1);
	Drawing::DrawRectangle(x, y, 16, 16, 2);
	Drawing::DrawRectangle(x - 4, y - 4, 16, 16, 3);
}