#pragma once

#define SOULCAST_USING_SDL3 (true)
#define SOULCAST_INPUTDEVICE_SDL3 (true)

// =============
// Standard Libs
// =============
#include <stdio.h>
#include <string>
#include <cmath>

// ==============
// Standard Types
// ==============
#include <cstdint>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

struct Color
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

struct Vector2
{
	int32 x;
	int32 y;
};

// =========
// Platforms
// =========
#define SOULCAST_WIN	(0)
#define SOULCAST_LINUX	(1)

#if defined _WIN32
	#define SOULCAST_PLATFORM (SOULCAST_WIN)
#elif defined __linux__
	#define SOULCAST_PLATFORM (SOULCAST_LINUX)
#endif

// ======================
// Render device backends
// ======================

// ===============
// General Defines
// ===============
#define SCREEN_XSIZE	(320)
#define SCREEN_YSIZE	(180)

#define REFRESH_RATE	(60)

#if SOULCAST_USING_SDL3
	#include <SDL3/SDL.h>
#endif

// ===============
// Engine Includes
// ===============

#include "Engine/Graphics/Drawing.hpp"
#include "Engine/Graphics/Palette.hpp"
#include "Engine/Input/Input.hpp"

namespace Soulcast
{

class SoulcastEngine
{
public:
	SoulcastEngine() = default;

	bool Init();
	void Run();
	void Shutdown();
	bool ProcessEvents();

public:
	bool initialized			= false;
	bool running				= false;
	bool borderless				= false;
	bool vsync					= false;

	int scalingMode				= 0;
	int windowScale				= 4;
	int refreshRate				= REFRESH_RATE;
	int screenRefreshRate		= REFRESH_RATE;
	int targetRefreshRate		= REFRESH_RATE;

	int gameSpeed				= 1;
	bool frameStep				= false;

	uint16* frameBuffer			= nullptr;
	uint32* texBuffer			= nullptr;

#if SOULCAST_USING_SDL3
	SDL_Window* window			= nullptr;
	SDL_Renderer* renderer		= nullptr;
	SDL_Texture* screenBuffer	= nullptr;
#endif
};

extern SoulcastEngine Engine;

}