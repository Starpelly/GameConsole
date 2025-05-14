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

enum EmulatorStates
{
	EMU_MAINGAME,
	EMU_EXITGAME,
	EMU_PAUSE,
	EMU_WAIT
};

// ===============
// Engine Includes
// ===============

#include "Engine/Audio/Audio.hpp"

#include "Engine/Core/Assert.hpp"
#include "Engine/Core/Math.hpp"

#include "Engine/CPU/CPU.hpp"

#include "Engine/Graphics/Palette.hpp"
#include "Engine/Graphics/Drawing.hpp"
#include "Engine/Graphics/Sprite.hpp"

#include "Engine/Input/Input.hpp"

#include "Engine/Platform/Filesystem.hpp"
#include "Engine/Platform/Platform.hpp"
#include "Engine/Platform/Stream.hpp"

#include "Engine/Scene/Scene.hpp"

namespace Soulcast
{
	class Emulator;

	class SOULCAST_API SoulcastEngine
	{
	public:
		SoulcastEngine() = default;

		bool Init(const char* workingDir, SDL_Window* window = nullptr);
		void Release();

	public:
		bool initialized			= false;
		Emulator* runningEmulator	= nullptr;
	};

	extern SOULCAST_API SoulcastEngine Engine;
}
