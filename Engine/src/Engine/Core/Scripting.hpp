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
	};
}