#pragma once

namespace Soulcast
{
	namespace Platform
	{
		// Opens a file and sets the handle, or returns an empty handle if it fails
		Ref<File> FileOpen(const char* path, FileMode mode);

		// Returns true if a file with the given path exists
		bool FileExists(const char* path);

		// Returns true if a file with the given path was deleted
		bool FileDelete(const char* path);

		// Returns true if a directory with the given path was successfully created
		bool DirectoryCreate(const char* path);

		// Returns true if a directory with the given path exists
		bool DirectoryExists(const char* path);

		// Returns true if a directory with the given path was deleted
		bool DirectoryDelete(const char* path);

		// enumerates a directory and appends each file to the given list
		void DirectoryEnumerate(std::vector<FilePath>& list, const char* path, bool recursive);

		// opens a directory in the OS file explorer / finder
		void DirectoryExplore(const char* path);

		// sets the contents of the clipboard
		void SetClipboard(const char* text);

		// gets the contents of the clipboard into the given string
		const char* GetClipboard();

		// Tries to open a URL in a web browser
		void OpenURL(const char* url);
	}
}