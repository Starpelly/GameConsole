#pragma once

#include "Soulcast.hpp"

#define SOULCAST_USING_SDL3 (true)
#define SOULCAST_INPUTDEVICE_SDL3 (true)
#define SOULCAST_HOTLOADING (true)

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
constexpr auto SCREEN_XSIZE		 = (320); // 480
constexpr auto SCREEN_YSIZE		 = (240); // 270

constexpr auto DEBUG_XSIZE		 = (128);
constexpr auto SCREEN_XSIZE_WIDE = (static_cast<int>((16.0f / 9.0f) * 240));

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
#include "Engine/Core/Math.hpp"
#include "Engine/Core/Memory.hpp"
#include "Engine/Core/Scripting.hpp"

#include "Engine/CPU/CPU.hpp"

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/PPU.hpp"
#include "Engine/Graphics/Sprite.hpp"

#include "Engine/Input/Input.hpp"

#include "Engine/Platform/Filesystem.hpp"
#include "Engine/Platform/Platform.hpp"
#include "Engine/Platform/Stream.hpp"

#include "Engine/Scene/Scene.hpp"

namespace Soulcast
{
	struct DebugMode
	{
		int8 palette;
	};

	class SOULCAST_API SoulcastEngine
	{
	public:
		SoulcastEngine() = default;

		bool Init(const char* workingDir, SDL_Window* window = nullptr);
		void Run();
        void DoOneFrame();
		void Release();

        void ResetSystem();

#if SOULCAST_USING_SDL3
        bool ProcessEvent(const SDL_Event& event);
#endif

	public:
		bool initialized			= false;
		bool running				= false;
		bool debugMode				= true;
		DebugMode debug{};

		SoulcastStates mode			= ENGINE_MAINGAME;

		bool borderless				= false;
		bool vsync					= false;
        bool windowContained        = false;

		int scalingMode				= 0;
		int windowScale				= 4;
		int refreshRate				= REFRESH_RATE;
		int screenRefreshRate		= REFRESH_RATE;
		int targetRefreshRate		= REFRESH_RATE;

		int gameSpeed				= 1;
		bool frameStep				= false;
        bool masterPaused           = false;
		double time					= 0.0;

        uint64 targetFreq           = 0;
        uint64 curTicks             = 0;
        uint64 prevTicks            = 0;

		SoundChip soundChip;
		PPU ppu;

	#if SOULCAST_USING_SDL3
		SDL_Window* window			= nullptr;
		SDL_Renderer* renderer		= nullptr;
		SDL_Texture* screenBuffer	= nullptr;
		SDL_Texture* dbScreenBuffer	= nullptr;
	#endif

		// Audio interpolation settings pls

	};

	extern SOULCAST_API SoulcastEngine Engine;
}
