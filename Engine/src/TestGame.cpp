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

struct
{
	Vector2 cameraPos;
} State;

static int fileTest = 0;

Bitmap testImage1;
Bitmap testImage2;
Bitmap marioTexture;

Sprite testSprite;

TestGame::TestGame()
{
	testImage1.Load("Sprites/switch.png");
	testImage2.Load("Sprites/palacebg.png");

	marioTexture.Load("Sprites/mario.png");

	Palette::LoadPaletteBank(0, "Palettes/switch.pal");
	Palette::LoadPaletteBank(1, "Palettes/palacebg.pal");
	Palette::LoadPaletteBank(2, "Palettes/mario.pal");

	testSprite.bitmap = &marioTexture;
}

TestGame::~TestGame()
{
}

static void loadPCMFile(int test)
{
	std::ostringstream filename;
	filename << "SoundFX/programmable_wave_samples/";
	filename << std::setw(2) << std::setfill('0') << test;
	filename << ".pcm";
	Engine.soundChip.pcm = load4BitPCMFile(filename.str().c_str());

	std::cout << "Loading sample " << filename.str() << std::endl;
}

void TestGame::Update()
{
	if (Input::IsButtonDown(INPUT_RIGHT))
	{
		speed = 0.02f;
	}
	else if (Input::IsButtonDown(INPUT_LEFT))
	{
		speed = -0.02f;
	}
	else
	{
		speed = 0.0f;
	}

	pos_x += 256.0f * speed;

	State.cameraPos.x = (int32)pos_x;
	State.cameraPos.y = (int32)pos_y;

	if (Input::IsButtonPressed(INPUT_DOWN))
	{
		// currentPlayer->duty -= 1 / 16.0f;
		// AudioDevice::TestThing(freq);
		fileTest--;
		// loadPCMFile(fileTest);
	}
	if (Input::IsButtonPressed(INPUT_UP))
	{
		// currentPlayer->duty += 1 / 16.0f;
		// AudioDevice::TestThing(freq);

		fileTest++;
		// loadPCMFile(fileTest);
	}

	auto mousePan = Math::lerp(-1, 1, ((float)Input::mouseX) / (SCREEN_XSIZE * Engine.windowScale));
	auto mouseFreq = -(Input::mouseY - (SCREEN_YSIZE * Engine.windowScale));
	// AudioDevice::SetPCMFreq(mouseFreq);
	// AudioDevice::SetPCMPan(0.0f);
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
	PPU::ClearScreen(9);
	Palette::SetActivePalette(0);

#if false
	for (int32 y = -ry; y < ry; y += 3)
	{
		for (int32 x = -rx; x < rx; x += 2)
		{
			float32 dist = std::sqrt(static_cast<float32>(x * x + y * y));
			float32 z = std::cos(dist / 6.0f - (t() * 8)) * 6.0f;
			PPU::SetPixel(rx + x, static_cast<int32>(ry + y - z), 4);
		}
	}
#endif

	// PPU::SetScreenPosition(-State.cameraPos.x, -State.cameraPos.y);

	PPU::DrawRectangle(0, 0, 32, 32, ACTIVE_PALETTE_ENTRY_TO_RGB565(1));

	// Palette::SetActivePalette(1);
	// PPU::DrawBackground(&testImage2, 0, 192);
	
	// Palette::SetActivePalette(0);
	// PPU::DrawBackground(&testImage1, 0, 184);

	// Palette::SetActivePalette(2);
	// PPU::DrawSprite(&testSprite, 64, 168);
}

void PlayerEntity::Update()
{

}

void PlayerEntity::Render()
{

}
