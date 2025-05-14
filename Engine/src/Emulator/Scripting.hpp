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

		static void Reset();

	private:
		static void InitLua();
		static void StartROM();

		static bool LoadScript(const char* path);
		static void StartHotloader();
		static void HotloadActive();
	};
}