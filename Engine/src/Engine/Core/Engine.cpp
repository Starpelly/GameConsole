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

	// AudioDevice::Init(&Engine.soundChip.state);
	Input::Init();
	Drawing::Init();
	ScriptingEngine::Init();

	return 0;
}

void SoulcastEngine::Release()
{
    Engine.initialized = false;

	ScriptingEngine::Release();
	Drawing::Release();
	Input::Release();
	AudioDevice::Release();

#if SOULCAST_USING_SDL3
	SDL_Quit();
#endif
}