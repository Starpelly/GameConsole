#pragma once

#include "Engine/Core/Engine.hpp"

namespace Soulcast
{
	class SOULCAST_API Emulator
	{
	public:
		Emulator() = default;
		~Emulator() { Release(); }

		bool Init(SDL_Window* window = nullptr);
		void Run();
		void DoOneFrame();
		void Release();

		void ResetSystem();

#if SOULCAST_USING_SDL3
		bool ProcessEvent(const SDL_Event& event);
#endif
	public:
		EmulatorStates mode = EMU_MAINGAME;

		bool initialized = false;
		bool running = false;

		bool borderless = false;
		bool vsync = false;
		bool windowContained = false;

		int scalingMode = 0;
		int windowScale = 4;
		int refreshRate = REFRESH_RATE;
		int screenRefreshRate = REFRESH_RATE;
		int targetRefreshRate = REFRESH_RATE;

		int gameSpeed = 1;
		bool frameStep = false;
		bool masterPaused = false;
		double time = 0.0;

		uint64 targetFreq = 0;
		uint64 curTicks = 0;
		uint64 prevTicks = 0;

#if SOULCAST_USING_SDL3
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
#endif

	public:
		ScreenInfo gameScreen;

		// Audio interpolation settings pls
	};
}