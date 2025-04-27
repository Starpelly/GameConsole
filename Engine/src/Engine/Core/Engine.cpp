#include "Engine.hpp"
#include <iostream>
#include <vector>

using namespace Soulcast;

SoulcastEngine Soulcast::Engine = SoulcastEngine();

bool SoulcastEngine::Init(const char* workingDirectory, SDL_Window* window)
{
	if (std::filesystem::exists(static_cast<string>(workingDirectory)))
	{
		std::filesystem::current_path(static_cast<string>(workingDirectory));
	}
	else
	{
		return false;
	}

    Engine.initialized = true;

	Input::Init();
	AudioDevice::Init();
	Drawing::Init();
	ScriptingEngine::Init();

	return 0;
}

void SoulcastEngine::Release()
{
    Engine.initialized = false;

	ScriptingEngine::Release();
	Drawing::Release();
	AudioDevice::Release();
	Input::Release();

#if SOULCAST_USING_SDL3
	SDL_Quit();
#endif
}