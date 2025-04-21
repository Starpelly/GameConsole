#include "Engine/Core/SoulcastEngine.hpp"

#include <kaguya/kaguya.hpp>

#define LUA_NAME "soul"

using namespace Soulcast;

static uint16 PaletteIndexToRGB565(int32 entry)
{
	return ACTIVE_PALETTE_ENTRY_TO_RGB565(entry);
}

char characters[74] =
{
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'!',
	'.',
	'-',
	',',
	'?',
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z',
	'#',
	'(',
	')',
	'\'',
	'*',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'/',
	':',
};

Bitmap fontBitmap;
Sprite font;

kaguya::State state;
bool hadErrors = false;
const char* errorStr;

static void DrawString(const std::string& text, int32 x, int32 y, int32 width)
{
	int32 charX = x;
	int32 charY = y;

#define continue_x() charX += 8; continue;
#define new_line() charX = x; charY += 8;

	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] == ' ')
		{
			continue_x();
		}
		if (text[i] == '\n')
		{
			new_line();
			continue;
		}
		if (charX >= width)
		{
			new_line();
		}

		int32 sprX = 0;
		for (int c = 0; c < 74; c++)
		{
			if (characters[c] == text[i])
			{
				sprX = c * 8;
				break;
			}
		}

		PPU::DrawSpriteRegion(&font, charX, charY, sprX, 0, 8, 8);

		charX += 8;
	}
}

static void HandleError(int errCode, const char* szError)
{
	errorStr = szError;
	std::cout << errorStr << std::endl;
}

void ScriptingEngine::Init()
{
	fontBitmap.Load("Sprites/font.png");
	font.bitmap = &fontBitmap;

	// Init lua
	state.setErrorHandler(HandleError);

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
	if (!state.dofile("Scripts/Test.lua"))
	{
		hadErrors = true;
	}
	else
	{
		// Call init function
		state[LUA_NAME]["init"]();
	}
}

void ScriptingEngine::Release()
{
}

void ScriptingEngine::UpdateScripts()
{
	if (hadErrors) return;

	state[LUA_NAME]["update"]();
}

void ScriptingEngine::RenderScripts()
{
	// PPU::ClearScreen(9);

	if (hadErrors)
	{
		PPU::ClearScreen(RGB888_TO_RGB565(147, 0, 0));
		PPU::SetColorMode(ColorMode::Direct);

		DrawString("Script Error:", 8, 8, SCREEN_XSIZE);
		DrawString(errorStr, 8, 32, SCREEN_XSIZE - 8);
	}
	else
	{
		state[LUA_NAME]["render"]();
	}
}
