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

#include "Engine/Core/Cartridge.hpp"
#include "Engine/Graphics/Drawing.hpp"
#include "Engine/Graphics/Palette.hpp"
#include "Engine/Input/Input.hpp"

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

		uint16* frameBuffer			= nullptr;
		uint32* texBuffer			= nullptr;

	#if SOULCAST_USING_SDL3
		SDL_Window* window			= nullptr;
		SDL_Renderer* renderer		= nullptr;
		SDL_Texture* screenBuffer	= nullptr;
	#endif
	};

	extern SOULCAST_API SoulcastEngine Engine;
}