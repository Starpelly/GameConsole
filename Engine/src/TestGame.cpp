#include "Engine/Core/SoulcastEngine.hpp"
#include "TestGame.hpp"

#include <iostream>
#include <chrono>

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

const int rx = SCREEN_XSIZE / 2;
const int ry = SCREEN_YSIZE / 2;

static float t()
{
	using namespace std::chrono;
	static auto start = steady_clock::now();
	auto now = steady_clock::now();
	duration<float> elapsed = now - start;
	return elapsed.count();
}

void TestGame::Render()
{
	Drawing::ClearScreen(1);

	for (int32 y = -ry; y < ry; y += 3)
	{
		for (int32 x = -rx; x < rx; x += 2)
		{
			float32 dist = std::sqrt(static_cast<float32>(x * x + y * y));
			float32 z = std::cos(dist / 6.0f - (t() * 8)) * 6.0f;
			Drawing::SetPixel(rx + x, static_cast<int32>(ry + y - z), 7);
		}
	}
}