#include "Engine/Core/Engine.hpp"
#include <vector>

using namespace Soulcast;

namespace Soulcast::Input
{
	InputButton inputDevice[INPUT_MAX];
	int32 mouseX;
	int32 mouseY;

#if SOULCAST_USING_SDL3
	std::vector<SDL_Gamepad*> gamepads;
#endif
}

void Input::Init()
{
#if SOULCAST_USING_SDL3
	// SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);

	inputDevice[INPUT_UP].keyMappings = SDL_SCANCODE_UP;
	inputDevice[INPUT_DOWN].keyMappings = SDL_SCANCODE_DOWN;
	inputDevice[INPUT_LEFT].keyMappings = SDL_SCANCODE_LEFT;
	inputDevice[INPUT_RIGHT].keyMappings = SDL_SCANCODE_RIGHT;
#endif
}

void Input::Clear()
{
}

void Input::Process()
{
#if SOULCAST_USING_SDL3
	int length = 0;
	const bool* keyState = SDL_GetKeyboardState(&length);

	for (int i = 0; i < INPUT_ANY; i++)
	{
		if (keyState[inputDevice[i].keyMappings])
		{
			inputDevice[i].setHeld();
			if (!inputDevice[INPUT_ANY].hold)
				inputDevice[INPUT_ANY].setHeld();
		}
		else
		{
			inputDevice[i].setReleased();
		}
	}

	bool isPressed = false;
	for (int i = 0; i < INPUT_ANY; i++)
	{
		if (keyState[inputDevice[i].keyMappings])
		{
			isPressed = true;
			break;
		}
	}
	if (isPressed)
	{

	}

	float x, y;
	SDL_GetMouseState(&x, &y);
	mouseX = (int32)x;
	mouseY = (int32)y;

#endif
}

void Input::Release()
{
#if SOULCAST_USING_SDL3
	// SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);
#endif
}

bool Input::IsButtonDown(InputButtons button)
{
	return inputDevice[button].hold;
}

bool Input::IsButtonPressed(InputButtons button)
{
	return inputDevice[button].press;
}
