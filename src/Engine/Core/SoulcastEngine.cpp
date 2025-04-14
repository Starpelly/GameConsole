#include "SoulcastEngine.hpp"

using namespace Soulcast;

SoulcastEngine Soulcast::Engine = SoulcastEngine();

bool SoulcastEngine::Init()
{
	initialized = true;
	running = true;

#if SOULCAST_USING_SDL3
	SDL_Init(SDL_INIT_VIDEO);

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

	Engine.window = SDL_CreateWindow("Soulcast", SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale, SDL_WINDOW_HIGH_PIXEL_DENSITY | flags);

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

	SDL_SetRenderLogicalPresentation(Engine.renderer, SCREEN_XSIZE, SCREEN_YSIZE, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
	SDL_SetRenderDrawBlendMode(Engine.renderer, SDL_BLENDMODE_BLEND);

	Engine.screenBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SCREEN_XSIZE, SCREEN_YSIZE);
	SDL_SetTextureScaleMode(Engine.screenBuffer, SDL_SCALEMODE_NEAREST);

	if (!Engine.screenBuffer)
	{
		printf("ERROR: Failed to create screen buffer!\nError Message: %s", SDL_GetError());
	}
#endif

	if (Engine.borderless)
	{
		SDL_RestoreWindow(window);
		SDL_SetWindowBordered(window, false);
	}

	SDL_SetWindowPosition(Engine.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

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

	Engine.frameBuffer = new uint16[SCREEN_XSIZE * SCREEN_YSIZE];
	memset(Engine.frameBuffer, 0, (SCREEN_XSIZE * SCREEN_YSIZE) * sizeof(uint16));

	Drawing::Init();
}

void SoulcastEngine::Run()
{
	uint64 targetFreq = SDL_GetPerformanceFrequency() / Engine.refreshRate;
	uint64 curTicks = 0;
	uint64 prevTicks = 0;

	while (running)
	{
		if (!vsync)
		{
			curTicks = SDL_GetPerformanceCounter();
			if (curTicks < prevTicks + targetFreq)
				continue;
			prevTicks = curTicks;
		}

		running = ProcessEvents();

		for (int s = 0; s < gameSpeed; ++s)
		{
			Drawing::ClearScreen();
			Drawing::DrawRectangle(4, 4, 16, 16);
		}

#if SOULCAST_USING_OPENGL && SOULCAST_USING_SDL3
		SDL_GL_SwapWindow(Engine.window);
#endif

		Drawing::Present();
	}
}

void SoulcastEngine::Shutdown()
{
	Drawing::Shutdown();

#if SOULCAST_USING_SDL3
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
		case SDL_EVENT_QUIT:
			return false;
		}
	}
#endif

	return true;
}
