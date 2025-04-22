#pragma once

namespace Soulcast
{
	class ScriptingEngine
	{
	public:
		static void Init();
		static void Release();

		static void UpdateScripts();
		static void RenderScripts();

	private:
		static bool LoadScript(const char* path);
		static void StartHotloader();
		static void HotloadActive();
	};
}