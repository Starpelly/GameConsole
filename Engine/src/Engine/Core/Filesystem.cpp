#include "Engine/Core/SoulcastEngine.hpp"

namespace Soulcast
{
	Ref<File> File::Open(const FilePath& path, FileMode mode)
	{
		SOULCAST_ASSERT_RUNNING();

		Ref<File> ref;
		if (Engine.running)
			ref = Platform::FileOpen(path.string().c_str(), mode);
		if (ref)
			ref->m_mode = mode;
		return ref;
	}

	bool File::Exists(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			return Platform::FileExists(path.string().c_str());
		return false;
	}

	bool File::Destroy(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			return Platform::FileDelete(path.string().c_str());
		return false;
	}

	FileMode File::Mode() const
	{
		return m_mode;
	}

	bool Directory::Create(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			return Platform::DirectoryCreate(path.string().c_str());
		return false;
	}

	bool Directory::Exists(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			return Platform::DirectoryExists(path.string().c_str());
		return false;
	}

	bool Directory::Destroy(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			return Platform::DirectoryDelete(path.string().c_str());
		return false;
	}

	std::vector<FilePath> Directory::Enumerate(const FilePath& path, bool recursive)
	{
		SOULCAST_ASSERT_RUNNING();

		std::vector<FilePath> list;

		if (Engine.running)
		{
			Platform::DirectoryEnumerate(list, path.string().c_str(), recursive);
			for (auto& it : list)
			{
				for (int n = 0; n < it.string().length(); n++)
					if (it.string()[n] == '\\') it.string()[n] = '/';
			}
		}

		return list;
	}

	void Directory::Explore(const FilePath& path)
	{
		SOULCAST_ASSERT_RUNNING();
		if (Engine.running)
			Platform::DirectoryExplore(path.string().c_str());
	}

	FilePath Path::GetFileName(const FilePath& path)
	{
		return path.filename();
	}

	FilePath Path::GetFileNameNoExtension(const FilePath& path)
	{
		FilePath filename = GetFileName(path);
		return GetPathNoExtension(filename);
	}

	FilePath Path::GetPathNoExtension(const FilePath& path)
	{
		return path.stem().string();
	}

	FilePath Path::GetDirectoryName(const FilePath& path)
	{
		return path.parent_path();
	}

	FilePath Path::GetPathAfter(const FilePath& path, const FilePath& after)
	{
		std::string pathStr = path.generic_string();
		std::string afterStr = after.generic_string();

		size_t pos = pathStr.find(afterStr);
		if (pos != std::string::npos) {
			return FilePath(pathStr.substr(pos + afterStr.length()));
		}

		return{};
	}

	FilePath Path::Normalize(const FilePath& path)
	{
		return path.lexically_normal();
	}

	FilePath Path::Join(const FilePath& a, const FilePath& b)
	{
		if (a.empty()) return b.lexically_normal();
		if (b.empty()) return a.lexically_normal();
		return (a / b).lexically_normal();
	}
}