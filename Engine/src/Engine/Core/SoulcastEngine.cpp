#include "SoulcastEngine.hpp"
// #include "TestGame.hpp"
#include <iostream>

using namespace Soulcast;

SoulcastEngine Soulcast::Engine = SoulcastEngine();

bool SoulcastEngine::Init()
{
	auto workingDirectory = "D:/Soulcast/test/Sandbox/Data";

	if (std::filesystem::exists(static_cast<string>(workingDirectory)))
	{
		std::filesystem::current_path(static_cast<string>(workingDirectory));
	}
	else
	{
		return false;
	}

	initialized = true;
	running = true;
	mode = ENGINE_MAINGAME;

#if SOULCAST_USING_SDL3
	SDL_Init(SDL_INIT_EVENTS);

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, Engine.vsync ? "1" : "0");
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

	uint8 flags = 0;

#if SOULCAST_USING_OPENGL
	flags |= SDL_WINDOW_OPENGL;

#if SOULCAST_PLATFORM != SOULCAST_ANDROID
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#endif

	flags |= SDL_WINDOW_RESIZABLE;

	int32 windowWidth = SCREEN_XSIZE * Engine.windowScale;
	int32 windowHeight = SCREEN_YSIZE * Engine.windowScale;

	if (Engine.debugMode)
	{
		windowWidth += DEBUG_XSIZE * Engine.windowScale;
	}

	Engine.window = SDL_CreateWindow("Soulcast", windowWidth, windowHeight, SDL_WINDOW_HIGH_PIXEL_DENSITY | flags);

	if (!Engine.window)
	{
		printf("ERROR: Failed to created window!\nError Message: %s", SDL_GetError());
		return 0;
	}

#if !SOULCAST_USING_OPENGL
	Engine.renderer = SDL_CreateRenderer(Engine.window, NULL);

	if (!Engine.renderer)
	{
		printf("ERROR: Failed to create renderer!\nError Message: %s", SDL_GetError());
		return 0;
	}

	SDL_SetRenderLogicalPresentation(Engine.renderer, SCREEN_XSIZE, SCREEN_YSIZE, (Engine.debugMode) ? SDL_LOGICAL_PRESENTATION_DISABLED : SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
	SDL_SetRenderDrawBlendMode(Engine.renderer, SDL_BLENDMODE_BLEND);

	// Game screen buffer texture
	{
		Engine.screenBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SCREEN_XSIZE, SCREEN_YSIZE);
		SDL_SetTextureScaleMode(Engine.screenBuffer, SDL_SCALEMODE_NEAREST);

		if (!Engine.screenBuffer)
		{
			printf("ERROR: Failed to create screen buffer!\nError Message: %s", SDL_GetError());
		}
	}

	// Debug screen buffer texture
	if (Engine.debugMode)
	{
		Engine.dbScreenBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, DEBUG_XSIZE, SCREEN_YSIZE);
		SDL_SetTextureScaleMode(Engine.dbScreenBuffer, SDL_SCALEMODE_NEAREST);

		if (!Engine.dbScreenBuffer)
		{
			printf("ERROR: Failed to create screen buffer!\nError Message: %s", SDL_GetError());
		}
	}

#endif

	if (Engine.borderless)
	{
		SDL_RestoreWindow(window);
		SDL_SetWindowBordered(window, false);
	}

	SDL_SetWindowPosition(Engine.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(Engine.window, !Engine.debugMode);

	SDL_DisplayID winID = SDL_GetDisplayForWindow(Engine.window);
	const SDL_DisplayMode* disp = SDL_GetCurrentDisplayMode(winID);
	if (disp != nullptr)
	{
		Engine.screenRefreshRate = disp->refresh_rate;
	}
	else
	{
		printf("ERROR: %s", SDL_GetError());
	}
#endif

	AudioDevice::Init();
	Input::Init();
	PPU::Init();
	ScriptingEngine::Init();

	return 0;
}

void SoulcastEngine::Run()
{	
	uint64 targetFreq = SDL_GetPerformanceFrequency() / Engine.refreshRate;
	uint64 curTicks = 0;
	uint64 prevTicks = 0;

	bool masterPaused = false;

	// TestGame testGame;

	while (Engine.running)
	{
		if (!Engine.vsync)
		{
			curTicks = SDL_GetPerformanceCounter();
			if (curTicks < prevTicks + targetFreq)
				continue;
			prevTicks = curTicks;
		}

		Engine.running = ProcessEvents();

		PPU::SetActiveScreen(&PPU::gameScreen);

		for (int s = 0; s < Engine.gameSpeed; ++s)
		{
			Input::Process();

			if (Engine.debugMode)
			{
				if (Input::IsButtonPressed(INPUT_UP))
				{
					debug.palette++;
				}
				if (Input::IsButtonPressed(INPUT_DOWN))
				{
					debug.palette--;
				}
				if (debug.palette < 0)
					debug.palette = 0;
				if (debug.palette >= PALETTE_BANK_COUNT)
					debug.palette = PALETTE_BANK_COUNT;
			}

			if (!masterPaused || Engine.frameStep)
			{
				switch (Engine.mode)
				{
				case ENGINE_MAINGAME:
					// testGame.Update();
					// testGame.Render();

					ScriptingEngine::UpdateScripts();
					ScriptingEngine::RenderScripts();
					break;

				case ENGINE_EXITGAME:
					Engine.running = false;
					break;

				case ENGINE_PAUSE:
				case ENGINE_WAIT:
					break;

				default: break;
				}
			}
		}

		// Render debug stuff (if we should)
		if (Engine.debugMode)
		{
			PPU::SetActiveScreen(&PPU::debugScreen);
			PPU::ClearScreen(RGB888_TO_RGB565(0, 8, 133));

			for (int i = 0; i < PALETTE_BANK_COUNT; i++)
			{
				PPU::RenderPalette(i, i * 24);
			}
		}

		// Render PPU output to physical screen!
		PPU::Present();

#if SOULCAST_USING_OPENGL && SOULCAST_USING_SDL3
		SDL_GL_SwapWindow(Engine.window);
#endif

		Engine.frameStep = false;
	}

	Release();
}

void SoulcastEngine::Release()
{
	ScriptingEngine::Release();
	PPU::Release();
	Input::Release();
	AudioDevice::Release();

#if SOULCAST_USING_SDL3
	if (Engine.debugMode)
	{
		SDL_DestroyTexture(Engine.dbScreenBuffer);
	}
	SDL_DestroyTexture(Engine.screenBuffer);
	SDL_DestroyRenderer(Engine.renderer);
	SDL_DestroyWindow(Engine.window);

	SDL_Quit();
#endif
}

bool SoulcastEngine::ProcessEvents()
{
#if SOULCAST_USING_SDL3
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_TERMINATING:
			Engine.mode = ENGINE_EXITGAME; return false;
		case SDL_EVENT_QUIT:
			Engine.mode = ENGINE_EXITGAME; return false;

		case SDL_EVENT_KEY_DOWN:

			break;
		}
	}
#endif

	return true;
}
