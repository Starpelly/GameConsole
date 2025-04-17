#include "Engine/Core/SoulcastEngine.hpp"

#if SOULCAST_PLATFORM == SOULCAST_WIN
	#include "Windows.h"
#endif

namespace Soulcast
{
	struct SDL3_File : public File
	{
		SDL_IOStream* handle;
		SDL3_File(SDL_IOStream* handle) : handle(handle) {}
		~SDL3_File() { if (handle) SDL_CloseIO(handle); }
		size_t length() override { return SDL_GetIOSize(handle); }
		size_t position() override { return SDL_TellIO(handle); }
		size_t seek(size_t position) override { return SDL_SeekIO(handle, position, SDL_IO_SEEK_SET); }
		size_t read(void* buffer, size_t length) override
		{
			if (sizeof(char) > 0 && length > 0)
				return SDL_ReadIO(handle, buffer, sizeof(char) * length) / sizeof(char);

			return 0;
		}
		size_t write(const void* buffer, size_t length) override
		{
			if (sizeof(char) > 0 && length > 0)
				return SDL_WriteIO(handle, buffer, sizeof(char) * length) / sizeof(char);
			return 0;
		}
	};

	char* sdl3_clipboard_text = nullptr;
}


using namespace Soulcast;

Ref<File> Platform::FileOpen(const char* path, FileMode mode)
{
	const char* sdl_mode = "";

	switch (mode)
	{
	case FileMode::OpenRead:
		sdl_mode = "rb";
		break;
	case FileMode::Open:
		sdl_mode = "r+b";
		break;
	case FileMode::CreateWrite:
		sdl_mode = "wb";
		break;
	case FileMode::Create:
		sdl_mode = "w+b";
		break;
	}

	auto ptr = SDL_IOFromFile(path, sdl_mode);
	if (!ptr)
		return Ref<File>();

	return Ref<File>(new SDL3_File(ptr));
}

bool Platform::FileExists(const char* path)
{
	return std::filesystem::is_regular_file(path);
}

bool Platform::FileDelete(const char* path)
{
	return std::filesystem::remove(path);
}

bool Platform::DirectoryCreate(const char* path)
{
	return std::filesystem::create_directories(path);
}

bool Platform::DirectoryExists(const char* path)
{
	return std::filesystem::is_directory(path);
}

bool Platform::DirectoryDelete(const char* path)
{
	return std::filesystem::remove_all(path) > 0;
}

void Platform::DirectoryEnumerate(std::vector<FilePath>& list, const char* path, bool recursive)
{
	if (std::filesystem::is_directory(path))
	{
		if (recursive)
		{
			for (auto& p : std::filesystem::recursive_directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
		else
		{
			for (auto& p : std::filesystem::directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
	}
}

void Platform::DirectoryExplore(const char* path)
{
#if SOULCAST_PLATFORM == SOULCAST_WIN

	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);

#elif SOULCAST_PLATFORM == SOULCAST_LINUX
	system(String::fmt("xdg-open \"%s\"", path).cstr());

#else

	system(String::fmt("open \"%s\"", path).cstr());

#endif
}

void Platform::SetClipboard(const char* text)
{
	SDL_SetClipboardText(text);
}

const char* Platform::GetClipboard()
{
	// free previous clipboard text
	if (sdl3_clipboard_text != nullptr)
		SDL_free(sdl3_clipboard_text);

	sdl3_clipboard_text = SDL_GetClipboardText();
	return sdl3_clipboard_text;
}

void Platform::OpenURL(const char* url)
{
	SDL_OpenURL(url);
}