#include "Engine/Core/SoulcastEngine.hpp"
#include "TestGame.hpp"

#include <iostream>
#include <chrono>
#include <sstream>

using namespace Soulcast;

struct TestAudioPlayer
{
	float freq = 440.0f;
	float duty = 0.25f;
};

static int fileTest = 0;

TestGame::TestGame()
{
}

static void loadPCMFile(int test)
{
	std::ostringstream filename;
	filename << "C:/Users/Braedon/Downloads/programmable_wave_samples/";
	filename << std::setw(2) << std::setfill('0') << test;
	filename << ".pcm";
	Engine.soundChip.pcm = load4BitPCMFile(filename.str().c_str());

	std::cout << "Loading sample " << filename.str() << std::endl;
}

void TestGame::Update()
{
	if (Input::IsButtonDown(INPUT_RIGHT))
	{
		speed += 0.02f;
	}
	else if (speed > 0.0f)
	{
		speed -= 0.02f;
	}

	if (Input::IsButtonDown(INPUT_LEFT))
	{
		speed -= 0.02f;
	}
	else if (speed < 0.0f)
	{
		speed += 0.02f;
	}

	pos_x += 6.0f * speed;

	x = (int32)pos_x;
	y = (int32)pos_y;

	if (Input::IsButtonPressed(INPUT_DOWN))
	{
		// currentPlayer->duty -= 1 / 16.0f;
		// AudioDevice::TestThing(freq);
		fileTest--;
		loadPCMFile(fileTest);
	}
	if (Input::IsButtonPressed(INPUT_UP))
	{
		// currentPlayer->duty += 1 / 16.0f;
		// AudioDevice::TestThing(freq);

		fileTest++;
		loadPCMFile(fileTest);
	}

	auto mousePan = Math::lerp(-1, 1, ((float)Input::mouseX) / (SCREEN_XSIZE * Engine.windowScale));
	auto mouseFreq = -(Input::mouseY - (SCREEN_YSIZE * Engine.windowScale));
	AudioDevice::SetPCMFreq(mouseFreq);
	AudioDevice::SetPCMPan(mousePan);
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
	Drawing::ClearScreen(9);

	/*
	for (int32 y = -ry; y < ry; y += 3)
	{
		for (int32 x = -rx; x < rx; x += 2)
		{
			float32 dist = std::sqrt(static_cast<float32>(x * x + y * y));
			float32 z = std::cos(dist / 6.0f - (t() * 8)) * 6.0f;
			Drawing::SetPixel(rx + x, static_cast<int32>(ry + y - z), 7);
		}
	}
	*/

	// Drawing::DrawRectangle(32, -(player1.freq - (SCREEN_YSIZE / Engine.windowScale)), 8, 8, currentPlayer == &player1 ? 3 : 2);
	// Drawing::DrawRectangle(128, -player2.freq, 8, 8, currentPlayer == &player2 ? 3 : 2);

	// Palette::SetPaletteColor(3, PaletteEntry(188, 100, 93));

	// Drawing::DrawSprite(x, y);
}