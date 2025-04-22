#include "Engine/Core/Engine.hpp"

#include <kaguya/kaguya.hpp>

#if SOULCAST_HOTLOADING
#include <efsw/efsw.hpp>
#include <queue>
#endif

#include <fstream>
#include <sstream>

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

lua_State* L = nullptr;
bool hadErrors = false;
std::string errorStr;

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
	hadErrors = true;
}

#if SOULCAST_HOTLOADING

std::queue<std::filesystem::path> hotloadingQueue;

/// Processes a file action
class UpdateListener : public efsw::FileWatchListener {
public:
	UpdateListener() {}

	string getActionName(efsw::Action action) {
		switch (action) {
		case efsw::Actions::Add:
			return "Add";
		case efsw::Actions::Modified:
			return "Modified";
		case efsw::Actions::Delete:
			return "Delete";
		case efsw::Actions::Moved:
			return "Moved";
		default:
			return "Bad Action";
		}
	}

	void handleFileAction(efsw::WatchID watchid, const string& dir,
		const string& filename, efsw::Action action,
		string oldFilename = "") override
	{
		/*
		std::cout << "Watch ID " << watchid << " DIR ("
			<< dir + ") FILE (" +
			(oldFilename.empty() ? "" : "from file " + oldFilename + " to ") +
			filename + ") has event "
			<< getActionName(action) << std::endl;
			*/
		if (action == efsw::Actions::Modified)
		{
			hotloadingQueue.push(dir + filename);
		}
	}
};

#endif

static std::string extractHotloadChunk(const std::string& filePath)
{
	std::ifstream file(filePath);
	std::string line;
	std::ostringstream chunk;
	bool inHotload = false;

	while (std::getline(file, line))
	{
		if (line.find("--[[HOTLOADABLE]]") != std::string::npos)
		{
			inHotload = true;
		}
		else if (line.find("--[[END]]") != std::string::npos)
		{
			inHotload = false;
		}
		else if (inHotload) {
			chunk << line << "\n";
		}
	}

	return chunk.str();
}

void ScriptingEngine::Init()
{
	fontBitmap.Load("Sprites/font.png");
	font.bitmap = &fontBitmap;

	InitLua();
	StartROM();

	StartHotloader();
}

void ScriptingEngine::Release()
{
}

void ScriptingEngine::UpdateScripts()
{
	HotloadActive();

	if (!hadErrors)
	{
		kaguya::State state(L);
		state[LUA_NAME]["update"]();
	}
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
		kaguya::State state(L);
		state[LUA_NAME]["render"]();
	}
}

void ScriptingEngine::Reset()
{
	if (L)
	{
		lua_close(L);
	}
	hadErrors = false;

	InitLua();
	StartROM();
}

/// =======================================================================================================================
/// Private Methods
/// =======================================================================================================================

void ScriptingEngine::InitLua()
{
	// Init lua
	L = luaL_newstate();
	luaL_openlibs(L);

	kaguya::State state(L);
	state.setErrorHandler(HandleError);

	// Create console table
	state[LUA_NAME] = kaguya::NewTable();

	// Global functions
	{
		// Memory
		state[LUA_NAME]["memory"] = kaguya::NewTable();
		state[LUA_NAME]["memory"]["read"] = &Memory::Peek;
		state[LUA_NAME]["memory"]["write"] = &Memory::Poke;

		// Drawing
		state[LUA_NAME]["clearScreen"] = &PPU::ClearScreen;
		state[LUA_NAME]["setScreenPosition"] = &PPU::SetScreenPosition;
		state[LUA_NAME]["drawRectangle"] = &PPU::DrawRectangle;
		state[LUA_NAME]["drawBackground"] = &PPU::DrawBackground;
		state[LUA_NAME]["drawSprite"] = &PPU::DrawSprite;
		state[LUA_NAME]["drawSpriteRegion"] = &PPU::DrawSpriteRegion;

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
}

void ScriptingEngine::StartROM()
{
	// Load test file
	LoadScript("Scripts/Main.lua");

	kaguya::State state(L);
	state[LUA_NAME]["init"]();
}

bool ScriptingEngine::LoadScript(const char* path)
{
	kaguya::State state(L);
	if (!state.dofile(path))
	{
		hadErrors = true;
	}

	return !hadErrors;
}

void ScriptingEngine::StartHotloader()
{
#if SOULCAST_HOTLOADING
	// @NOTE: 
	// This is problematic...
	efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();
	UpdateListener* listener = new UpdateListener();

	efsw::WatchID watchID = fileWatcher->addWatch("D:/Soulcast/test/Sandbox/Data/Scripts", listener, true);

	fileWatcher->watch();
#endif
}

void ScriptingEngine::HotloadActive()
{
#if SOULCAST_HOTLOADING
	int errorCount = 0;
	int hotloadCount = hotloadingQueue.size();
	while (!hotloadingQueue.empty())
	{
		auto hotloadPath = hotloadingQueue.front();
		hotloadingQueue.pop();

		std::string code = extractHotloadChunk(hotloadPath.string());

		if (luaL_loadstring(L, code.c_str()) != LUA_OK)
		{
			errorCount++;
			errorStr = lua_tostring(L, -1);

			std::cerr << "Lua hotload error: " << lua_tostring(L, -1) << "\n";
			lua_pop(L, 1);
		}
		else
		{
			if (lua_pcall(L, 0, 0, 0) != LUA_OK)
			{
				errorCount++;
				errorStr = lua_tostring(L, -1);
		
				std::cerr << "Error running hotload chunk: " << lua_tostring(L, -1) << "\n";
				lua_pop(L, 1);
			}
			else
			{
				std::cout << "[Hotloaded update/render]\n";
			}
		}
	}

	if (hotloadCount > 0)
	{
		hadErrors = errorCount > 0;
	}

#endif
}
