// Modified from https://github.com/NoelFB/blah/blob/master/include/blah_filesystem.h
// License: MIT

#pragma once

namespace Soulcast
{
	class File;

	using FilePath = std::filesystem::path;

	enum class FileMode
	{
		// Opens an existing file for reading.
		OpenRead,

		// Opens an existing file for reading and writing.
		Open,

		// Creates a new file or overwrites an existing file for writing.
		CreateWrite,

		// Creates a new file or overwrites an existing file for reading and writing.
		Create,
	};

	class File
	{
	protected:
		File() = default;

	public:
		// Opens a file at the given path.
		// If it fails, this will return an empty reference.
		static Ref<File> Open(const FilePath& path, FileMode mode);

		// checks if the given file exists
		static bool Exists(const FilePath& path);

		// deletes the given file
		static bool Destroy(const FilePath& path);

		// Default Destructor
		virtual ~File() = default;

		// Gets the Mode the File was opened with
		FileMode Mode() const;

		// Gets the File Length
		virtual size_t length() = 0;

		// Gets the current File Position
		virtual size_t position() = 0;

		// Seeks to the given position in the File
		virtual size_t seek(size_t position) = 0;

		// Reads from the File into the buffer, and returns how many bytes were successfully read
		virtual size_t read(void* buffer, size_t length) = 0;

		// Writes from the buffer into the File, nd returns how many bytes were successfully written
		virtual size_t write(const void* buffer, size_t length) = 0;

	private:
		FileMode m_mode = FileMode::Open;
	};

	namespace Directory
	{
		// Creates a new directory at the given location.
		// Returns false if unable to create the directory.
		bool Create(const FilePath& path);

		// Returns whether the given directory exists
		bool Exists(const FilePath& path);

		// Tries to delete a path and returns whether it was successful
		bool Destroy(const FilePath& path);

		// Enumerates over a directory and returns a list of files & directories
		std::vector<FilePath> Enumerate(const FilePath& path, bool recursive = true);

		// Opens the path in the File Explorer / Finder
		void Explore(const FilePath& path);
	}

	namespace Path
	{
		// Returns the file name of the path
		FilePath GetFileName(const FilePath& path);

		// Returns the file name of the path, without the file extension
		FilePath GetFileNameNoExtension(const FilePath& path);

		// Returns the path without any file extensions
		FilePath GetPathNoExtension(const FilePath& path);

		// Returns relative path
		FilePath GetPathAfter(const FilePath& path, const FilePath& after);

		// Gets the top directory name from the path
		FilePath GetDirectoryName(const FilePath& path);

		// Normalizes a path (removes ../, changes \\ to /, removes redundant slashes, etc)
		FilePath Normalize(const FilePath& path);

		// Joins two paths together
		FilePath Join(const FilePath& a, const FilePath& b);

		// Joins two paths together
		template<typename ... Args>
		FilePath Join(const FilePath& a, const FilePath& b, const Args&... args)
		{
			return Join(a, Join(b, args...));
		}
	}
}