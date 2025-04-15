#include "Engine/Core/SoulcastEngine.hpp"
#include "Engine/Core/CPU.hpp"

using namespace Soulcast;

#if SOULCAST_PLATFORM == SOULCAST_WIN && _MSC_VER
	#include "Windows.h"
#endif

int main(int argc, char* argv[])
{
	Soulcast::Engine.Init();
	Soulcast::Engine.Run();
	Soulcast::Engine.Release();

	return 0;
}