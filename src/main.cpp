#include "Engine/Core/SoulcastEngine.hpp"

#if SOULCAST_PLATFORM == SOULCAST_WIN && _MSC_VER
	#include "Windows.h"
#endif

int main(int argc, char* argv[])
{
	Soulcast::Engine.Init();
	Soulcast::Engine.Run();
	Soulcast::Engine.Shutdown();

	return 0;
}