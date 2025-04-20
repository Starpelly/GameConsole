#pragma once

#include "Soulcast.hpp"

#define SOULCAST_USING_SDL3 (true)
#define SOULCAST_INPUTDEVICE_SDL3 (true)

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

// ======================
// Render device backends
// ======================

// ===============
// General Defines
// ===============
#define SCREEN_XSIZE	(320)	// 480
#define SCREEN_YSIZE	(240)	// 270

#define REFRESH_RATE	(60)

#if SOULCAST_USING_SDL3
	#include <SDL3/SDL.h>
#endif

enum SoulcastStates
{
	ENGINE_MAINGAME,
	ENGINE_EXITGAME,
	ENGINE_PAUSE,
	ENGINE_WAIT
};

// ===============
// Engine Includes
// ===============

#include "Engine/Audio/Audio.hpp"

#include "Engine/Core/Assert.hpp"
#include "Engine/Core/Filesystem.hpp"
#include "Engine/Core/Math.hpp"
#include "Engine/Core/Platform.hpp"
#include "Engine/Core/Stream.hpp"

#include "Engine/Graphics/Animation.hpp"
#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/PPU.hpp"
#include "Engine/Graphics/Sprite.hpp"

#include "Engine/Input/Input.hpp"

#include "Engine/Scene/Scene.hpp"

namespace Soulcast
{
	class SOULCAST_API SoulcastEngine
	{
	public:
		SoulcastEngine() = default;

		bool Init();
		void Run();
		void Release();
		bool ProcessEvents();

	public:
		bool initialized			= false;
		bool running				= false;

		SoulcastStates mode			= ENGINE_MAINGAME;

		bool borderless				= false;
		bool vsync					= false;

		int scalingMode				= 0;
		int windowScale				= 4;
		int refreshRate				= REFRESH_RATE;
		int screenRefreshRate		= REFRESH_RATE;
		int targetRefreshRate		= REFRESH_RATE;

		int gameSpeed				= 1;
		bool frameStep				= false;

		SoundChip soundChip;
		PPU ppu;

	#if SOULCAST_USING_SDL3
		SDL_Window* window			= nullptr;
		SDL_Renderer* renderer		= nullptr;
		SDL_Texture* screenBuffer	= nullptr;
	#endif

		// Audio interpolation settings pls

	};

	extern SOULCAST_API SoulcastEngine Engine;
}