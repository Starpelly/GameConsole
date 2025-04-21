#include "Engine/Core/SoulcastEngine.hpp"

#include <kaguya/kaguya.hpp>

#define LUA_NAME "soul"

kaguya::State state;
int render;

using namespace Soulcast;

static uint16 PaletteIndexToRGB565(int32 entry)
{
	return ACTIVE_PALETTE_ENTRY_TO_RGB565(entry);
}

void ScriptingEngine::Init()
{
	// Create console table
	state[LUA_NAME] = kaguya::NewTable();

	// Global functions
	{
		// Drawing
		state[LUA_NAME]["clearScreen"] = &PPU::ClearScreen;
		state[LUA_NAME]["drawRectangle"] = &PPU::DrawRectangle;
		state[LUA_NAME]["drawBackground"] = &PPU::DrawBackground;
		state[LUA_NAME]["drawSprite"] = &PPU::DrawSprite;
		state[LUA_NAME]["setScreenPosition"] = &PPU::SetScreenPosition;

		// Palettes
		state[LUA_NAME]["loadPalette"] = &Palette::LoadPaletteBank;
		state[LUA_NAME]["setActivePalette"] = &Palette::SetActivePalette;
		state[LUA_NAME]["paletteIndexToRGB565"] = &PaletteIndexToRGB565;

		// Input
		state[LUA_NAME]["gamepad"] = kaguya::NewTable();
		state[LUA_NAME]["gamepad"]["isDown"] = &Input::IsButtonDown;
		state[LUA_NAME]["gamepad"]["isPressed"] = &Input::IsButtonPressed;
	}
	// Classes
	{
		// Vector2
		state[LUA_NAME]["vector2"].setClass(kaguya::UserdataMetatable<Vector2>()
			.setConstructors<Vector2()>()
			.addProperty("x", &Vector2::x)
			.addProperty("y", &Vector2::y)
		);

		// Sprite
		state[LUA_NAME]["sprite"].setClass(kaguya::UserdataMetatable<Sprite>()
			.setConstructors<Sprite()>()
			.addProperty("bitmap", &Sprite::bitmap)
		);

		// Bitmap
		state[LUA_NAME]["bitmap"].setClass(kaguya::UserdataMetatable<Bitmap>()
			.setConstructors<Bitmap()>()
			.addFunction("load", &Bitmap::Load)
		);
	}

	// Load test file
	state.dofile("Scripts/Test.lua");

	// Call init function
	state[LUA_NAME]["init"]();
}

void ScriptingEngine::Release()
{
}

void ScriptingEngine::UpdateScripts()
{
	state[LUA_NAME]["update"]();
}

void ScriptingEngine::RenderScripts()
{
	// PPU::ClearScreen(9);

	state[LUA_NAME]["render"]();
}
